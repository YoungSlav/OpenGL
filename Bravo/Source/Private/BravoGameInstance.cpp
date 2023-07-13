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
#include "BravoTransform.h"

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
	
	
	
	if ( auto dirLightActor = NewObject<BravoDirLightActor>("DirLight") )
	{
		dirLightActor->SetLocation(glm::vec3(0.0f,  500.0f, 0.0f ));
		dirLightActor->SetRotation(glm::vec3(0.0f, 0.0f, -90.0f));
		dirLightActor->SetLightColor(glm::vec3(1.0f));
	}
	
	//for ( int32 i = 0; i < 1; ++i )
	//{
	//	if ( auto spotLight = NewObject<BravoSpotLightActor>("SpotLight_" + std::to_string(i)) )
	//	{
	//		spotLight->SetLocation(glm::vec3(-5.0f,  10.0f, 0.0f ));
	//		spotLight->SetDirection(glm::normalize(glm::vec3(0.0f) - spotLight->GetLocation() ));
	//		spotLight->SetScale(glm::vec3(-0.5f));
	//		spotLight->SetLightColor(BravoLightColor(glm::vec3(0.2f), glm::vec3(1.0f), glm::vec3(0.1f)));
	//		
	//		BravoMeshPtr coneAsset = AssetManager->LoadAsset<BravoMesh>("primitives\\cone.fbx");
	//		auto coneComp = spotLight->NewObject<BravoStaticMeshComponent>("SpotLight_" + std::to_string(i) + "_mesh");
	//		coneComp->SetMesh(coneAsset);
	//		BravoMaterialPtr coneMat = std::shared_ptr<BravoMaterial>(new BravoMaterial());
	//		coneMat->Textures[EBravoTextureType::diffuse] = AssetManager->LoadAsset<BravoTexture>("Textures\\white.png");
	//		coneMat->Shininess = 100.0f;
	//		coneComp->SetMaterial(coneMat);
	//		
	//		spotLights.push_back(spotLight);
	//	}
	//}
	
	SpawnTestInstances();
	return true;
}

void BravoGameInstance::SpawnCubes()
{
	std::shared_ptr<BravoAssetManager> AssetManager = Engine->GetAssetManager();

	//if ( auto planeActor = NewObject<BravoActor>("PlaneMeshActor") )
	//{
	//	BravoMeshPtr planeAsset = AssetManager->LoadAsset<BravoMesh>("primitives\\plane.fbx");
	//	planeActor->SetScale(glm::vec3(10.0f, 10.0f, 1.0f));
	//	planeActor->SetRotation(glm::vec3(-90.0f, 0.0f, 0.0f));
	//	auto planeMesh = planeActor->NewObject<BravoStaticMeshComponent>("PlaneMeshStaticMesh");
	//	planeMesh->SetMesh(planeAsset);
	//	BravoMaterialPtr planeMat = std::shared_ptr<BravoMaterial>(new BravoMaterial());
	//	planeMat->Textures[EBravoTextureType::diffuse] = AssetManager->LoadAsset<BravoTexture>("Textures\\grey.png");
	//	planeMat->Shininess = 64.0f;
	//	planeMesh->SetMaterial(planeMat);
	//
	//}

	BravoMeshPtr cubeAsset = AssetManager->LoadAsset<BravoMesh>("primitives\\cube.fbx");
	std::vector<glm::vec3> locations {
		glm::vec3(0.0, 5.0, 0.0),
		glm::vec3(10.0, 5.0, 0.0),
		glm::vec3(20.0, 5.0, 0.0),
		glm::vec3(30.0, 5.5, 0.0),
		glm::vec3(40.0, 5.0, 0.0),
	};
	if ( auto cubeActor = NewObject<BravoActor>("Cube") )
	{
		for ( int32 i = 0; i < 10; ++i )
		{
			auto cubeMesh = cubeActor->NewObject<BravoStaticMeshComponent>("Cube_MeshComponent_" + std::to_string(i));
			cubeMesh->SetMesh(cubeAsset);
			cubeMesh->SetCastShadows(true);
			glm::vec3 newLocation = glm::vec3(0.0f);
			newLocation.x = glm::sin(glm::radians(36.0f*i)) * 5;
			newLocation.z = glm::cos(glm::radians(36.0f*i)) * 5;
			cubeMesh->SetLocation(newLocation);
			BravoMaterialPtr cubeMat = std::shared_ptr<BravoMaterial>(new BravoMaterial());
			cubeMat->Textures[EBravoTextureType::diffuse] = AssetManager->LoadAsset<BravoTexture>("Textures\\grey.png");
			cubeMat->Shininess = 64.0f;
			cubeMesh->SetMaterial(cubeMat);
		}
	
		Cubes.push_back(cubeActor);
	}
}

void BravoGameInstance::SpawnTestInstances()
{
	std::shared_ptr<BravoAssetManager> AssetManager = Engine->GetAssetManager();

	if ( auto planeActor = NewObject<BravoActor>("PlaneMeshActor") )
	{
		BravoMeshPtr planeAsset = AssetManager->LoadAsset<BravoMesh>("primitives\\plane.fbx");
		planeActor->SetScale(glm::vec3(10.0f, 10.0f, 1.0f));
		planeActor->SetRotation(glm::vec3(-90.0f, 0.0f, 0.0f));
		auto planeMesh = planeActor->NewObject<BravoStaticMeshComponent>("PlaneMeshStaticMesh");
		planeMesh->SetMesh(planeAsset);
		BravoMaterialPtr planeMat = std::shared_ptr<BravoMaterial>(new BravoMaterial());
		planeMat->Textures[EBravoTextureType::diffuse] = AssetManager->LoadAsset<BravoTexture>("Textures\\grey.png");
		planeMat->Shininess = 64.0f;
		planeMesh->SetMaterial(planeMat);
	
	}

	BravoMeshPtr cubeAsset = AssetManager->LoadAsset<BravoMesh>("primitives\\cube.fbx");
	if ( auto cubeActor = NewObject<BravoActor>("Cube") )
	{
		auto cubeMesh = cubeActor->NewObject<BravoStaticMeshComponent>("Cube_MeshComponent");
		cubeMesh->SetMesh(cubeAsset);
		cubeMesh->SetCastShadows(true);
		BravoMaterialPtr cubeMat = std::shared_ptr<BravoMaterial>(new BravoMaterial());
		cubeMat->Textures[EBravoTextureType::diffuse] = AssetManager->LoadAsset<BravoTexture>("Textures\\grey.png");
		cubeMat->Shininess = 64.0f;
		cubeMesh->SetMaterial(cubeMat);
		cubeMesh->RemoveAllInstances();
		for ( int32 i = 0; i < 10; ++i )
		{
			glm::vec3 newLocation = glm::vec3(0.0f);
			newLocation.x = glm::sin(glm::radians(36.0f*i)) * 5;
			newLocation.z = glm::cos(glm::radians(36.0f*i)) * 5;
			newLocation.y = 5.0f;
			cubeMesh->AddInstance(BravoTransform(newLocation, glm::vec3(0.0f), glm::vec3(1.0f)));
		}
		Cubes.push_back(cubeActor);
	}
}

void BravoGameInstance::Tick(float DeltaTime)
{
	std::shared_ptr<BravoActor> cube = Cubes[0].lock();
	cube->SetRotation(glm::vec3(0.0f, LifeTime*30.0f, 0.0f));
	//std::vector<std::shared_ptr<BravoComponent>> components = cube->GetComponents();
	//for ( int32 i = 0; i < components.size(); ++i )
	//{
	//	components[i]->SetRotation(glm::vec3(0.0f, 0.0f, i*30 + LifeTime*30.0f));
	//	glm::vec3 newLocation = glm::vec3(0.0f);
	//	newLocation.x = glm::sin(glm::radians(36.0f*i)) * (glm::sin(LifeTime) * 5 + 5);
	//	newLocation.z = glm::cos(glm::radians(36.0f*i)) * (glm::sin(LifeTime) * 5 + 5);
	//	components[i]->SetLocation(newLocation);
	//
	//	glm::vec3 newScale = glm::vec3((glm::sin(LifeTime + 0.1f*i) + 1.0f) / 2.0f);
	//	components[i]->SetScale(newScale);
	//
	//}

	//float lightDistance = 5.0f;
	//float h = LifeTime * 0.5;
	//Log::LogMessage("LightHeight: " + std::to_string(h));
	//glm::vec3 newLocation = glm::vec3(0.1f, h, 0.1f);
	//newLocation.x = glm::sin(LifeTime) * lightDistance;
	//newLocation.z = glm::cos(LifeTime) * lightDistance;
	//
	//spotLights[0].lock()->SetLocation(newLocation);
	//spotLights[0].lock()->SetDirection(glm::normalize(glm::vec3(0.0f) - newLocation ));
	//
	//spotLights[0].lock()->CutOff = glm::cos(glm::radians(12.0f));
	//spotLights[0].lock()->OuterCutOff = glm::cos(glm::radians(15.0f));
	//spotLights[0].lock()->Constant = 1.0f;
	//spotLights[0].lock()->Linear = 0.045f;
	//spotLights[0].lock()->Quadratic = 0.0075f;
}
