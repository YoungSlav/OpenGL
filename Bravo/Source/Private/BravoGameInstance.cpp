#include "BravoGameInstance.h"
#include "BravoEngine.h"
#include "BravoViewport.h"
#include "BravoAssetManager.h"

#include "BravoAsset.h"
#include "BravoStaticMeshAsset.h"
#include "BravoShaderAsset.h"
#include "BravoTextureAsset.h"
#include "BravoCubemapAsset.h"
#include "BravoMaterialPBR.h"
#include "BravoMaterialUnlit.h"

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

#include "BravoGizmo.h"

bool BravoGameInstance::Initialize_Internal()
{

	if ( !Engine )
		return false;

	if ( Engine->GetViewport()->GetHUD() )
	{
		auto screen = NewObject<BravoScreen_Debug>("DebugScreen");
		Engine->GetViewport()->GetHUD()->AddScreen(screen);
	}
	
	std::shared_ptr<BravoAssetManager> AssetManager = Engine->GetAssetManager();
	
		
	Camera = NewObject<BravoCamera>("Camera");
	Engine->SetCamera(Camera);
	//Camera->SetLocation(glm::vec3(0.0f, 30.0f, -30.0));
	//Camera->SetDirection(glm::vec3(0.0f) - Camera->GetLocation_World());

	Player = NewObject<BravoPlayer>("Player");
	
	Player->SetLocation(glm::vec3(0.0f, 30.0f, 0.0) - BravoMath::forwardV*30.0f);
	Player->SetDirection(glm::vec3(0.0f, 0.0f, 0.0f) - Player->GetLocation_World());
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
	auto InfinitePlane = NewObject<BravoInfinitePlaneActor>("InfinitePlane");

	
	
	//if ( auto planeActor = NewObject<BravoActor>("PlaneMeshActor") )
	//{
	//	std::shared_ptr<BravoStaticMeshAsset> planeAsset = AssetManager->FindOrLoad<BravoStaticMeshAsset>("CubeAsset", BravoStaticMeshLoadingParams("primitives\\cube.fbx"));
	//	planeActor->SetScale(glm::vec3(50.0f, 0.1f, 50.0f));
	//	//planeActor->SetRotation(glm::vec3(-90.0f, 0.0f, 0.0f));
	//	//planeActor->SetLocation(glm::vec3(0.0f, -1.0f, 0.0f));
	//	auto planeMesh = planeActor->NewObject<BravoStaticMeshComponent>("PlaneMeshStaticMesh");
	//	planeMesh->SetMesh(planeAsset);
	//	planeMesh->SetCastShadows(true);
	//	//planeMesh->SetScale(glm::vec3(50.0f, 0.1f, 50.0f));
	//	
	//	BravoPBRMaterialParams materailLoadingParams;
	//	materailLoadingParams.AlbedoColor = glm::vec3(1.0f, 1.0f, 1.0f);
	//	materailLoadingParams.AoColor = 1.0f;
	//	std::shared_ptr<BravoMaterialPBR> planeMat = planeMesh->NewObject<BravoMaterialPBR>();
	//	planeMat->Load(materailLoadingParams);
	//	planeMesh->SetMaterial(planeMat);
	//}

	{
	TestActor = NewObject<BravoActor>("TestActor");
	auto cubeAsset = AssetManager->FindOrLoad<BravoStaticMeshAsset>("CubeAsset", BravoStaticMeshLoadingParams("primitives\\cube.fbx"));
	TestComponent = TestActor->NewObject<BravoStaticMeshComponent>("TestComponent");
	TestComponent->SetMesh(cubeAsset);

	BravoPBRMaterialParams materailLoadingParams;
	materailLoadingParams.AlbedoColor = glm::vec3(1.0f, 1.0f, 1.0f);
	materailLoadingParams.AoColor = 1.0f;
	auto material = TestComponent->NewObject<BravoMaterialPBR>();
	material->Load(materailLoadingParams);
	TestComponent->SetMaterial(material);
	}

	
	SpawnDirLights();

	
	//SpawnPointLights();
	//SpawnSpotLights();

	SpawnSpheres();
	SpawnTestInstances();

	return true;
}


void BravoGameInstance::TestTransforms()
{
	glm::vec3 location = BravoMath::RandVector(1000000.0);
	glm::vec3 direction = glm::abs(BravoMath::RandVector(1.0f));
	glm::quat rotation = BravoMath::DirectionToQuaternion(direction);
	//glm::vec3 direction = BravoMath::RotationToDirection(rotation);
	//rotation = BravoMath::DirectionToRotation(direction);
	glm::vec3 scale = glm::abs(BravoMath::RandVector(10.0));

	BravoTransform parent(location, rotation, scale);
	location = parent.GetLocation();
	rotation = parent.GetRotation();
	direction = parent.GetForwardVector();
	scale = parent.GetScale();
	
	BravoTransform child;

	BravoTransform childWorld(parent.GetTransformMatrix() * child.GetTransformMatrix());
	
	glm::vec3 worldCLoc = childWorld.GetLocation();
	glm::quat worldCRot = childWorld.GetRotation();
	glm::vec3 worldCDir = childWorld.GetForwardVector();
	glm::vec3 worldCSc = childWorld.GetScale();

	bool mat = BravoMath::IsNearlyEqual(childWorld.GetTransformMatrix(), parent.GetTransformMatrix());
	bool loc = BravoMath::IsNearlyEqual(location, worldCLoc);
	bool rot = BravoMath::IsNearlyEqual(rotation, worldCRot);
	bool dir = BravoMath::IsNearlyEqual(direction, worldCDir);
	bool sc = BravoMath::IsNearlyEqual(scale, worldCSc);

	if ( !mat )
	{
		Log::LogMessage(ELog::Error, "Mat failed {} --> {}", parent.GetTransformMatrix(), child.GetTransformMatrix());
	}
	if ( !loc )
	{
		Log::LogMessage(ELog::Error, "Loc failed {} --> {}", location, worldCLoc);
	}
	if ( !rot )
	{
		Log::LogMessage(ELog::Error, "Rot failed {} --> {}", rotation, worldCRot);
	}
	if ( !dir )
	{
		Log::LogMessage(ELog::Error, "Dir failed {} --> {}", direction, worldCDir);
	}
	if ( !sc )
	{
		Log::LogMessage(ELog::Error, "Sc failed {} --> {}", scale, worldCSc);
	}
}


void BravoGameInstance::SpawnDirLights()
{
	int32 count = 1;
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
	std::shared_ptr<BravoStaticMeshAsset> sphereAsset = AssetManager->FindOrLoad<BravoStaticMeshAsset>("SphereMeshAsset", BravoStaticMeshLoadingParams("primitives\\sphere.fbx"));
	
	std::vector<glm::vec3> locations = {
		glm::vec3(0.0f, 20.0f, 0.0f),
	};
	for ( int32 i = 0 ; i < locations.size(); ++i )
	{
		if ( auto pointLightActor = NewObject<BravoPointLightActor>("PointLight") )
		{
		//	pointLightActor->SetLocation(locations[i]);
			pointLightActor->SetLightColor(glm::vec3(1.0f));
			BravoPointLightSettings PointSettings;
			PointSettings.Intencity = 700.0f;
			pointLightActor->SetSettings(PointSettings);

			auto sphereMesh = pointLightActor->NewObject<BravoStaticMeshComponent>("SphereStaticMesh");
			sphereMesh->SetMesh(sphereAsset);
			sphereMesh->SetScale(glm::vec3(0.25f));
			BravoUnlitMaterialParams materailLoadingParams;
			materailLoadingParams.AlbedoColor = pointLightActor->GetLightColor();
			std::shared_ptr<BravoMaterialUnlit> material = pointLightActor->NewObject<BravoMaterialUnlit>();
			material->Load(materailLoadingParams);
			sphereMesh->SetMaterial(material);

			pointLights.push_back(pointLightActor);
		}
	}
}
void BravoGameInstance::SpawnSpotLights()
{
	std::shared_ptr<BravoAssetManager> AssetManager = Engine->GetAssetManager();
	
	std::shared_ptr<BravoStaticMeshAsset> coneAsset = AssetManager->FindOrLoad<BravoStaticMeshAsset>("ConeMeshAsset", BravoStaticMeshLoadingParams("primitives\\cone.fbx"));
	int32 count = 2;
	for ( int32 i = 0 ; i < count; ++i )
	{
		if ( auto spotLightActor = NewObject<BravoSpotLightActor>("SpotLight") )
		{
			spotLightActor->SetDirection(glm::vec3(0.0f, 0.0f, 0.0f) - spotLightActor->GetLocation());
			spotLightActor->SetLightColor(glm::vec3(1.0f));
			BravoSpotLightSettings SpotSettings;
			SpotSettings.CutOff			= 5.0f;
			SpotSettings.OuterCutOff	= 15.0f;
			SpotSettings.Intencity = 700;
			spotLightActor->SetSettings(SpotSettings);

			auto coneMesh = spotLightActor->NewObject<BravoStaticMeshComponent>("SpotLightStaticMesh");
			coneMesh->SetMesh(coneAsset);
			coneMesh->SetRotation(glm::vec3(0.0f, -90.0f, 0.0f));
			coneMesh->SetScale(glm::vec3(0.5f, 0.25f, 0.25f));
			BravoUnlitMaterialParams materailLoadingParams;
			materailLoadingParams.AlbedoColor = spotLightActor->GetLightColor();
			std::shared_ptr<BravoMaterialUnlit> material = spotLightActor->NewObject<BravoMaterialUnlit>();
			material->Load(materailLoadingParams);
			coneMesh->SetMaterial(material);

			spotLights.push_back(spotLightActor);
		}
	}
}

void BravoGameInstance::SpawnSpheres()
{
	std::shared_ptr<BravoAssetManager> AssetManager = Engine->GetAssetManager();

	std::shared_ptr<BravoStaticMeshAsset> cubeAsset = AssetManager->FindOrLoad<BravoStaticMeshAsset>("SphereAsset", BravoStaticMeshLoadingParams("primitives\\sphere.fbx"));
	BravoPBRMaterialParams materailLoadingParams;
	materailLoadingParams.AlbedoColor = glm::vec3(0.5f, 0.0f, 0.0f);
	materailLoadingParams.AlbedoTexture = "Textures\\rustedIron\\albedo.png";
	materailLoadingParams.MetallicTexture = "Textures\\rustedIron\\metallic.png";
	materailLoadingParams.RoughnessTexture = "Textures\\rustedIron\\roughness.png";
	materailLoadingParams.AoColor = 1.0f;
	


	if ( auto cubeActor = NewObject<BravoActor>("Cube") )
	{
		int32 count = 30;
		for ( int32 i = 0; i < count; ++i )
		{
			auto cubeMesh = cubeActor->NewObject<BravoStaticMeshComponent>("Cube_MeshComponent_" + std::to_string(i));
			cubeMesh->SetMesh(cubeAsset);
			cubeMesh->SetCastShadows(true);

			std::shared_ptr<BravoMaterialPBR> material = cubeMesh->NewObject<BravoMaterialPBR>();
			material->Load(materailLoadingParams);
			glm::vec3 newLocation = glm::vec3(0.0f);
			newLocation.x = glm::sin(glm::radians(360.0f / count *i)) * 3;
			newLocation.z = glm::cos(glm::radians(360.0f / count *i)) * 3;
			newLocation.y = 7.0f;

			
			cubeMesh->SetLocation(newLocation);
			cubeMesh->SetScale(glm::vec3(0.3f));

			cubeMesh->SetMaterial(material);
		}
	
		Cubes.push_back(cubeActor);
	}
}

void BravoGameInstance::SpawnTestInstances()
{
	std::shared_ptr<BravoAssetManager> AssetManager = Engine->GetAssetManager();

	std::shared_ptr<BravoStaticMeshAsset> sphereAsset = AssetManager->FindOrLoad<BravoStaticMeshAsset>("asdasd", BravoStaticMeshLoadingParams("primitives\\sphere.fbx"));

	BravoPBRMaterialParams materailLoadingParams;
	materailLoadingParams.AlbedoColor = glm::vec3(0.5f, 0.0f, 0.0f);
	materailLoadingParams.AlbedoTexture = "Textures\\rustedIron\\albedo.png";
	materailLoadingParams.MetallicTexture = "Textures\\rustedIron\\metallic.png";
	materailLoadingParams.RoughnessTexture = "Textures\\rustedIron\\roughness.png";
	materailLoadingParams.AoColor = 1.0f;

	if ( auto sphereActor = NewObject<BravoActor>("Cube") )
	{
		auto sphereMesh = sphereActor->NewObject<BravoStaticMeshComponent>("Cube_MeshComponent");
		std::shared_ptr<BravoMaterialPBR> material = sphereMesh->NewObject<BravoMaterialPBR>();
		material->Load(materailLoadingParams);
		sphereActor->SetLocation(glm::vec3(0.0f, 10.0f, 0.0f));
		sphereMesh->SetMesh(sphereAsset);
		sphereMesh->SetCastShadows(true);
		sphereMesh->SetMaterial(material);
		sphereMesh->RemoveAllInstances();
		int32 count = 50;
		for ( int32 i = 0; i < count; ++i )
		{
			glm::vec3 newLocation = glm::vec3(0.0f);
			newLocation.x = glm::sin(glm::radians(360.0f / count *i)) * 20;
			newLocation.z = glm::cos(glm::radians(360.0f / count *i)) * 20;
			sphereMesh->AddInstance(BravoInstanceData(BravoTransform(newLocation, glm::quat(), glm::vec3(0.3f))));
		}
		Cubes.push_back(sphereActor);
	}
}

void BravoGameInstance::Tick(float DeltaTime)
{	
	for ( int32 i = 0; i < pointLights.size(); ++i )
	{
		glm::vec3 newLocation = glm::vec3(0.0f);
		newLocation.x = glm::sin(LifeTime) * 20.0f;
		newLocation.z = glm::cos(LifeTime) * 20.0f;
		newLocation.y = 20.0f;
		pointLights[i]->SetLocation(newLocation);

		pointLights[i]->SetDirection(glm::vec3(0.0f) - newLocation);

		
	}

	for ( int32 i = 0; i < spotLights.size(); ++i )
	{
		glm::vec3 newLocation = glm::vec3(0.0f);
		newLocation.x = glm::sin(LifeTime + glm::radians(360.0f / spotLights.size() *i)) * 10.0f;
		newLocation.z = glm::cos(LifeTime + glm::radians(360.0f / spotLights.size() *i)) * 10.0f;
	
		newLocation.y = 20.0f;

		spotLights[i]->SetLocation(newLocation);
		spotLights[i]->SetDirection(glm::vec3(0.0f) - newLocation);
	}
	{
		glm::vec3 newLocation2 = glm::vec3(0.0f);
		newLocation2.x = glm::sin(LifeTime) * 50;
		newLocation2.z = glm::cos(LifeTime) * 50;
		newLocation2.y = 50.0f;
		//Player->SetLocation(newLocation2);
		//Player->SetDirection(glm::vec3(0.0f) - newLocation2);
	}

	TestCompTransform(DeltaTime);
}


void BravoGameInstance::TestCompTransform(float DeltaTime)
{
	//TestActor->SetScale(glm::vec3(0.5f));
	//TestComponent->SetScale(glm::vec3(0.5f, 0.75f, 0.9f));

	glm::quat cRotation = TestComponent->GetRotation_World();

	glm::quat aRotation = glm::quat(glm::radians(20.0f * DeltaTime), BravoMath::upV);

	//TestComponent->SetRotation_World(aRotation * cRotation);
}