#include "BravoGameInstance.h"
#include "BravoEngine.h"

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


void BravoGameInstance::Init()
{
	if ( auto Player = GetEngine()->SpawnObject<BravoPlayer>() )
	{
		Player->SetLocation(glm::vec3(-10.0, 10.0f, 0.0));
		Player->SetDirection(glm::normalize(glm::vec3(0.0f) - Player->GetLocation()));
	}

	
	if ( auto skyboxActor = GetEngine()->SpawnObject<BravoSkyboxActor>() )
	{
		std::vector<std::string> skyboxFaces
		{ "right.jpg", "left.jpg", "top.jpg", "bottom.jpg", "front.jpg", "back.jpg", };
		skyboxActor->SetCubemap(BravoAsset::Load<BravoCubemap>("Cubemaps\\skybox\\", skyboxFaces));
	}

	auto groundPlane = GetEngine()->SpawnObject<BravoInfinitePlaneActor>();
	
	
	if ( auto dirLightActor = GetEngine()->SpawnObject<BravoDirLightActor>() )
	{
		dirLightActor->SetLocation(glm::vec3(0.0f,  10.0f, 0.0f ));
		dirLightActor->SetRotation(glm::vec3(0.0f, 0.0f, 90.0f));
		dirLightActor->SetScale(glm::vec3(0.1f));
		dirLightActor->SetLightColor(glm::vec3(1.0f));
	}
	
	//for ( int i = 0; i < 1; ++i )
	//{
	//	if ( auto spotLight = GetEngine()->SpawnObject<BravoSpotLightActor>() )
	//	{
	//		spotLight->SetScale(glm::vec3(-0.5f));
	//		spotLight->SetLightColor(glm::vec3(1.0f));
	//		spotLights.push_back(spotLight);
	//	}
	//}

	BravoMeshPtr cubeAsset = BravoAsset::Load<BravoMesh>("primitives\\cube.fbx");
	BravoMaterialPtr cubeMat = std::shared_ptr<BravoMaterial>(new BravoMaterial());
	cubeMat->Textures[EBravoTextureType::diffuse] = BravoAsset::Load<BravoTexture>("Textures\\grey.png");
	cubeMat->Shininess = 64.0f;
	cubeAsset->SetMaterial(0, cubeMat);
	
	std::vector<glm::vec3> locations {
		glm::vec3(2.0, 5.0, 2.0),
		glm::vec3(3.0, 2.0, 0.0),
		glm::vec3(-2.0, 3.0, -1.5),
		glm::vec3(-2.0, 1.5, 2.0),
		glm::vec3(2.0, 1.0, -3.0),
	};
	float angle = 0.0f;
	for ( unsigned int i = 0; i < locations.size(); ++i )
	{
		if ( auto cubeActor = GetEngine()->SpawnObject<BravoMeshActor>() )
		{
			cubeActor->SetLocation(locations[i]);
			cubeActor->SetRotation(glm::vec3(angle, angle, angle));
			angle += 20.0f;
			cubeActor->SetMesh(cubeAsset);
		}
	}	
}

void BravoGameInstance::Tick(float DeltaTime)
{
	return;
	//LifeTime = 0.0f;
	float lightDistance = 5.0f;
	glm::vec3 newLocation = glm::vec3(0.0f, 10.0f, 0.0f);
	newLocation.x = -glm::sin(LifeTime) * lightDistance;
	newLocation.z = -glm::cos(LifeTime) * lightDistance;
	
	spotLights[0].lock()->SetLocation(newLocation);
	spotLights[0].lock()->SetDirection(glm::normalize(glm::vec3(0.0f) - newLocation ));
	
	spotLights[0].lock()->CutOff = glm::cos(glm::radians(12.0f));
	spotLights[0].lock()->OuterCutOff = glm::cos(glm::radians(15.0f));
	spotLights[0].lock()->Constant = 1.0f;
	spotLights[0].lock()->Linear = 0.09f;
	spotLights[0].lock()->Quadratic = 0.032f;
}
