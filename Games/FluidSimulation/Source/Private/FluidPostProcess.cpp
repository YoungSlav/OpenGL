#include "FluidPostProcess.h"
#include "BravoEngine.h"
#include "BravoViewport.h"
#include "BravoRenderTarget.h"
#include "BravoShaderAsset.h"
#include "BravoAssetManager.h"
#include "FluidSimulation.h"
#include "FluidContainer.h"
#include "BravoCamera.h"

bool FluidPostProcess::Initialize_Internal()
{
	if ( !BravoPostProcess::Initialize_Internal() )
		return false;

	Shader = Engine->GetAssetManager()->FindOrLoad<BravoShaderAsset>("FluidPostProcess", BravoShaderLoadingParams("FluidPostProcess"));
	if ( !Shader )
		return false;

	return true;
}

bool FluidPostProcess::EnsureReady()
{
	if ( !BravoPostProcess::EnsureReady() )
		return false;
	
	const std::shared_ptr<BravoCamera> camera = Engine->GetCamera();
	if ( !camera )
		return false;

	if (Simulation == nullptr || Container == nullptr)
		return false;

	return true;
}


void FluidPostProcess::Render_Internal()
{
	assert( Simulation != nullptr && Container != nullptr );
	const std::shared_ptr<BravoCamera> camera = Engine->GetCamera();

	Shader->Use();

		Shader->SetInt("ParticleCount", Simulation->ParticleCount);
		Shader->SetFloat2("WorldSize", Simulation->WorldSize);

		Shader->SetFloat1("ParticleMass", Simulation->ParticleMass);
		Shader->SetFloat1("ParticleRadius", Simulation->ParticleRadius);
		Shader->SetFloat1("SmoothingRadius", Simulation->SmoothingRadius);
		Shader->SetFloat1("TargetDensity", Simulation->TargetDensity);

		const float DensityScale		= 6.0f  / (1.0f * (glm::pi<float>() * glm::pow(Simulation->SmoothingRadius, 4)));
		Shader->SetFloat1("DensityScale", DensityScale);

		glm::mat4 CameraProjection = camera->GetProjectionMatrix();
		glm::mat4 CameraView = camera->GetViewMatrix();

		glm::mat4 mCamera = CameraProjection * CameraView;
		glm::mat4 model  = Container->GetTransform().GetTransformMatrix();

		Shader->SetMatrix4d("model", model);
		Shader->SetMatrix4d("camera", mCamera);

		Draw();

	Shader->StopUsage();
}