#include "RTGameInstnace.h"
#include "BravoEngine.h"
#include "BravoHUD.h"
#include "BravoScreen_Debug.h"
#include "BravoScreen_ObjectHierarchy.h"
#include "BravoAssetManager.h"
#include "BravoCamera.h"
#include "BravoPlayer.h"
#include "BravoSkyboxActor.h"
#include "BravoCubemapAsset.h"
#include "BravoViewport.h"
#include "BravoDirectionalLightActor.h"
#include "BravoPointLightActor.h"
#include "BravoSpotLightActor.h"
#include "BravoStaticMeshAsset.h"
#include "BravoActor.h"
#include "BravoStaticMeshComponent.h"
#include "BravoMaterialPBR.h"
#include "BravoMaterialUnlit.h"
#include "RTPostProcess.h"
#include "RTSceneComponent.h"


bool RTGameInstnace::Initialize_Internal()
{
	if ( !Engine )
		return false;

	if ( Engine->GetViewport()->GetHUD() )
	{
		auto debugScreen = NewObject<BravoScreen_Debug>("DebugScreen");
		Engine->GetViewport()->GetHUD()->AddScreen(debugScreen);

		auto hierarchyScreen = NewObject<BravoScreen_ObjectHierarchy>("Hierarchy Screen", Self<RTGameInstnace>());
		Engine->GetViewport()->GetHUD()->AddScreen(hierarchyScreen);
	}
	
	std::shared_ptr<BravoAssetManager> AssetManager = Engine->GetAssetManager();
	
		
	Camera = NewObject<BravoCamera>("Camera", EBravoCameraType::Perspective);
	Engine->SetCamera(Camera);

	Player = NewObject<BravoPlayer>("Player");
	Player->SetLocation(glm::vec3(5.0f, 0.0f, 0.0f));
	Player->SetDirection(glm::vec3(0.0f) - glm::vec3(5.0f, 0.0f, 0.0f));
	Camera->AttachTo(Player);
	Camera->SetTransform(BravoTransform());

	auto Skybox = AssetManager->FindOrLoad<BravoCubemapAsset>("SkyboxAsset", BravoCubemapLoadingParams({
			"Cubemaps\\skybox\\right.jpg",
			"Cubemaps\\skybox\\left.jpg",
			"Cubemaps\\skybox\\top.jpg",
			"Cubemaps\\skybox\\bottom.jpg",
			"Cubemaps\\skybox\\front.jpg",
			"Cubemaps\\skybox\\back.jpg", }));

	if ( auto skyboxActor = NewObject<BravoSkyboxActor>("Skybox") )
	{
		skyboxActor->SetCubemap(Skybox);
	}

	RayTracingPP = NewObject<RTPostProcess>("RayTracingPostProcess");
	Engine->GetViewport()->AddPostProcess(RayTracingPP);

	RayTracingPP->RegisterSkybox(Skybox);

	
	{
		auto PlaneActor = NewObject<BravoActor>("planeActor");
		auto PlaneComponent = PlaneActor->NewObject<RTSceneComponent>("planeComponent", ERTComponentType::ERTComponentType_Plane);
		RayTracingPP->RegisterSceneComponent(PlaneComponent);
		
		PlaneComponent->SetLocation(glm::vec3(0.0f));
		PlaneComponent->SetDirection(glm::vec3(0.0f, 1.0f, 0.0f));
		PlaneComponent->SetScale(glm::vec3(50.0f));
		
		PlaneComponent->SetMaterial({glm::vec3(1.0f, 0.0f, 0.0f), 0.0f, 1.0f, 0.0f});
	}
	{
		auto SphereActor = NewObject<BravoActor>("sphereActor");
		SphereActor->SetLocation(glm::vec3(0.0f, 0.0f, 0.0f));

		{
			auto SphereComponent = SphereActor->NewObject<RTSceneComponent>("sphereComponent1", ERTComponentType::ERTComponentType_Sphere);
			RayTracingPP->RegisterSceneComponent(SphereComponent);

			SphereComponent->SetLocation(glm::vec3(-5.0f, 3.0f, 0.0f));
			SphereComponent->SetScale(glm::vec3(3.0f));

			SphereComponent->SetMaterial({glm::vec3(0.0f, 1.0f, 0.0f), 0.0f, 1.0f, 0.0f});
		}
		{
			auto SphereComponent = SphereActor->NewObject<RTSceneComponent>("sphereComponent2", ERTComponentType::ERTComponentType_Sphere);
			RayTracingPP->RegisterSceneComponent(SphereComponent);
		
			SphereComponent->SetLocation(glm::vec3(5.0f, 3.0f, 0.0f));
			SphereComponent->SetScale(glm::vec3(3.0f));
		
			SphereComponent->SetMaterial({glm::vec3(0.0f, 0.0f, 1.0f), 0.0f, 1.0f, 0.0f});
		}
	}


	

	return true;
}

void RTGameInstnace::Tick(float DeltaTime)
{
	
}