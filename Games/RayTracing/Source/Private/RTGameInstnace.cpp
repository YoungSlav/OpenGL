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
	Player->SetLocation(glm::vec3(0.0f, 50.0f, 50.0f));
	Player->SetDirection(glm::vec3(0.0f) - Player->GetLocation());
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
		auto SunActor = NewObject<BravoActor>("sunActor");
		SunActor->SetLocation(glm::vec3(0.0f, 150.0f, 0.0f));
		SunActor->SetDirection(glm::vec3(0.0f) - SunActor->GetLocation());
	
		{
			auto SphereComponent = SunActor->NewObject<RTSceneComponent>("sunComponent", ERTComponentType::ERTComponentType_Sphere);
			RayTracingPP->RegisterSceneComponent(SphereComponent);
	
			SphereComponent->SetLocation(glm::vec3(1000.0f));
			SphereComponent->SetScale(glm::vec3(1000.0f));
	
			RTMaterial mat;
			mat.LightColor = glm::vec3(1.0f);
			mat.LightStrength = 10.0f;
	
			SphereComponent->SetMaterial(mat);
		}
	}
	
	{
		auto PlaneActor = NewObject<BravoActor>("planeActor");
		auto PlaneComponent = PlaneActor->NewObject<RTSceneComponent>("planeComponent", ERTComponentType::ERTComponentType_Sphere);
		RayTracingPP->RegisterSceneComponent(PlaneComponent);
		
		PlaneComponent->SetLocation(glm::vec3(0.0f, -100.0f, 0.0f));
		PlaneComponent->SetDirection(glm::vec3(0.0f, 0.0f, 0.0f));
		PlaneComponent->SetScale(glm::vec3(100.0f));

		RTMaterial mat;
		mat.Color = glm::vec3(0.1f, 0.23f, 0.76f);
		
		PlaneComponent->SetMaterial(mat);
	}
	{
		auto SphereActor = NewObject<BravoActor>("sphereActor");
		SphereActor->SetLocation(glm::vec3(0.0f, 0.0f, 0.0f));

		{
			auto SphereComponent = SphereActor->NewObject<RTSceneComponent>("sphereComponent1", ERTComponentType::ERTComponentType_Sphere);
			RayTracingPP->RegisterSceneComponent(SphereComponent);

			SphereComponent->SetLocation(glm::vec3(-5.0f, 3.0f, 0.0f));
			SphereComponent->SetScale(glm::vec3(3.0f));

			RTMaterial mat;
			mat.Color = glm::vec3(0.23f, 0.45f, 0.87f);
			mat.Smoothness = 1.0f;
			mat.Specular = 1.0f;

			SphereComponent->SetMaterial(mat);
		}
		{
			auto SphereComponent = SphereActor->NewObject<RTSceneComponent>("sphereComponent2", ERTComponentType::ERTComponentType_Sphere);
			RayTracingPP->RegisterSceneComponent(SphereComponent);
		
			SphereComponent->SetLocation(glm::vec3(10.0f, 7.0f, 0.0f));
			SphereComponent->SetScale(glm::vec3(7.0f));
		
			RTMaterial mat;
			mat.Color = glm::vec3(0.456f, 0.234f, 0.717f);
			mat.Smoothness = 1.0f;
			mat.Specular = 1.0f;

			SphereComponent->SetMaterial(mat);
		}
	}

	return true;
}

void RTGameInstnace::Tick(float DeltaTime)
{
	
}