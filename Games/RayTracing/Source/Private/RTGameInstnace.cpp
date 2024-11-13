#include "RTGameInstnace.h"
#include "BravoEngine.h"
#include "BravoHUD.h"
#include "BravoScreen_Debug.h"
#include "BravoScreen_ObjectHierarchy.h"
#include "BravoAssetManager.h"
#include "BravoCamera.h"
#include "BravoPlayer.h"
#include "BravoSkyboxActor.h"
#include "BravoCubemapAsset.h"
#include "BravoViewport.h"
#include "BravoDirectionalLightActor.h"
#include "BravoPointLightActor.h"
#include "BravoSpotLightActor.h"
#include "BravoStaticMeshAsset.h"
#include "BravoActor.h"
#include "BravoStaticMeshComponent.h"
#include "BravoMaterialPBR.h"
#include "BravoMaterialUnlit.h"


bool RTGameInstnace::Initialize_Internal()
{
	if ( !Engine )
		return false;

	if ( Engine->GetViewport()->GetHUD() )
	{
		auto debugScreen = NewObject<BravoScreen_Debug>("DebugScreen");
		Engine->GetViewport()->GetHUD()->AddScreen(debugScreen);

		auto hierarchyScreen = NewObject<BravoScreen_ObjectHierarchy>("Hierarchy Screen", Self<RTGameInstnace>());
		Engine->GetViewport()->GetHUD()->AddScreen(hierarchyScreen);
	}
	
	std::shared_ptr<BravoAssetManager> AssetManager = Engine->GetAssetManager();
	
		
	Camera = NewObject<BravoCamera>("Camera", EBravoCameraType::Perspective);
	Engine->SetCamera(Camera);

	Player = NewObject<BravoPlayer>("Player");
	Player->SetLocation(glm::vec3(1000.0f, 0.0f, 1000.0f));
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

	DirLight = NewObject<BravoDirectionalLightActor>("DirLight", BravoDirectionalLightSettings(), glm::vec3(1.0f));
	DirLight->SetAmbientLightColor(glm::vec3(1.0f));
	glm::vec3 newLocation = glm::vec3(0.0f);
	newLocation.x = 100.0f;
	newLocation.z = 100.0f;
	newLocation.y = 100.0f;
	DirLight->SetLocation(newLocation);
	DirLight->SetDirection(glm::vec3(0.0f, 0.0f, 0.0f) - DirLight->GetLocation());
	
	{
		BravoPointLightSettings PointSettings;
		PointSettings.Intencity = 1000.0f;
		PointLightActor = NewObject<BravoPointLightActor>("PointLight", PointSettings, glm::vec3(1.0f));
		PointLightActor->SetLocation(glm::vec3(1000.0f, 0.0f, 1000.0f));
		
		std::shared_ptr<BravoStaticMeshAsset> sphereAsset = AssetManager->FindOrLoad<BravoStaticMeshAsset>("SphereAsset", BravoStaticMeshLoadingParams("primitives\\sphere.fbx"));
		auto sphereMashComponent = PointLightActor->NewObject<BravoStaticMeshComponent>("sphereMashComponent");
		sphereMashComponent->SetCastShadows(false);
		sphereMashComponent->SetMesh(sphereAsset);
		
		BravoUnlitMaterialParams lightMaterial;
		lightMaterial.AlbedoColor = glm::vec3(1.0f);
		std::shared_ptr<BravoMaterialUnlit> sphereMat = sphereMashComponent->NewObject<BravoMaterialUnlit>();
		sphereMat->Load(lightMaterial);
		sphereMashComponent->SetMaterial(sphereMat);
	}
	
	BravoSpotLightSettings SpotSettings;
	SpotSettings.Intencity = 700.0f;
	SpotLightActor = NewObject<BravoSpotLightActor>("Spot", SpotSettings, glm::vec3(1.0f));
	

	

	std::shared_ptr<BravoStaticMeshAsset> planeAsset = AssetManager->FindOrLoad<BravoStaticMeshAsset>("CubeAsset", BravoStaticMeshLoadingParams("primitives\\sphere.fbx"));
	auto planeActor = NewObject<BravoActor>("PlaneMeshActor");
	planeActor->SetScale(glm::vec3(10.0f, 10.0f, 10.0f));
	planeActor->SetLocation(glm::vec3(1000.0f, 0.0f, 1000.0f));
	auto planeMesh = planeActor->NewObject<BravoStaticMeshComponent>("PlaneMeshStaticMesh");
	planeMesh->SetMesh(planeAsset);
	planeMesh->SetCastShadows(true);

	const float dist = 3.0f;
	planeMesh->RemoveAllInstances();
	std::vector<BravoTransform> Instances;
	Instances.resize(6);
	Instances[0].SetLocation(glm::vec3(-dist, 0.0f, 0.0f));
	Instances[1].SetLocation(glm::vec3(dist , 0.0f, 0.0f));
	Instances[2].SetLocation(glm::vec3(0.0f, -dist, 0.0f));
	Instances[3].SetLocation(glm::vec3(0.0f, dist , 0.0f));
	Instances[4].SetLocation(glm::vec3(0.0f, 0.0f, -dist));
	Instances[5].SetLocation(glm::vec3(0.0f, 0.0f, dist ));
	for ( auto& inst :Instances )
		planeMesh->AddInstance(inst);

	BravoPBRMaterialParams planeMaterailLoadingParams;
	//planeMaterailLoadingParams.RoughnessTexture = "Textures\\LightPlankFlooring\\light-plank-flooring_roughness.png";
	//planeMaterailLoadingParams.NormalTexture = "Textures\\LightPlankFlooring\\light-plank-flooring_normal-ogl.png";
	//planeMaterailLoadingParams.MetallicTexture = "Textures\\LightPlankFlooring\\light-plank-flooring_metallic.png";
	//planeMaterailLoadingParams.AoTexture = "Textures\\LightPlankFlooring\\light-plank-flooring_ao.png";
	//planeMaterailLoadingParams.AlbedoTexture = "Textures\\LightPlankFlooring\\light-plank-flooring_albedo.png";
	//planeMaterailLoadingParams.HeightTexture = "Textures\\LightPlankFlooring\\light-plank-flooring_height.png";
	//planeMaterailLoadingParams.HeightScale = 0.1f;

	planeMaterailLoadingParams.RoughnessTexture = "Textures\\rustedIron\\roughness.png";
	planeMaterailLoadingParams.NormalTexture = "Textures\\rustedIron\\normal.png";
	planeMaterailLoadingParams.MetallicTexture = "Textures\\rustedIron\\metallic.png";
	//planeMaterailLoadingParams.MetallicColor = 1.0f;
	//planeMaterailLoadingParams.AoColor = 1.0f;
	planeMaterailLoadingParams.AlbedoTexture = "Textures\\rustedIron\\albedo.png";
	//planeMaterailLoadingParams.AlbedoColor = glm::vec3(1.0f);


	std::shared_ptr<BravoMaterialPBR> planeMat = planeMesh->NewObject<BravoMaterialPBR>();
	planeMat->Load(planeMaterailLoadingParams);

	planeMesh->SetMaterial(planeMat);

	uint32 boxCount = 0;
	glm::vec3 boxSpacing = glm::vec3(60.0f, 0.0f, 0.0f);
	float boxSize = 50.0f;
	const float boxHalfSize = boxSize / 2.0f;
	glm::vec3 spawnCenter = planeActor->GetLocation_World();

	glm::vec3 TotalSpace = boxSpacing * (float(boxCount-1));
	glm::vec3 spawnStartPos = spawnCenter - TotalSpace / 2.0f;

	std::vector<glm::vec3> roomCenters;
	roomCenters.resize(boxCount);

	BravoPBRMaterialParams roomMaterailLoadingParams;
	roomMaterailLoadingParams.AlbedoColor = glm::vec3(1.0f, 1.0f, 1.0f);
	roomMaterailLoadingParams.AoColor = 1.0f;
	std::shared_ptr<BravoMaterialPBR> roomMat = NewObject<BravoMaterialPBR>();
	roomMat->Load(roomMaterailLoadingParams);

	for ( uint32 i = 0; i < boxCount; ++i )
	{
		glm::vec3 boxPosition = spawnStartPos + (boxSpacing*float(i));
		roomCenters[i] = boxPosition;

		if ( auto room = NewObject<BravoActor>("ShowRoom" + std::to_string(i)) )
		{
			room->SetLocation(boxPosition);

			auto leftWall = room->NewObject<BravoStaticMeshComponent>("LeftWall" + std::to_string(i));
			leftWall->SetMesh(planeAsset);
			leftWall->SetCastShadows(true);
			leftWall->SetScale(glm::vec3(0.5f, boxHalfSize, boxHalfSize));
			leftWall->SetLocation(glm::vec3(-boxHalfSize, boxHalfSize, 0.0f));
			leftWall->SetMaterial(planeMat);

			auto rightWall = room->NewObject<BravoStaticMeshComponent>("RightWall" + std::to_string(i));
			rightWall->SetMesh(planeAsset);
			rightWall->SetCastShadows(true);
			rightWall->SetScale(glm::vec3(0.5f, boxHalfSize, boxHalfSize));
			rightWall->SetLocation(glm::vec3(+boxHalfSize, boxHalfSize, 0.0f));
			rightWall->SetMaterial(planeMat);

			auto backWall = room->NewObject<BravoStaticMeshComponent>("BackWall" + std::to_string(i));
			backWall->SetMesh(planeAsset);
			backWall->SetCastShadows(true);
			backWall->SetScale(glm::vec3(boxHalfSize, boxHalfSize, 0.5f));
			backWall->SetLocation(glm::vec3(0.0f, boxHalfSize, -boxHalfSize));
			backWall->SetMaterial(planeMat);

			auto roof = room->NewObject<BravoStaticMeshComponent>("Roof" + std::to_string(i));
			roof->SetMesh(planeAsset);
			roof->SetCastShadows(true);
			roof->SetScale(glm::vec3(boxHalfSize, 0.5f, boxHalfSize));
			roof->SetLocation(glm::vec3(0.0f, boxSize, 0.0f));
			roof->SetMaterial(planeMat);
		}
	}

	

	return true;
}

void RTGameInstnace::Tick(float DeltaTime)
{
	
}