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
#include "BravoSpotLightActor.h"
#include "BravoPointLightActor.h"

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
		Player->SetLocation(glm::vec3(-15.0792866, 17.3910675, 0.121996939));
		Player->SetRotation(glm::vec3(0.00000000, 0.800000012, -59.5000076));
		//Player->SetDirection(glm::normalize(glm::vec3(0.0f) - Player->GetLocation()));
		
		std::shared_ptr<BravoCamera> Camera = NewObject<BravoCamera>("Camera");
		Engine->SetCamera(Camera);
		Camera->AttachTo(Player);
	}
	
	if ( auto skyboxActor = NewObject<BravoSkyboxActor>("Skybox") )
	{
		skyboxActor->SetCubemap(AssetManager->LoadAsset<BravoCubemap>("Cubemaps\\skybox\\", { "right.jpg", "left.jpg", "top.jpg", "bottom.jpg", "front.jpg", "back.jpg", }));
	}
	//auto InfinitePlane = NewObject<BravoInfinitePlaneActor>("InfinitePlane");
	
	if ( auto planeActor = NewObject<BravoActor>("PlaneMeshActor") )
	{
		BravoMeshPtr planeAsset = AssetManager->LoadAsset<BravoMesh>("primitives\\cube.fbx");
		planeActor->SetScale(glm::vec3(50.0f, 0.1f, 50.0f));
		//planeActor->SetRotation(glm::vec3(-90.0f, 0.0f, 0.0f));
		planeActor->SetLocation(glm::vec3(0.0f, -1.0f, 0.0f));
		auto planeMesh = planeActor->NewObject<BravoStaticMeshComponent>("PlaneMeshStaticMesh");
		planeMesh->SetMesh(planeAsset);
		planeMesh->SetCastShadows(true);
		BravoMaterialPtr planeMat = std::shared_ptr<BravoMaterial>(new BravoMaterial());
		planeMat->Textures[EBravoTextureType::diffuse] = AssetManager->LoadAsset<BravoTexture>("Textures\\grey.png");
		planeMat->Shininess = 64.0f;
		planeMesh->SetMaterial(planeMat);
	}
	
	
	//SpawnDirLights();
	//SpawnPointLightTest();
	SpawnPointLights();
	//SpawnSpotLights();
	SpawnTestInstances();
	return true;
}

void BravoGameInstance::SpawnPointLightTest()
{
	std::shared_ptr<BravoAssetManager> AssetManager = Engine->GetAssetManager();
	BravoMeshPtr cubeAsset = AssetManager->LoadAsset<BravoMesh>("primitives\\cube.fbx");

	BravoMaterialPtr wallMat = std::shared_ptr<BravoMaterial>(new BravoMaterial());
	wallMat->Textures[EBravoTextureType::diffuse] = AssetManager->LoadAsset<BravoTexture>("Textures\\grey.png");
	wallMat->Shininess = 64.0f;

	std::vector<glm::vec3> scales = {
		glm::vec3(0.1f, 10.0f, 10.0f),
		glm::vec3(0.1f, 10.0f, 10.0f),
		glm::vec3(10.0f, 10.0f, 0.1f),
		glm::vec3(10.0f, 10.0f, 0.1f)
	};
	std::vector<glm::vec3> locations = {
		glm::vec3(10.0f, 10.0f, 0.0f),
		glm::vec3(-10.0f, 10.0f, 0.0f),
		glm::vec3(0.0f, 10.0f, 10.0f),
		glm::vec3(0.0f, 10.0f, -10.0f)
	};
	for ( int32 i = 0; i < 4; ++i )
	{
		if ( auto wallActor = NewObject<BravoActor>("Wall" + std::to_string(i)) )
		{
			wallActor->SetScale(scales[i]);
			wallActor->SetLocation(locations[i]);
			auto wallMesh = wallActor->NewObject<BravoStaticMeshComponent>("WallMeshStaticMesh1");
			wallMesh->SetMesh(cubeAsset);
			wallMesh->SetCastShadows(true);
			wallMesh->SetMaterial(wallMat);
		}
	}
}

void BravoGameInstance::SpawnDirLights()
{
	int32 count = 2;
	for ( int32 i = 0 ; i < count; ++i )
	{
		if ( auto dirLightActor = NewObject<BravoDirectionalLightActor>("DirLight") )
		{
			glm::vec3 newLocation = glm::vec3(0.0f);
			newLocation.x = glm::sin(glm::radians(360.0f / count *i)) * 100;
			newLocation.z = glm::cos(glm::radians(360.0f / count *i)) * 100;
			newLocation.y = 100.0f;
			dirLightActor->SetLocation(newLocation);
			dirLightActor->SetDirection(glm::vec3(0.0f, 0.0f, 0.0f) - dirLightActor->GetLocation());
			dirLightActor->SetLightColor(glm::vec3(1.0f));
		}
	}
}

void BravoGameInstance::SpawnPointLights()
{
	std::shared_ptr<BravoAssetManager> AssetManager = Engine->GetAssetManager();
	BravoMeshPtr sphereAsset = AssetManager->LoadAsset<BravoMesh>("primitives\\sphere.fbx");
	
	std::vector<glm::vec3> locations = {
		glm::vec3(0.0f, 10.0f, 0.0f),
		glm::vec3(0.0f, 20.0f, 0.0f)
	};
	for ( int32 i = 0 ; i < locations.size(); ++i )
	{
		if ( auto pointLightActor = NewObject<BravoPointLightActor>("PointLight") )
		{
			pointLightActor->SetLocation(locations[i]);
			pointLightActor->SetLightColor(glm::vec3(1.0f));
			BravoPointLightSettings PointSettings;
			PointSettings.Intencity = 10;
			pointLightActor->SetSettings(PointSettings);

			auto sphereMesh = pointLightActor->NewObject<BravoStaticMeshComponent>("SphereStaticMesh");
			sphereMesh->SetMesh(sphereAsset);
			sphereMesh->SetScale(glm::vec3(0.25f));
			BravoMaterialPtr sphereMat = std::shared_ptr<BravoMaterial>(new BravoMaterial());
			sphereMat->Textures[EBravoTextureType::diffuse] = AssetManager->LoadAsset<BravoTexture>("Textures\\grey.png");
			sphereMat->Shininess = 64.0f;
			sphereMesh->SetMaterial(sphereMat);

			pointLights.push_back(pointLightActor);
		}
	}
}
void BravoGameInstance::SpawnSpotLights()
{
	std::shared_ptr<BravoAssetManager> AssetManager = Engine->GetAssetManager();
	
	BravoMeshPtr coneAsset = AssetManager->LoadAsset<BravoMesh>("primitives\\cone.fbx");
	int32 count = 2;
	for ( int32 i = 0 ; i < count; ++i )
	{
		if ( auto spotLightActor = NewObject<BravoSpotLightActor>("SpotLight") )
		{
			glm::vec3 newLocation = glm::vec3(0.0f);
			newLocation.x = glm::sin(glm::radians((360.0f / count *i) + 3.14f / 2.0f)) * 20;
			newLocation.z = glm::cos(glm::radians((360.0f / count *i) + 3.14f / 2.0f)) * 20;
			newLocation.y = 10.0f;
			spotLightActor->SetLocation(newLocation);

			spotLightActor->SetDirection(glm::vec3(0.0f, 0.0f, 0.0f) - spotLightActor->GetLocation());
			spotLightActor->SetLightColor(glm::vec3(1.0f));
			BravoSpotLightSettings SpotSettings;
			SpotSettings.CutOff			= 10.0f;
			SpotSettings.OuterCutOff	= 12.0f;
			SpotSettings.Intencity = 10;
			spotLightActor->SetSettings(SpotSettings);

			auto coneMesh = spotLightActor->NewObject<BravoStaticMeshComponent>("SpotLightStaticMesh");
			coneMesh->SetMesh(coneAsset);
			coneMesh->SetRotation(glm::vec3(0.0f, 180.0f, 0.0f));
			coneMesh->SetScale(glm::vec3(0.5f, 0.25f, 0.25f));
			BravoMaterialPtr coneMat = std::shared_ptr<BravoMaterial>(new BravoMaterial());
			coneMat->Textures[EBravoTextureType::diffuse] = AssetManager->LoadAsset<BravoTexture>("Textures\\grey.png");
			coneMat->Shininess = 64.0f;
			coneMesh->SetMaterial(coneMat);

			spotLights.push_back(spotLightActor);
		}
	}
}

void BravoGameInstance::SpawnCubes()
{
	std::shared_ptr<BravoAssetManager> AssetManager = Engine->GetAssetManager();

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


	BravoMeshPtr cubeAsset = AssetManager->LoadAsset<BravoMesh>("primitives\\cube.fbx");
	if ( auto cubeActor = NewObject<BravoActor>("Cube") )
	{
		auto cubeMesh = cubeActor->NewObject<BravoStaticMeshComponent>("Cube_MeshComponent");
		cubeActor->SetLocation(glm::vec3(0.0f, 10.0f, 0.0f));
		cubeMesh->SetMesh(cubeAsset);
		cubeMesh->SetCastShadows(true);
		BravoMaterialPtr cubeMat = std::shared_ptr<BravoMaterial>(new BravoMaterial());
		cubeMat->Textures[EBravoTextureType::diffuse] = AssetManager->LoadAsset<BravoTexture>("Textures\\grey.png");
		cubeMat->Shininess = 64.0f;
		cubeMesh->SetMaterial(cubeMat);
		cubeMesh->RemoveAllInstances();
		for ( int32 i = 0; i < 50; ++i )
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
	cube->SetRotation(glm::vec3(LifeTime*10.0f, 0.0f, 90.0f));
	std::vector<std::shared_ptr<BravoComponent>> components = cube->GetComponents();
	std::shared_ptr<BravoStaticMeshComponent> mesh = std::dynamic_pointer_cast<BravoStaticMeshComponent>(components[0]);
	
	for ( int32 i = 0; i < mesh->InstanceCount(); ++i )
	{
		BravoTransform newTransform;
		glm::vec3 newLocation = glm::vec3(0.0f);
		newLocation.z = glm::sin(glm::radians(float(360.0f / mesh->InstanceCount() * i))) * 2;
		newLocation.y = glm::cos(glm::radians(float(360.0f / mesh->InstanceCount() * i))) * 2;
		newTransform.SetLocation(newLocation);
		newTransform.SetScale(glm::vec3(0.1f));
		mesh->UpdateInstance(i, BravoMeshInstance(newTransform), false);
	}
	mesh->UpdateInstanceBuffer();

	for ( int32 i = 0; i < spotLights.size(); ++i )
	{
		glm::vec3 newLocation = glm::vec3(0.0f);
		newLocation.x = glm::sin(LifeTime + glm::radians(360.0f / spotLights.size() *i)) * 5;
		newLocation.z = glm::cos(LifeTime + glm::radians(360.0f / spotLights.size() *i)) * 5;
	
		newLocation.y = 20.0f;

		spotLights[i]->SetLocation(newLocation);
		spotLights[i]->SetDirection(glm::vec3(0.0f) - newLocation);
	}
}
