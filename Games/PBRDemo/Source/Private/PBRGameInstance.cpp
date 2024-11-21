#include "PBRGameInstance.h"
#include "BravoEngine.h"
#include "BravoViewport.h"
#include "BravoHUD.h"
#include "BravoScreen_Debug.h"
#include "BravoAssetManager.h"
#include "BravoCamera.h"
#include "BravoPlayer.h"
#include "BravoCubemapAsset.h"
#include "BravoSkyboxActor.h"
#include "BravoDirectionalLightActor.h"

#include "BravoStaticMeshAsset.h"
#include "BravoStaticMeshComponent.h"
#include "BravoMaterialPBR.h"
#include "BravoSpotLightActor.h"
#include "BravoMaterialUnlit.h"

#include "BravoPointLightActor.h"

bool PBRGameInstance::Initialize_Internal()
{
	if ( !BravoObject::Initialize_Internal() )
		return false;

	if ( !Engine )
		return false;

	if ( Engine->GetViewport()->GetHUD() )
	{
		auto debugScreen = NewObject<BravoScreen_Debug>("DebugScreen");
		Engine->GetViewport()->GetHUD()->AddScreen(debugScreen);
	}
	
	std::shared_ptr<BravoAssetManager> AssetManager = Engine->GetAssetManager();
		
	Camera = NewObject<BravoCamera>("Camera", EBravoCameraType::Perspective);
	Engine->SetCamera(Camera);

	Player = NewObject<BravoPlayer>("Player");
	Player->SetLocation(glm::vec3(0.0f, 50.0f, 0.0f));
	Camera->AttachTo(Player);
	Camera->SetTransform(BravoTransform());


	if ( auto skyboxActor = NewObject<BravoSkyboxActor>("Skybox") )
	{
		auto SkyboxAsset = AssetManager->FindOrLoad<BravoCubemapAsset>("SkyboxAsset", BravoCubemapLoadingParams({
				"Cubemaps\\skybox\\right.jpg",
				"Cubemaps\\skybox\\left.jpg",
				"Cubemaps\\skybox\\top.jpg",
				"Cubemaps\\skybox\\bottom.jpg",
				"Cubemaps\\skybox\\front.jpg",
				"Cubemaps\\skybox\\back.jpg", }));
		skyboxActor->SetCubemap(SkyboxAsset);
	}

	//if ( auto dirLightActor = NewObject<BravoDirectionalLightActor>("DirLight", BravoDirectionalLightSettings(), glm::vec3(1.0f)) )
	//{
	//	dirLightActor->SetAmbientLightColor(glm::vec3(0.5f));
	//
	//	glm::vec3 newLocation = glm::vec3(0.0f);
	//	dirLightActor->SetLocation(newLocation);
	//	dirLightActor->SetDirection(glm::vec3(0.0f, 0.0f, 0.0f) - dirLightActor->GetLocation());
	//}

	if ( auto backPanelActor = NewObject<BravoActor>("BackPanelActor") )
	{
		backPanelActor->SetLocation(glm::vec3(40.0f, 50.0f, 0.0f));
		Player->SetDirection(backPanelActor->GetLocation_World() - Player->GetLocation_World());
		backPanelActor->SetDirection(backPanelActor->GetLocation_World() - Player->GetLocation_World());

		if ( auto backPanelMesh = backPanelActor->NewObject<BravoStaticMeshComponent>("BackPanelMesh") )
		{
			auto planeAsset = AssetManager->FindOrLoad<BravoStaticMeshAsset>("plane", BravoStaticMeshLoadingParams("primitives\\plane.fbx"));
			backPanelMesh->SetScale(glm::vec3(20.0f));
			backPanelMesh->SetMesh(planeAsset);
			if ( auto planeMaterial = backPanelMesh->NewObject<BravoMaterialPBR>("BackPlaneMaterial") )
			{
				BravoPBRMaterialParams material;
				material.AlbedoColor = glm::vec3(1.0f);
				planeMaterial->Load(material);

				backPanelMesh->SetMaterial(planeMaterial);
			}
		}
	}

	const int32 squreSize = 5;
	const float totalSpace = 36.0f;
	const float offset = totalSpace / squreSize;
	const glm::vec3 startPos = glm::vec3(0.0, -totalSpace / 2.0f, -totalSpace / 2.0f) + glm::vec3(0.0f, offset/2.0f, offset/2.0f);
	glm::vec3 spawnPos = startPos;

	auto sphereAsset = AssetManager->FindOrLoad<BravoStaticMeshAsset>("sphere", BravoStaticMeshLoadingParams("primitives\\sphere.fbx"));

	BravoPBRMaterialParams sphereMaterial;
	sphereMaterial.AlbedoColor = glm::vec3(0.6f, 0.0f, 0.0f);

	auto PBRDemoActor = NewObject<BravoActor>("PBRDemoActor");
	PBRDemoActor->SetLocation(glm::vec3(35.0f, 50.0f, 0.0f));
	for ( int32 x = 0; x < squreSize; ++x )
	{
		for ( int32 y = 0; y < squreSize; ++y )
		{
			auto sphereMesh = PBRDemoActor->NewObject<BravoStaticMeshComponent>("SphereMesh" + std::to_string(x) + "_" + std::to_string(y));
			sphereMesh->SetLocation(spawnPos);
			sphereMesh->SetScale(glm::vec3(2.0f));
			sphereMesh->SetMesh(sphereAsset);
			//sphereMesh->SetCastShadows(true);
			auto sphereMat = sphereMesh->NewObject<BravoMaterialPBR>("SphereMaterial" + std::to_string(x) + "_" + std::to_string(y));
			sphereMaterial.MetallicColor = float(x) / squreSize;
			sphereMaterial.RoughnessColor = float(y) / squreSize;
			sphereMat->Load(sphereMaterial);
			sphereMesh->SetMaterial(sphereMat);
				
			spawnPos.y += offset;
		}
		spawnPos.y = startPos.y;
		spawnPos.z += offset;
	}
	
	{
		BravoPointLightSettings PointSettings;
		PointSettings.Intencity = 1500.0f;

		PointLight = NewObject<BravoPointLightActor>("PointLight", PointSettings, glm::vec3(1.0f));
		auto sphereMesh = PointLight->NewObject<BravoStaticMeshComponent>("PointLightStaticMesh");
		sphereMesh->SetMesh(sphereAsset);
		sphereMesh->SetScale(glm::vec3(0.25f));
		BravoUnlitMaterialParams materailLoadingParams;
		materailLoadingParams.AlbedoColor = PointLight->GetLightColor();
		std::shared_ptr<BravoMaterialUnlit> material = PointLight->NewObject<BravoMaterialUnlit>();
		material->Load(materailLoadingParams);
		sphereMesh->SetMaterial(material);

	}
	//{
	//	BravoSpotLightSettings SpotSettings;
	//	SpotSettings.CutOff			= 10.0f;
	//	SpotSettings.OuterCutOff	= 25.0f;
	//	SpotSettings.Intencity = 1500;
	//
	//	SpotLight = NewObject<BravoSpotLightActor>("SpotLight", SpotSettings, glm::vec3(1.0f));
	//	SpotLight->SetLocation(Player->GetLocation_World());
	//	SpotLight->SetLocation(glm::vec3(-50.0f, 50.0f, 0.0f));
	//	SpotLight->SetDirection(PBRDemoActor->GetLocation_World() - SpotLight->GetLocation());
	//
	//	auto coneAsset = AssetManager->FindOrLoad<BravoStaticMeshAsset>("cone", BravoStaticMeshLoadingParams("primitives\\cone.fbx"));
	//	auto coneMesh = SpotLight->NewObject<BravoStaticMeshComponent>("SpotLightStaticMesh");
	//	coneMesh->SetMesh(coneAsset);
	//	coneMesh->SetRotation(glm::vec3(0.0f, -90.0f, 0.0f));
	//	coneMesh->SetScale(glm::vec3(0.5f, 0.25f, 0.25f));
	//	BravoUnlitMaterialParams materailLoadingParams;
	//	materailLoadingParams.AlbedoColor = SpotLight->GetLightColor();
	//	std::shared_ptr<BravoMaterialUnlit> material = SpotLight->NewObject<BravoMaterialUnlit>();
	//	material->Load(materailLoadingParams);
	//	coneMesh->SetMaterial(material);
	//}
	

	return true;
}

void PBRGameInstance::Tick(float DeltaTime)
{
	const glm::vec3 baseLocation = glm::vec3(20.0f, 50.0f, 0.0f);
	glm::vec3 offset = glm::vec3(0.0f);
	offset.y = glm::sin(LifeTime) * 10.0f;
	offset.z = glm::cos(LifeTime) * 10.0f;
	PointLight->SetLocation(baseLocation + offset);
}