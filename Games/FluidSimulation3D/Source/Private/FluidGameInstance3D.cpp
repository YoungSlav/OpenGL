#include "FluidGameInstance3D.h"
#include "BravoCamera.h"
#include "BravoEngine.h"
#include "BravoViewport.h"
#include "FluidSimulation3D.h"
#include "BravoHUD.h"
#include "BravoPlayer.h"
#include "BravoScreen_Debug.h"
#include "FluidPostProcess.h"
#include "FluidScreen_SimulationSettings.h"
#include "FluidScreen_PostProcessSettings.h"

bool FluidGameInstance3D::Initialize_Internal()
{
	if ( !BravoObject::Initialize_Internal() )
		return false;
		
	if ( Engine->GetViewport()->GetHUD() )
	{
		auto debugScreen = NewObject<BravoScreen_Debug>("DebugScreen");
		Engine->GetViewport()->GetHUD()->AddScreen(debugScreen);
	}

	Camera = NewObject<BravoCamera>("FluidCamera", EBravoCameraType::Perspective);
	Engine->SetCamera(Camera);
	Player = NewObject<BravoPlayer>("Player");
	Player->SetMoveSpeed(5.0f);
	
	Player->SetLocation(glm::vec3(0.0f, 30.0f, 0.0) - BravoMath::forwardV*30.0f);
	Player->SetDirection(glm::vec3(0.0f, 0.0f, 0.0f) - Player->GetLocation_World());
	Camera->AttachTo(Player);
	Camera->SetTransform(BravoTransform());

	Simulation = NewObject<FluidSimulation3D>("FluidSimulation3D");

	RaymarchingPP = NewObject<FluidPostProcess>("FluidPostProcess", Simulation, Simulation->GetBoundingBox());
	Engine->GetViewport()->AddPostProcess(RaymarchingPP);

	{
		auto fluidSettingsScreen = NewObject<FluidScreen_SimulationSettings>("Fluid Settings", Simulation);
		Engine->GetViewport()->GetHUD()->AddScreen(fluidSettingsScreen);

		auto postProcessSettingsScreen = NewObject<FluidScreen_PostProcessSettings>("Post process settings", RaymarchingPP);
		Engine->GetViewport()->GetHUD()->AddScreen(postProcessSettingsScreen);
	}

	return true;
}