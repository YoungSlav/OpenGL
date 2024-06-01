#include "FluidGameInstance.h"
#include "BravoCamera.h"
#include "BravoEngine.h"
#include "BravoViewport.h"
#include "FluidContainer.h"
#include "FluidSimulation.h"

bool FluidGameInstance::Initialize_Internal()
{
	if ( !BravoObject::Initialize_Internal() )
		return false;

	Engine->GetViewport()->OnResizeDelegate.AddSP(Self<FluidGameInstance>(), &FluidGameInstance::OnViewportResize);

	{
		Camera = NewObject<BravoCamera>("FluidCamera", EBravoCameraType::Orthographic);
		Engine->SetCamera(Camera);

		glm::vec3 NewLocation = glm::vec3(0.0f);
		NewLocation.z = 2.0f;
		glm::vec3 NewDirection = glm::vec3(0.0f) - NewLocation;
	
		Camera->SetLocation(NewLocation);
		Camera->SetDirection(NewDirection);
	}
	{
		Container = NewObject<FluidContainer>("FluidContainer", true);
	}
	{
		// spawn fluid
		Simulation = NewObject<FluidSimulation>("FluidSimulation", Container);
	}
	OnViewportResize(Engine->GetViewport()->GetViewportSize());

	Simulation->SpawnParticles(1);

	return true;
}

void FluidGameInstance::OnViewportResize(const glm::ivec2& NewSize)
{
	const float particleScale = 0.02;
	Container->SetSize(glm::vec2(float(NewSize.x-10), float(NewSize.y-10)));
	Simulation->UpdateScale(float(NewSize.y)*particleScale);
}

void FluidGameInstance::Tick(float DeltaTime)
{
	
}