#include "FluidPostProcess.h"
#include "BravoEngine.h"
#include "BravoViewport.h"
#include "BravoRenderTarget.h"
#include "BravoShaderAsset.h"
#include "BravoAssetManager.h"
#include "FluidSimulation.h"
#include "FluidContainer.h"
#include "BravoCamera.h"
#include "BravoTextureUnitManager.h"

bool FluidPostProcess::Initialize_Internal()
{
	if ( !BravoPostProcess::Initialize_Internal() )
		return false;

	Shader = Engine->GetAssetManager()->FindOrLoad<BravoShaderAsset>("FluidPostProcess", BravoShaderLoadingParams("FluidPostProcess"));
	if ( !Shader )
		return false;

	
	DensitiesMapShader = Engine->GetAssetManager()->FindOrLoad<BravoShaderAsset>("FluidDensities", BravoShaderLoadingParams("FluidDensities"));
	if ( !DensitiesMapShader )
		return false;

	glm::ivec2 Size = Engine->GetViewport()->GetViewportSize();
	DensitiesRenderTarget = NewObject<BravoRenderTarget>("FluidDensitiesTarget", Size , GL_R32F, GL_RED, GL_FLOAT, false);

	return true;
}

bool FluidPostProcess::EnsureReady()
{
	if ( !BravoPostProcess::EnsureReady() )
		return false;
	
	const std::shared_ptr<BravoCamera> camera = Engine->GetCamera();
	if ( !camera )
		return false;

	if ( Simulation == nullptr || Container == nullptr )
		return false;

	return true;
}


void FluidPostProcess::Render_Internal()
{
	assert( Simulation != nullptr && Container != nullptr );
	const std::shared_ptr<BravoCamera> camera = Engine->GetCamera();

	glm::mat4 CameraProjection = camera->GetProjectionMatrix();
	glm::mat4 CameraView = camera->GetViewMatrix();

	glm::mat4 mCamera = CameraProjection * CameraView;
	glm::mat4 model  = Container->GetTransform().GetTransformMatrix();

	DensitiesRenderTarget->Bind();
	DensitiesMapShader->Use();

		DensitiesMapShader->SetMatrix4d("camera", mCamera);
		DensitiesMapShader->SetMatrix4d("model", model);

		DensitiesMapShader->SetInt("ParticleCount", Simulation->GetParticleCount());
		DensitiesMapShader->SetFloat2("WorldSize", Simulation->GetWorldSize());
		DensitiesMapShader->SetFloat1("ParticleMass", Simulation->GetParticleMass());
		DensitiesMapShader->SetFloat1("SmoothingRadius", Simulation->GetSmoothingRadius());
		DensitiesMapShader->SetFloat1("DensityScale", Simulation->GetDensityScale());

		Draw();

	DensitiesMapShader->StopUsage();
	DensitiesRenderTarget->Unbind();

	Shader->Use();

		Shader->SetMatrix4d("model", model);
		Shader->SetMatrix4d("camera", mCamera);

		Shader->SetMatrix4d("ModelInverse", glm::inverse(model));
		Shader->SetFloat2("WorldSize", Simulation->GetWorldSize());

		Shader->SetFloat1("TargetDensity", Simulation->TargetDensity);
		Shader->SetFloat1("DensityOffset", Simulation->TargetDensity*0.15f);

		Shader->SetFloat1("LightMarchingStep", 0.1f);
		Shader->SetFloat2("DirToLight", glm::normalize(glm::vec2(-1.0f, 1.0f)));
		Shader->SetFloat3("ScatteringCoefficients", glm::vec3(1.0f, 1.0f, 1.0f));

		int32 DensitiesMap = BravoTextureUnitManager::BindTexture();
		glActiveTexture(GL_TEXTURE0 + DensitiesMap);
		glBindTexture(GL_TEXTURE_2D, DensitiesRenderTarget->GetColorTexture());
		Shader->SetInt("DensitiesMap", DensitiesMap);
		
		Draw();

		BravoTextureUnitManager::UnbindTexture(DensitiesMap);

	Shader->StopUsage();
}