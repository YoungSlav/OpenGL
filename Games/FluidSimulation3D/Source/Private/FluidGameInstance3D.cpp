#include "FluidGameInstance3D.h"
#include "BravoCamera.h"
#include "BravoEngine.h"
#include "BravoViewport.h"
#include "FluidSimulation3D.h"
#include "BravoHUD.h"
#include "BravoPlayer.h"
#include "BravoScreen_Debug.h"

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
	
	Player->SetLocation(glm::vec3(0.0f, 30.0f, 0.0) - BravoMath::forwardV*30.0f);
	Player->SetDirection(glm::vec3(0.0f, 0.0f, 0.0f) - Player->GetLocation_World());
	Camera->AttachTo(Player);
	Camera->SetTransform(BravoTransform());

	Simulation = NewObject<FluidSimulation3D>("FluidSimulation3D");
	
	return true;
}