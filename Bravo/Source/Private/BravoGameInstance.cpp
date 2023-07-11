#include "BravoGameInstance.h"
#include "BravoEngine.h"
#include "BravoAssetManager.h"

#include "BravoAsset.h"
#include "BravoMesh.h"
#include "BravoShader.h"
#include "BravoTexture.h"
#include "BravoCubemap.h"

#include "BravoPlayer.h"
#include "BravoActor.h"
#include "BravoStaticMeshComponent.h"
#include "BravoLightActor.h"
#include "BravoSkyboxActor.h"
#include "BravoInfinitePlaneActor.h"
#include "BravoMath.h"

#include "BravoScreen_Debug.h"
#include "BravoHUD.h"

bool BravoGameInstance::Initialize_Internal()
{
	if ( !Engine )
		return false;

	if ( Engine->GetHUD() )
	{
		auto screen = NewObject<BravoScreen_Debug>("DebugScreen");
		Engine->GetHUD()->AddScreen(screen);
	}
	
	
	std::shared_ptr<BravoAssetManager> AssetManager = Engine->GetAssetManager();
	
	if ( std::shared_ptr<BravoPlayer> Player = NewObject<BravoPlayer>("Player") )
	{
		Player->SetLocation(glm::vec3(-10.0, 10.0f, 0.0));
		Player->SetDirection(glm::normalize(glm::vec3(0.0f) - Player->GetLocation()));
		
		std::shared_ptr<BravoCamera> Camera = NewObject<BravoCamera>("Camera");
		Engine->SetCamera(Camera);
		Camera->AttachTo(Player);
	}
	
	if ( auto skyboxActor = NewObject<BravoSkyboxActor>("Skybox") )
	{
		skyboxActor->SetCubemap(AssetManager->LoadAsset<BravoCubemap>("Cubemaps\\skybox\\", { "right.jpg", "left.jpg", "top.jpg", "bottom.jpg", "front.jpg", "back.jpg", }));
	}

	auto InfinitePlane = NewObject<BravoInfinitePlaneActor>("InfinitePlane");
	
	BravoMeshPtr planeAsset = AssetManager->LoadAsset<BravoMesh>("primitives\\plane.fbx");
	
	if ( auto planeActor = NewObject<BravoActor>("PlaneMeshActor") )
	{
		planeActor->SetScale(glm::vec3(10.0f, 10.0f, 1.0f));
		planeActor->SetRotation(glm::vec3(-90.0f, 0.0f, 0.0f));
		auto planeMesh = planeActor->NewObject<BravoStaticMeshComponent>("PlaneMeshStaticMesh");
		planeMesh->SetMesh(planeAsset);
		BravoMaterialPtr planeMat = std::shared_ptr<BravoMaterial>(new BravoMaterial());
		planeMat->Textures[EBravoTextureType::diffuse] = AssetManager->LoadAsset<BravoTexture>("Textures\\grey.png");
		planeMat->Shininess = 64.0f;
		planeMesh->SetMaterial(planeMat);
	
	}
	
	
	if ( auto dirLightActor = NewObject<BravoDirLightActor>("DirLight") )
	{
		dirLightActor->SetLocation(glm::vec3(0.0f,  10.0f, 0.0f ));
		dirLightActor->SetRotation(glm::vec3(0.0f, 0.0f, -90.0f));
		dirLightActor->SetScale(glm::vec3(0.1f));
		dirLightActor->SetLightColor(glm::vec3(1.0f));
	}
	
	for ( int32 i = 0; i < 1; ++i )
	{
		if ( auto spotLight = NewObject<BravoSpotLightActor>("SpotLight_" + std::to_string(i)) )
		{
			spotLight->SetLocation(glm::vec3(-5.0f,  10.0f, 0.0f ));
			spotLight->SetDirection(glm::normalize(glm::vec3(0.0f) - spotLight->GetLocation() ));
			spotLight->SetScale(glm::vec3(-0.5f));
			spotLight->SetLightColor(glm::vec3(1.0f));
			spotLights.push_back(spotLight);
		}
	}
	
	SpawnCubes();
	return true;
}

void BravoGameInstance::SpawnCubes()
{
	std::shared_ptr<BravoAssetManager> AssetManager = Engine->GetAssetManager();

	BravoMeshPtr cubeAsset = AssetManager->LoadAsset<BravoMesh>("primitives\\cube.fbx");
	std::vector<glm::vec3> locations {
		glm::vec3(2.0, 5.0, 2.0),
		glm::vec3(3.0, 2.0, 0.0),
		glm::vec3(-2.0, 3.0, -1.5),
		glm::vec3(-2.0, 1.5, 2.0),
		glm::vec3(2.0, 1.0, -3.0),
	};
	float angle = 0.0f;
	for ( uint32 i = 0; i < locations.size(); ++i )
	{
		if ( auto cubeActor = NewObject<BravoActor>("Cube_" + std::to_string(i)) )
		{
			cubeActor->SetLocation(locations[i]);
			cubeActor->SetRotation(glm::vec3(angle, angle, angle));
			angle += 20.0f;
			auto cubeMesh = cubeActor->NewObject<BravoStaticMeshComponent>("Cube_" + std::to_string(i) + "_MeshComponent");
			cubeMesh->SetMesh(cubeAsset);
			cubeMesh->SetCastShadows(true);
	
			BravoMaterialPtr cubeMat = std::shared_ptr<BravoMaterial>(new BravoMaterial());
			cubeMat->Textures[EBravoTextureType::diffuse] = AssetManager->LoadAsset<BravoTexture>("Textures\\grey.png");
			cubeMat->Shininess = 64.0f;
			cubeMesh->SetMaterial(cubeMat);
			Cubes.push_back(cubeActor);
		}
	}
}

void BravoGameInstance::SpawnTest()
{
	std::shared_ptr<BravoAssetManager> AssetManager = Engine->GetAssetManager();
	BravoMeshPtr cubeAsset = AssetManager->LoadAsset<BravoMesh>("test\\test.fbx");
	if ( auto cubeActor = NewObject<BravoActor>("HeavyTest") )
	{
		cubeActor->SetScale(glm::vec3(0.01f));
		auto cubeMesh = cubeActor->NewObject<BravoStaticMeshComponent>("HeavyTest_MeshComponent");
		cubeMesh->SetMesh(cubeAsset);
		cubeMesh->SetCastShadows(true);
		BravoMaterialPtr cubeMat = std::shared_ptr<BravoMaterial>(new BravoMaterial());
		cubeMat->Textures[EBravoTextureType::diffuse] = AssetManager->LoadAsset<BravoTexture>("Textures\\grey.png");
		cubeMat->Shininess = 64.0f;
		cubeMesh->SetMaterial(cubeMat);
		Cubes.push_back(cubeActor);
	}
}

void BravoGameInstance::Tick(float DeltaTime)
{
	float lightDistance = 5.0f;
	glm::vec3 newLocation = glm::vec3(0.0f, 10.0f, 0.0f);
	newLocation.x = glm::sin(LifeTime) * lightDistance;
	newLocation.z = glm::cos(LifeTime) * lightDistance;
	
	spotLights[0].lock()->SetLocation(newLocation);
	spotLights[0].lock()->SetDirection(glm::normalize(glm::vec3(0.0f) - newLocation ));
	
	spotLights[0].lock()->CutOff = glm::cos(glm::radians(12.0f));
	spotLights[0].lock()->OuterCutOff = glm::cos(glm::radians(15.0f));
	spotLights[0].lock()->Constant = 1.0f;
	spotLights[0].lock()->Linear = 0.09f;
	spotLights[0].lock()->Quadratic = 0.032f;
}
