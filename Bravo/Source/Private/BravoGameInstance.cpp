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
	Engine = BravoEngine::GetInstance();

	BravoPlayer* Player = Engine->SpawnObject<BravoPlayer>();
	Player->SetLocation(glm::vec3(-10.0, 10.0f, 0.0));
	Player->SetDirection(glm::normalize(glm::vec3(0.0f) - Player->GetLocation()));
	Children.push_back(Player);

	//
	BravoSkyboxActor* skyboxActor = Engine->SpawnObject<BravoSkyboxActor>();
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

	BravoInfinitePlaneActor* groundPlane = Engine->SpawnObject<BravoInfinitePlaneActor>();
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
	
	BravoMeshActor* meshActor = Engine->SpawnObject<BravoMeshActor>();
	meshActor->SetLocation(glm::vec3(-3.00f,  0.05f, 0.80f ));
	meshActor->SetScale(glm::vec3(0.01f));
	meshActor->SetMesh(meshAsset);
	Children.push_back(meshActor);
	*/
	
	
	
	dirLightActor = Engine->SpawnObject<BravoDirLightActor>();
	dirLightActor->SetLocation(glm::vec3(0.0f,  10.0f, 0.0f ));
	dirLightActor->SetScale(glm::vec3(0.1f));
	dirLightActor->SetLightColor(glm::vec3(1.0f));
	Children.push_back(dirLightActor);

	for ( int i = 0; i < 2; ++i )
	{
		spotLights[i] = Engine->SpawnObject<BravoSpotLightActor>();
		spotLights[i]->SetScale(glm::vec3(-0.5f));
		spotLights[i]->SetLightColor(glm::vec3(1.0f));
		Children.push_back(spotLights[i]);
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
		BravoMeshActor* cubeActor = Engine->SpawnObject<BravoMeshActor>();
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
		Children[i]->Destroy();
}

void BravoGameInstance::Tick(float DeltaTime)
{

	//LifeTime = 0.0f;
	float lightDistance = 5.0f;
	glm::vec3 newLocation = glm::vec3(0.0f, 10.0f, 0.0f);
	newLocation.x = -glm::sin(LifeTime) * lightDistance;
	newLocation.z = -glm::cos(LifeTime) * lightDistance;
	
	spotLights[0]->SetLocation(newLocation);
	spotLights[0]->SetDirection(glm::normalize(glm::vec3(0.0f) - newLocation));
	
	spotLights[0]->CutOff = glm::cos(glm::radians(12.0f));
	spotLights[0]->OuterCutOff = glm::cos(glm::radians(15.0f));
	spotLights[0]->Constant = 1.0f;
	spotLights[0]->Linear = 0.09f;
	spotLights[0]->Quadratic = 0.032f;
	
	
	newLocation.x = glm::cos(LifeTime) * lightDistance;
	newLocation.z = glm::sin(LifeTime) * lightDistance;
	spotLights[1]->SetLocation(newLocation);
	spotLights[1]->SetDirection(glm::normalize(glm::vec3(0.0f) - newLocation));
	
	spotLights[1]->CutOff = glm::cos(glm::radians(20.5f));
	spotLights[1]->OuterCutOff = glm::cos(glm::radians(30.0f));
	spotLights[1]->Constant = 1.0f;
	spotLights[1]->Linear = 0.09f;
	spotLights[1]->Quadratic = 0.032f;
}
