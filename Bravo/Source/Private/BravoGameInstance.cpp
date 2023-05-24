#include "BravoGameInstance.h"
#include "BravoEngine.h"
#include "BravoAssetManager.h"

#include "BravoAsset.h"
#include "BravoMesh.h"
#include "BravoShader.h"
#include "BravoTexture.h"
#include "BravoCubemap.h"

#include "BravoPlayer.h"
#include "BravoMeshActor.h"
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
		Engine->GetHUD()->AddScreen(Engine->NewObject<BravoScreen_Debug>());
	}
	
	
	std::shared_ptr<BravoAssetManager> AssetManager = Engine->GetAssetManager();
	
	if ( std::shared_ptr<BravoPlayer> Player = Engine->NewObject<BravoPlayer>() )
	{
		Player->SetLocation(glm::vec3(-10.0, 10.0f, 0.0));
		Player->SetDirection(glm::normalize(glm::vec3(0.0f) - Player->GetLocation()));
	}
	
	if ( auto skyboxActor = Engine->NewObject<BravoSkyboxActor>() )
	{
		skyboxActor->SetCubemap(AssetManager->LoadAsset<BravoCubemap>("Cubemaps\\skybox\\", { "right.jpg", "left.jpg", "top.jpg", "bottom.jpg", "front.jpg", "back.jpg", }));
	}

	auto groundPlane = Engine->NewObject<BravoInfinitePlaneActor>();

	BravoMeshPtr planeAsset = AssetManager->LoadAsset<BravoMesh>("primitives\\plane.fbx");
	BravoMaterialPtr planeMat = std::shared_ptr<BravoMaterial>(new BravoMaterial());
	planeMat->Textures[EBravoTextureType::diffuse] = AssetManager->LoadAsset<BravoTexture>("Textures\\grey.png");
	planeMat->Shininess = 64.0f;
	planeAsset->SetMaterial(planeMat);
	if ( auto planeActor = Engine->NewObject<BravoMeshActor>() )
	{
		planeActor->SetScale(glm::vec3(10.0f, 10.0f, 1.0f));
		planeActor->SetRotation(glm::vec3(-90.0f, 0.0f, 0.0f));
		planeActor->SetMesh(planeAsset);
	}
	
	
	if ( auto dirLightActor = Engine->NewObject<BravoDirLightActor>() )
	{
		dirLightActor->SetLocation(glm::vec3(10.0f,  10.0f, 0.0f ));
		dirLightActor->SetRotation(glm::vec3(0.0f, 0.0f, -90.0f));
		dirLightActor->SetScale(glm::vec3(0.1f));
		dirLightActor->SetLightColor(glm::vec3(1.0f));
	}
	
	//for ( int32 i = 0; i < 1; ++i )
	//{
	//	if ( auto spotLight = Engine->NewObject<BravoSpotLightActor>() )
	//	{
	//		spotLight->SetScale(glm::vec3(-0.5f));
	//		spotLight->SetLightColor(glm::vec3(1.0f));
	//		spotLights.push_back(spotLight);
	//	}
	//}

	BravoMeshPtr cubeAsset = AssetManager->LoadAsset<BravoMesh>("primitives\\cube.fbx");
	BravoMaterialPtr cubeMat = std::shared_ptr<BravoMaterial>(new BravoMaterial());
	cubeMat->Textures[EBravoTextureType::diffuse] = AssetManager->LoadAsset<BravoTexture>("Textures\\grey.png");
	cubeMat->Shininess = 64.0f;
	cubeAsset->SetMaterial(cubeMat);
	
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
		if ( auto cubeActor = Engine->NewObject<BravoMeshActor>() )
		{
			cubeActor->SetLocation(locations[i]);
			cubeActor->SetRotation(glm::vec3(angle, angle, angle));
			angle += 20.0f;
			cubeActor->SetMesh(cubeAsset);
		}
	}
	return true;
}

void BravoGameInstance::Tick(float DeltaTime)
{
	//float lightDistance = 5.0f;
	//glm::vec3 newLocation = glm::vec3(0.0f, 10.0f, 0.0f);
	//newLocation.x = -glm::sin(LifeTime) * lightDistance;
	//newLocation.z = -glm::cos(LifeTime) * lightDistance;
	//
	//spotLights[0].lock()->SetLocation(newLocation);
	//spotLights[0].lock()->SetDirection(glm::normalize(glm::vec3(0.0f) - newLocation ));
	//
	//spotLights[0].lock()->CutOff = glm::cos(glm::radians(12.0f));
	//spotLights[0].lock()->OuterCutOff = glm::cos(glm::radians(15.0f));
	//spotLights[0].lock()->Constant = 1.0f;
	//spotLights[0].lock()->Linear = 0.09f;
	//spotLights[0].lock()->Quadratic = 0.032f;
}
