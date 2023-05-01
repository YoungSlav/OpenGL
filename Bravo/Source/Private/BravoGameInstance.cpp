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

	std::shared_ptr<BravoPlayer> Player = GetEngine()->SpawnObject<BravoPlayer>();
	Player->SetLocation(glm::vec3(-10.0, 10.0f, 0.0));
	Player->SetDirection(glm::normalize(glm::vec3(0.0f) - Player->GetLocation()));
	Children.push_back(Player);

	//
	std::shared_ptr<BravoSkyboxActor> skyboxActor = GetEngine()->SpawnObject<BravoSkyboxActor>();
	skyboxActor->SetLocation(glm::vec3(0.0f,  0.0f, 0.0f ));
	std::vector<std::string> skyboxFaces
	{
		"right.jpg",
		"left.jpg",
		"top.jpg",
		"bottom.jpg",
		"front.jpg",
		"back.jpg",
	};
	skyboxActor->SetCubemap(BravoAsset::Load<BravoCubemap>("Cubemaps\\skybox\\", skyboxFaces));
	Children.push_back(skyboxActor);

	std::shared_ptr<BravoInfinitePlaneActor> groundPlane = GetEngine()->SpawnObject<BravoInfinitePlaneActor>();
	groundPlane->SetLocation(glm::vec3(0.0f, 0.0f, 0.0f));
	//groundPlane->SetRotation(glm::vec3(90.0f, 0.0f, 0.0));
	Children.push_back(groundPlane);
	
	
	
	/*
	BravoMeshPtr meshAsset = BravoAsset::Load<BravoMesh>("Nikita\\stylized_containers.obj");
	BravoMaterialPtr mat[6];
	for ( int i = 0; i < 6; ++i )
	{
		mat[i] = std::shared_ptr<BravoMaterial>(new BravoMaterial());
		mat[i]->Shininess = 64.0f;
		meshAsset->SetMaterial(i, mat[i]);
	}
	
	mat[0]->Textures[EBravoTextureType::diffuse] = BravoAsset::Load<BravoTexture>("Nikita\\Textures\\crate\\diffuse.tga");
	mat[1]->Textures[EBravoTextureType::diffuse] = BravoAsset::Load<BravoTexture>("Nikita\\Textures\\barrel\\diffuse.tga");
	mat[2]->Textures[EBravoTextureType::diffuse] = BravoAsset::Load<BravoTexture>("Nikita\\Textures\\chest\\diffuse.tga");
	mat[3]->Textures[EBravoTextureType::diffuse] = BravoAsset::Load<BravoTexture>("Nikita\\Textures\\chest\\top\\diffuse.tga");
	mat[4]->Textures[EBravoTextureType::diffuse] = BravoAsset::Load<BravoTexture>("Nikita\\Textures\\barrel\\diffuse.tga");
	mat[5]->Textures[EBravoTextureType::diffuse] = BravoAsset::Load<BravoTexture>("Nikita\\Textures\\crate\\diffuse.tga");
	
	mat[0]->Textures[EBravoTextureType::normal] = BravoAsset::Load<BravoTexture>("Nikita\\Textures\\crate\\normal.tga");
	mat[1]->Textures[EBravoTextureType::normal] = BravoAsset::Load<BravoTexture>("Nikita\\Textures\\barrel\\normal.tga");
	mat[2]->Textures[EBravoTextureType::normal] = BravoAsset::Load<BravoTexture>("Nikita\\Textures\\chest\\normal.tga");
	mat[3]->Textures[EBravoTextureType::normal] = BravoAsset::Load<BravoTexture>("Nikita\\Textures\\chest\\top\\normal.tga");
	mat[4]->Textures[EBravoTextureType::normal] = BravoAsset::Load<BravoTexture>("Nikita\\Textures\\barrel\\normal.tga");
	mat[5]->Textures[EBravoTextureType::normal] = BravoAsset::Load<BravoTexture>("Nikita\\Textures\\crate\\normal.tga");
	
	BravoMeshActor* meshActor = GetEngine()->SpawnObject<BravoMeshActor>();
	meshActor->SetLocation(glm::vec3(-3.00f,  0.05f, 0.80f ));
	meshActor->SetScale(glm::vec3(0.01f));
	meshActor->SetMesh(meshAsset);
	Children.push_back(meshActor);
	*/
	
	
	
	dirLightActor = GetEngine()->SpawnObject<BravoDirLightActor>();
	dirLightActor.lock()->SetLocation(glm::vec3(0.0f,  10.0f, 0.0f ));
	dirLightActor.lock()->SetScale(glm::vec3(0.1f));
	dirLightActor.lock()->SetLightColor(glm::vec3(1.0f));
	Children.push_back(dirLightActor.lock());

	for ( int i = 0; i < 2; ++i )
	{
		spotLights[i] = GetEngine()->SpawnObject<BravoSpotLightActor>();
		spotLights[i].lock()->SetScale(glm::vec3(-0.5f));
		spotLights[i].lock()->SetLightColor(glm::vec3(1.0f));
		Children.push_back(spotLights[i].lock());
	}



	
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
		std::shared_ptr<BravoMeshActor> cubeActor = GetEngine()->SpawnObject<BravoMeshActor>();
		cubeActor->SetLocation(locations[i]);
		cubeActor->SetRotation(glm::vec3(angle, angle, angle));
		angle += 20.0f;
		cubeActor->SetMesh(cubeAsset);
		Children.push_back(cubeActor);
	}


}

void BravoGameInstance::OnDestroy()
{
	for ( int i = Children.size()-1; i >= 0; --i )
		Children[i].lock()->Destroy();
}

void BravoGameInstance::Tick(float DeltaTime)
{

	//LifeTime = 0.0f;
	float lightDistance = 5.0f;
	glm::vec3 newLocation = glm::vec3(0.0f, 10.0f, 0.0f);
	newLocation.x = -glm::sin(LifeTime) * lightDistance;
	newLocation.z = -glm::cos(LifeTime) * lightDistance;
	
	spotLights[0].lock()->SetLocation(newLocation);
	spotLights[0].lock()->SetDirection(glm::normalize(glm::vec3(0.0f) - newLocation));
	
	spotLights[0].lock()->CutOff = glm::cos(glm::radians(12.0f));
	spotLights[0].lock()->OuterCutOff = glm::cos(glm::radians(15.0f));
	spotLights[0].lock()->Constant = 1.0f;
	spotLights[0].lock()->Linear = 0.09f;
	spotLights[0].lock()->Quadratic = 0.032f;
	
	
	newLocation.x = glm::cos(LifeTime) * lightDistance;
	newLocation.z = glm::sin(LifeTime) * lightDistance;
	spotLights[1].lock()->SetLocation(newLocation);
	spotLights[1].lock()->SetDirection(glm::normalize(glm::vec3(0.0f) - newLocation));
	
	spotLights[1].lock()->CutOff = glm::cos(glm::radians(20.5f));
	spotLights[1].lock()->OuterCutOff = glm::cos(glm::radians(30.0f));
	spotLights[1].lock()->Constant = 1.0f;
	spotLights[1].lock()->Linear = 0.09f;
	spotLights[1].lock()->Quadratic = 0.032f;
}
