#include "FluidPostProcess_Density.h"
#include "FluidSimulation.h"
#include "FluidContainer.h"
#include "BravoEngine.h"
#include "BravoAssetManager.h"
#include "BravoShaderAsset.h"

bool FluidPostProcess_Density::Initialize_Internal()
{
	if ( !BravoPostProcess::Initialize_Internal() )
		return false;

	Shader = Engine->GetAssetManager()->FindOrLoad<BravoShaderAsset>("DensityPP", BravoShaderLoadingParams("Density"));
	if ( !Shader )
		return false;
	
	return true;
}

void FluidPostProcess_Density::Render_Internal()
{
	assert( Simulation != nullptr && Container != nullptr );

	Shader->Use();

		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, Simulation->GetParticlesSSBO());

		Shader->SetInt("particleCount", Simulation->ParticlesCount);
		Shader->SetFloat1("targetDensity", Simulation->TargetDensity);
		Shader->SetFloat1("particleMass", Simulation->ParticleMass);
		Shader->SetFloat1("smoothingRadius", Simulation->SmoothingRadius);
		Shader->SetFloat2("worldBounds", Container->GetSize());

		Draw();

	Shader->StopUsage();
}