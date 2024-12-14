#include "FluidPostProcess.h"
#include "BravoEngine.h"
#include "BravoAssetManager.h"
#include "BravoShaderAsset.h"
#include "BravoRenderTarget.h"
#include "BravoViewport.h"
#include "BravoTextureUnitManager.h"
#include "BravoCamera.h"

bool FluidPostProcess::Initialize_Internal()
{
	if ( !BravoPostProcess::Initialize_Internal() )
		return false;

	Shader = Engine->GetAssetManager()->FindOrLoad<BravoShaderAsset>("AntiAliasingShader", BravoShaderLoadingParams("PostProccess"));
	if ( !Shader )
		return false;

	RayMarchingCompute = Engine->GetAssetManager()->FindOrLoad<BravoShaderAsset>("FluidRayMarching", BravoShaderLoadingParams("FluidRayMarching"));
	if ( !RayMarchingCompute )
		return false;

	glm::ivec2 Size = Engine->GetViewport()->GetViewportSize();
	RenderTarget = NewObject<BravoRenderTarget>("RayTracingRenderTarget", Size / 4, GL_RGBA32F, GL_RGBA, GL_FLOAT, false);

	Engine->GetViewport()->OnResizeDelegate.AddSP(Self<FluidPostProcess>(), &FluidPostProcess::OnViewportResize);
	
	return true;
}

void FluidPostProcess::OnViewportResize(const glm::ivec2& NewSize)
{
	RenderTarget->Resize(NewSize);
}

bool FluidPostProcess::EnsureReady()
{
	if ( !Shader->EnsureGPUReady() || !RayMarchingCompute->EnsureGPUReady() )
		return false;

	return true;
}

void FluidPostProcess::Render_Internal()
{
	glm::ivec2 OutputImageSize = RenderTarget->GetSize();

	const std::shared_ptr<BravoCamera> camera = Engine->GetCamera();
	if ( !camera )
		return;


	const glm::mat4 view = camera->GetViewMatrix();
	const glm::mat4 proj = camera->GetProjectionMatrix();

	int32 OutputTextureUnit = BravoTextureUnitManager::BindTexture();
	glActiveTexture(GL_TEXTURE0 + OutputTextureUnit);
	glBindTexture(GL_TEXTURE_2D, RenderTarget->GetColorTexture());

	RayMarchingCompute->Use();
		glBindImageTexture(OutputTextureUnit, RenderTarget->GetColorTexture(), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

		RayMarchingCompute->SetInt("outputTexture", OutputTextureUnit);

		RayMarchingCompute->SetFloat3("eyeLocation", camera->GetLocation_World());
		RayMarchingCompute->SetMatrix4d("invViewMat", glm::inverse(view));
		RayMarchingCompute->SetMatrix4d("invProjMat", glm::inverse(proj));

		GLuint workGroupSizeX = (OutputImageSize.x + 15) / 16;  // Round up to the nearest multiple of 16
		GLuint workGroupSizeY = (OutputImageSize.y + 15) / 16;
		glDispatchCompute(workGroupSizeX, workGroupSizeY, 1);

		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	RayMarchingCompute->StopUsage();

	Shader->Use();
		Shader->SetInt("screenTexture", OutputTextureUnit);
		Draw();
	Shader->StopUsage();

	BravoTextureUnitManager::UnbindTexture(OutputTextureUnit);
}