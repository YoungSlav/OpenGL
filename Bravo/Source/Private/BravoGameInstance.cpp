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
#include "BravoDirectionalLightActor.h"

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
	
	
	
	if ( auto dirLightActor = NewObject<BravoDirectionalLightActor>("DirLight") )
	{
		dirLightActor->SetLocation(glm::vec3(-100.0f,  100.0f, 0.0f ));
		dirLightActor->SetDirection(glm::vec3(0.0f, 0.0f, 0.0f) - dirLightActor->GetLocation());
		dirLightActor->SetLightColor(glm::vec3(1.0f));
	}
	
	SpawnCubes();
	return true;
}

void BravoGameInstance::SpawnCubes()
{
	std::shared_ptr<BravoAssetManager> AssetManager = Engine->GetAssetManager();

	if ( auto planeActor = NewObject<BravoActor>("PlaneMeshActor") )
	{
		BravoMeshPtr planeAsset = AssetManager->LoadAsset<BravoMesh>("primitives\\plane.fbx");
		planeActor->SetScale(glm::vec3(10.0f, 10.0f, 1.0f));
		planeActor->SetRotation(glm::vec3(-90.0f, 0.0f, 0.0f));
		planeActor->SetLocation(glm::vec3(0.0f, -1.0f, 0.0f));
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
		for ( int32 i = 0; i < 50; ++i )
		{
			auto cubeMesh = cubeActor->NewObject<BravoStaticMeshComponent>("Cube_MeshComponent_" + std::to_string(i));
			cubeMesh->SetMesh(cubeAsset);
			cubeMesh->SetCastShadows(true);

			BravoMaterialPtr cubeMat = std::shared_ptr<BravoMaterial>(new BravoMaterial());
			cubeMat->Textures[EBravoTextureType::diffuse] = AssetManager->LoadAsset<BravoTexture>("Textures\\grey.png");
			cubeMat->Shininess = 64.0f;
			glm::vec3 newLocation = glm::vec3(0.0f);
			newLocation.x = glm::sin(glm::radians(360.0f / 50 *i)) * 5;
			newLocation.z = glm::cos(glm::radians(360.0f / 50 *i)) * 5;
			cubeMesh->SetLocation(newLocation);
			cubeMesh->SetScale(glm::vec3(0.1f));

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
		planeActor->SetScale(glm::vec3(100.0f, 100.0f, 1.0f));
		planeActor->SetRotation(glm::vec3(-90.0f, 0.0f, 0.0f));
		planeActor->SetLocation(glm::vec3(0.0f, 0.0f, 0.0f));
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
		for ( int32 i = 0; i < 360; ++i )
		{
			cubeMesh->AddInstance(BravoTransform(), false);
		}
		cubeMesh->UpdateInstanceBuffer();
		Cubes.push_back(cubeActor);
	}
}

void BravoGameInstance::Tick(float DeltaTime)
{
	std::shared_ptr<BravoActor> cube = Cubes[0].lock();
	cube->SetRotation(glm::vec3(0.0f, LifeTime*10.0f, 90.0f));
	std::vector<std::shared_ptr<BravoComponent>> components = cube->GetComponents();
	std::shared_ptr<BravoStaticMeshComponent> mesh = std::dynamic_pointer_cast<BravoStaticMeshComponent>(components[0]);
	
	for ( int32 i = 0; i < mesh->InstanceCount(); ++i )
	{
		BravoTransform newTransform;
		newTransform.SetRotation(glm::vec3(0.0f, 0.0f, i*30 + LifeTime*30.0f));
		glm::vec3 newLocation = glm::vec3(0.0f);
		newLocation.x = glm::sin(glm::radians(float(i))) * (glm::sin(LifeTime*0.2f) * 5 + 10);
		newLocation.z = glm::cos(glm::radians(float(i))) * (glm::sin(LifeTime*0.2f) * 5 + 10);
		newTransform.SetLocation(newLocation);
		newTransform.SetScale(glm::vec3(0.1f));
		mesh->UpdateInstance(i, BravoMeshInstance(newTransform), false);
	}
	mesh->UpdateInstanceBuffer();
}
