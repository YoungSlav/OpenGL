#include "FluidPostProcess.h"
#include "BravoEngine.h"
#include "BravoAssetManager.h"
#include "BravoShaderAsset.h"
#include "BravoRenderTarget.h"
#include "BravoViewport.h"
#include "BravoTextureUnitManager.h"
#include "BravoCamera.h"
#include "BravoBoundingBox.h"
#include "FluidSimulation3D.h"

bool FluidPostProcess::Initialize_Internal()
{
	if ( !BravoPostProcess::Initialize_Internal() )
		return false;

	CreateDensitiesMap();
	BoundingBox->OnTransformUpdated.AddSP(Self<FluidPostProcess>(), &FluidPostProcess::OnBoundingBoxTransofrmUpdated);

	Shader = Engine->GetAssetManager()->FindOrLoad<BravoShaderAsset>("AntiAliasingShader", BravoShaderLoadingParams("PostProccess"));
	if ( !Shader )
		return false;

	RayMarchingCompute = Engine->GetAssetManager()->FindOrLoad<BravoShaderAsset>("FluidRayMarching", BravoShaderLoadingParams("FluidRayMarching"));
	if ( !RayMarchingCompute )
		return false;

	DensitiesSampleCompute = Engine->GetAssetManager()->FindOrLoad<BravoShaderAsset>("SampleDensities", BravoShaderLoadingParams("SampleDensities"));
	if ( !DensitiesSampleCompute )
		return false;

	glm::ivec2 Size = Engine->GetViewport()->GetViewportSize();
	RenderTarget = NewObject<BravoRenderTarget>("RayTracingRenderTarget", Size , GL_RGBA32F, GL_RGBA, GL_FLOAT, false);

	Engine->GetViewport()->OnResizeDelegate.AddSP(Self<FluidPostProcess>(), &FluidPostProcess::OnViewportResize);
	
	return true;
}

void FluidPostProcess::CreateDensitiesMap()
{
	if ( DensitiesMapBuffer != 0 )
		glDeleteTextures(1, &DensitiesMapBuffer);

	glm::vec3 BoundsSize = glm::abs(BoundingBox->GetScale() / DensitiesResolution);
	DensitiesMapSize = glm::ivec3(BoundsSize);

	glGenTextures(1, &DensitiesMapBuffer);
    glBindTexture(GL_TEXTURE_3D, DensitiesMapBuffer);
    glTexImage3D(GL_TEXTURE_3D, 0, GL_R8UI, DensitiesMapSize.x, DensitiesMapSize.y, DensitiesMapSize.z, 0, GL_RED_INTEGER, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_3D, 0);
}

void FluidPostProcess::OnBoundingBoxTransofrmUpdated(const class IBravoTransformable*)
{
	CreateDensitiesMap();
}

void FluidPostProcess::OnViewportResize(const glm::ivec2& NewSize)
{
	RenderTarget->Resize(NewSize);
}

bool FluidPostProcess::EnsureReady()
{
	if ( !Simulation || !BoundingBox )
		return false;

	if ( !Shader->EnsureGPUReady() || !RayMarchingCompute->EnsureGPUReady() || !DensitiesSampleCompute->EnsureGPUReady() )
		return false;

	if ( DensitiesMapBuffer == 0 )
		return false;

	const std::shared_ptr<BravoCamera> camera = Engine->GetCamera();
	if ( !camera )
		return false;

	const glm::mat4 view = camera->GetViewMatrix();
	const glm::mat4 proj = camera->GetProjectionMatrix();

	DensitiesSampleCompute->Use();

		DensitiesSampleCompute->SetFloat1("DensitiesResolution", DensitiesResolution);
		DensitiesSampleCompute->SetFloat3("BoundsSize", BoundingBox->GetScale());
		DensitiesSampleCompute->SetFloat3("BoundsOrigin", BoundingBox->GetLocation());
		DensitiesSampleCompute->SetMatrix4d("BoundingBox", BoundingBox->GetTransform_World().GetTransformMatrix());

		DensitiesSampleCompute->SetInt("ParticleCount", Simulation->ParticleCount);

		DensitiesSampleCompute->SetFloat1("DensityScale", Simulation->DensityScale);
		DensitiesSampleCompute->SetFloat1("ParticleMass", Simulation->ParticleMass);

		DensitiesSampleCompute->SetFloat1("SmoothingRadius", Simulation->SmoothingRadius);
		DensitiesSampleCompute->SetFloat1("SmoothingRadius2", Simulation->SmoothingRadius*Simulation->SmoothingRadius);

		DensitiesSampleCompute->SetFloat1("DensityOffset", DensityOffset);

		
	DensitiesSampleCompute->StopUsage();

	RayMarchingCompute->Use();

		RayMarchingCompute->SetFloat3("BoundsSize", BoundingBox->GetScale());

		RayMarchingCompute->SetMatrix4d("BoundingBox", BoundingBox->GetTransform_World().GetTransformMatrix());
		RayMarchingCompute->SetMatrix4d("inverseBoundingBox", glm::inverse(BoundingBox->GetTransform_World().GetTransformMatrix()));

		RayMarchingCompute->SetFloat3("DensitiesMapSize", glm::vec3(DensitiesMapSize));

		RayMarchingCompute->SetFloat1("DensitiesResolutionInverse", (1.0f / DensitiesResolution));
		RayMarchingCompute->SetFloat1("DensityMultiplier", DensityMultiplier);
		RayMarchingCompute->SetFloat1("DensityOffset", DensityOffset);

		RayMarchingCompute->SetFloat1("MarchingRayStep", MarchingRayStep);
		RayMarchingCompute->SetFloat1("TargetDensity", Simulation->TargetDensity);

		RayMarchingCompute->SetFloat1("LightMarchingRayStep", LightMarchingRayStep);
		RayMarchingCompute->SetFloat3("DirToLight", DirToLight);

		RayMarchingCompute->SetFloat3("ScatteringCoefficients", ScatteringCoefficients);

		RayMarchingCompute->SetFloat1("SurfaceStepsTreashold", SurfaceStepsTreashold);

		RayMarchingCompute->SetFloat3("eyeLocation", camera->GetLocation_World());
		RayMarchingCompute->SetMatrix4d("invViewMat", glm::inverse(view));
		RayMarchingCompute->SetMatrix4d("invProjMat", glm::inverse(proj));
	RayMarchingCompute->StopUsage();

	return true;
}

void FluidPostProcess::Render_Internal()
{
	glm::ivec2 OutputImageSize = RenderTarget->GetSize();

	{
		DensitiesSampleCompute->Use();

		glBindImageTexture(0, DensitiesMapBuffer, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_R8UI);

		GLuint workGroupSizeX = (DensitiesMapSize.x + 7) / 8;  // Round up to the nearest multiple of 16
		GLuint workGroupSizeY = (DensitiesMapSize.y + 7) / 8;
		GLuint workGroupSizeZ = (DensitiesMapSize.z + 7) / 8;
		glDispatchCompute(workGroupSizeX, workGroupSizeY, workGroupSizeZ);


		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	
		DensitiesSampleCompute->StopUsage();
	}
	//return;
	{
		RayMarchingCompute->Use();
		
		glBindImageTexture(0, RenderTarget->GetColorTexture(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
		glBindImageTexture(1, DensitiesMapBuffer, 0, GL_TRUE, 0, GL_READ_ONLY, GL_R8UI);
		

		GLuint workGroupSizeX = (OutputImageSize.x + 15) / 16;  // Round up to the nearest multiple of 16
		GLuint workGroupSizeY = (OutputImageSize.y + 15) / 16;
		glDispatchCompute(workGroupSizeX, workGroupSizeY, 1);

		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		RayMarchingCompute->StopUsage();
	}
	
	int32 OutputTextureUnit = BravoTextureUnitManager::BindTexture();
	glActiveTexture(GL_TEXTURE0 + OutputTextureUnit);
	glBindTexture(GL_TEXTURE_2D, RenderTarget->GetColorTexture());

	Shader->Use();
		Shader->SetInt("screenTexture", OutputTextureUnit);
		Draw();
	Shader->StopUsage();

	BravoTextureUnitManager::UnbindTexture(OutputTextureUnit);
}