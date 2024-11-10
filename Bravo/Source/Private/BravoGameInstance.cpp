#include "BravoGameInstance.h"
#include "BravoEngine.h"
#include "BravoViewport.h"
#include "BravoAssetManager.h"

#include "BravoAsset.h"
#include "BravoStaticMeshAsset.h"
#include "BravoShaderAsset.h"
#include "BravoTextureAsset.h"
#include "BravoCubemapAsset.h"
#include "BravoMaterialPBR.h"
#include "BravoMaterialUnlit.h"

#include "BravoPlayer.h"
#include "BravoActor.h"
#include "BravoStaticMeshComponent.h"
#include "BravoLightActor.h"
#include "BravoDirectionalLightActor.h"
#include "BravoSpotLightActor.h"
#include "BravoPointLightActor.h"

#include "BravoSkyboxActor.h"
#include "BravoInfinitePlaneActor.h"
#include "BravoMath.h"

#include "BravoHUD.h"
#include "BravoTransform.h"

#include "BravoGizmo.h"

#include "BravoScreen_Debug.h"
#include "BravoScreen_ObjectHierarchy.h"
#include "BravoTerrainActor.h"
#include "BravoBoundingBox.h"


bool BravoGameInstance::Initialize_Internal()
{
	if ( !Engine )
		return false;

	if ( Engine->GetViewport()->GetHUD() )
	{
		auto debugScreen = NewObject<BravoScreen_Debug>("DebugScreen");
		Engine->GetViewport()->GetHUD()->AddScreen(debugScreen);

		auto hierarchyScreen = NewObject<BravoScreen_ObjectHierarchy>("Hierarchy Screen", Self<BravoGameInstance>());
		Engine->GetViewport()->GetHUD()->AddScreen(hierarchyScreen);
	}
	
	std::shared_ptr<BravoAssetManager> AssetManager = Engine->GetAssetManager();
	
		
	Camera = NewObject<BravoCamera>("Camera", EBravoCameraType::Perspective);
	Engine->SetCamera(Camera);

	Player = NewObject<BravoPlayer>("Player");
	
	Player->SetLocation(glm::vec3(0.0f, 30.0f, 0.0) - BravoMath::forwardV*30.0f);
	Player->SetDirection(glm::vec3(0.0f, 0.0f, 0.0f) - Player->GetLocation_World());
	Camera->AttachTo(Player);
	Camera->SetTransform(BravoTransform());
	
	if ( auto skyboxActor = NewObject<BravoSkyboxActor>("Skybox") )
	{
		skyboxActor->SetCubemap(AssetManager->FindOrLoad<BravoCubemapAsset>("SkyboxAsset", BravoCubemapLoadingParams({
			"Cubemaps\\skybox\\right.jpg",
			"Cubemaps\\skybox\\left.jpg",
			"Cubemaps\\skybox\\top.jpg",
			"Cubemaps\\skybox\\bottom.jpg",
			"Cubemaps\\skybox\\front.jpg",
			"Cubemaps\\skybox\\back.jpg", })));
	}
	auto InfinitePlane = NewObject<BravoInfinitePlaneActor>("InfinitePlane");

	//if ( auto planeActor = NewObject<BravoActor>("BoundingBoxActor") )
	//{
	//	auto planeMesh = planeActor->NewObject<BravoBoundingBox>("BoundingBox");
	//}

	if ( auto planeActor = NewObject<BravoActor>("PlaneMeshActor") )
	{
		std::shared_ptr<BravoStaticMeshAsset> planeAsset = AssetManager->FindOrLoad<BravoStaticMeshAsset>("CubeAsset", BravoStaticMeshLoadingParams("primitives\\cube.fbx"));
		planeActor->SetScale(glm::vec3(50.0f, 1.0f, 50.0f));
		//planeActor->SetRotation(glm::vec3(-90.0f, 0.0f, 0.0f));
		//planeActor->SetLocation(glm::vec3(0.0f, -1.0f, 0.0f));
		auto planeMesh = planeActor->NewObject<BravoStaticMeshComponent>("PlaneMeshStaticMesh");
		planeMesh->SetMesh(planeAsset);
		planeMesh->SetCastShadows(true);
		//planeMesh->SetScale(glm::vec3(50.0f, 0.1f, 50.0f));
		
		BravoPBRMaterialParams materailLoadingParams;
		materailLoadingParams.AlbedoColor = glm::vec3(1.0f, 1.0f, 1.0f);
		materailLoadingParams.AoColor = 1.0f;
		std::shared_ptr<BravoMaterialPBR> planeMat = planeMesh->NewObject<BravoMaterialPBR>();
		planeMat->Load(materailLoadingParams);
		planeMesh->SetMaterial(planeMat);
	}

	{
		auto TestActor = NewObject<BravoActor>("TestActor");
		TestActor->SetLocation(glm::vec3(0.0f, 5.0f, 0.0f));
		auto cubeAsset = AssetManager->FindOrLoad<BravoStaticMeshAsset>("CubeAsset", BravoStaticMeshLoadingParams("primitives\\cube.fbx"));
		auto TestComponent = TestActor->NewObject<BravoStaticMeshComponent>("TestComponent");
		TestComponent->SetMesh(cubeAsset);
		TestComponent->SetCastShadows(true);

		BravoUnlitMaterialParams materailLoadingParams;
		materailLoadingParams.AlbedoColor = glm::vec3(1.0f, 1.0f, 1.0f);

	//	BravoPBRMaterialParams materailLoadingParams;
	//	materailLoadingParams.AlbedoColor = glm::vec3(1.0f, 1.0f, 1.0f);
	//	materailLoadingParams.AoColor = 1.0f;
		auto material = TestComponent->NewObject<BravoMaterialUnlit>();
		material->Load(materailLoadingParams);
		TestComponent->SetMaterial(material);
	}

	SpawnDirLights();

	//if ( auto pointLightActor = NewObject<BravoSpotLightActor>("PointLight", BravoSpotLightSettings(), glm::vec3(1.0f)) )
	//{
	//	glm::vec3 newLocation = glm::vec3(0.0f);
	//	//newLocation.x = glm::sin(glm::radians(360.0f / count *i)) * 100;
	//	//newLocation.z = glm::cos(glm::radians(360.0f / count *i)) * 100;
	//	newLocation.y = 20.0f;
	//	pointLightActor->SetLocation(newLocation);
	//	pointLightActor->SetDirection(glm::vec3(0.0f, 0.0f, 0.0f) - pointLightActor->GetLocation());
	//}

	return true;
}


void BravoGameInstance::SpawnDirLights()
{
	int32 count = 1;
	for ( int32 i = 0 ; i < count; ++i )
	{
		if ( auto dirLightActor = NewObject<BravoDirectionalLightActor>("DirLight", BravoDirectionalLightSettings(), glm::vec3(1.0f)) )
		{
			glm::vec3 newLocation = glm::vec3(0.0f);
			//newLocation.x = glm::sin(glm::radians(360.0f / count *i)) * 100;
			//newLocation.z = glm::cos(glm::radians(360.0f / count *i)) * 100;
			newLocation.y = 100.0f;
			dirLightActor->SetLocation(newLocation);
			dirLightActor->SetDirection(glm::vec3(0.0f, 0.0f, 0.0f) - dirLightActor->GetLocation());
		}
	}
}


void BravoGameInstance::Tick(float DeltaTime)
{	
	
}

