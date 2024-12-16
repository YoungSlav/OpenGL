#include "FluidGameInstance.h"
#include "BravoCamera.h"
#include "BravoEngine.h"
#include "BravoViewport.h"
#include "FluidContainer.h"
#include "FluidSimulation.h"
#include "BravoHUD.h"
#include "BravoScreen_Debug.h"
#include "FluidScreen_Simulation.h"
#include "FluidPostProcess.h"

bool FluidGameInstance::Initialize_Internal()
{
	if ( !BravoObject::Initialize_Internal() )
		return false;

	//Engine->LimitFPS(120.0f);

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
		Container = NewObject<FluidContainer>("FluidContainer");
	}
	{
		// spawn fluid
		Simulation = NewObject<FluidSimulation>("FluidSimulation", Container);

		Container->SetSimulation(Simulation);
	}

	//auto PP = NewObject<FluidPostProcess>("SimulationPP", Simulation, Container);
	//Engine->GetViewport()->AddPostProcess(PP);

	if ( Engine->GetViewport()->GetHUD() )
	{
		auto debugScreen = NewObject<BravoScreen_Debug>("DebugScreen");
		Engine->GetViewport()->GetHUD()->AddScreen(debugScreen);

		auto simulationSettings = NewObject<FluidScreen_Simulation>("SimulationSettings", Simulation);
		Engine->GetViewport()->GetHUD()->AddScreen(simulationSettings);
	}

	OnViewportResize(Engine->GetViewport()->GetViewportSize());
	


	return true;
}

void FluidGameInstance::OnViewportResize(const glm::ivec2& NewSize)
{
	float vWidth = float(NewSize.x);
	float vHeight = float(NewSize.y);

	float WorldWidth = WorldHeight * vWidth / vHeight;
	glm::vec2 WorldSize = glm::vec2(WorldWidth, WorldHeight);

	Container->SetSize(WorldSize);
	Simulation->SpawnParticles();

	Camera->SetWorld2DSize(WorldSize);
}