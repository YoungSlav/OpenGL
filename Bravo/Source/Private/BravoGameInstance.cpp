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
	
	if ( std::shared_ptr<BravoPlayer> Player = NewObject<BravoPlayer>("Player") )
	{
		Player->SetLocation(glm::vec3(-15.0792866, 17.3910675, 0.121996939));
		Player->SetRotation(glm::vec3(0.00000000, 0.0, -30.0));
		
		std::shared_ptr<BravoCamera> Camera = NewObject<BravoCamera>("Camera");
		Engine->SetCamera(Camera);
		Camera->AttachTo(Player);
		Camera->SetTransform(BravoTransform());
	}

	
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
	//	planeActor->SetLocation(glm::vec3(0.0f, -1.0f, 0.0f));
	//	auto planeMesh = planeActor->NewObject<BravoStaticMeshComponent>("PlaneMeshStaticMesh");
	//	planeMesh->SetMesh(planeAsset);
	//	planeMesh->SetCastShadows(true);
	//	
	//	BravoPBRMaterialParams materailLoadingParams;
	//	materailLoadingParams.AlbedoColor = glm::vec3(1.0f, 1.0f, 1.0f);
	//	materailLoadingParams.AoColor = 1.0f;
	//	std::shared_ptr<BravoMaterialPBR> planeMat = planeMesh->NewObject<BravoMaterialPBR>();
	//	planeMat->Load(materailLoadingParams);
	//	planeMesh->SetMaterial(planeMat);
	//}

	//if ( auto gizmo = NewObject<BravoGizmo>("BravoGizmo") )
	//{
	//	gizmo->SetLocation(glm::vec3(2.0f, 2.0f, 2.0f));
	//}
	
	SpawnDirLights();

	
	SpawnPointLights();
	SpawnSpotLights();

	SpawnSpheres();
	//
	//SpawnTestInstances();
	return true;
}


void BravoGameInstance::Test()
{
	glm::vec3 location = BravoMath::RandVector(1000000.0);
	glm::vec3 rotation = BravoMath::RotationToDirection(BravoMath::RandVector(1.0));
	BravoMath::DirectionToQuaternion(BravoMath::RandVector(1.0));
	glm::vec3 scale = glm::abs(BravoMath::RandVector(10.0));

	BravoTransform parent(location, rotation, scale);
	location = parent.GetLocation();
	rotation = parent.GetRotation();
	scale = parent.GetScale();
	
	BravoTransform child;

	BravoTransform childWorld(parent.GetTransformMatrix() * child.GetTransformMatrix());
	
	glm::vec3 worldCLoc = childWorld.GetLocation();
	glm::vec3 worldCRot = childWorld.GetRotation();
	glm::vec3 worldCSc = childWorld.GetScale();

	bool mat = childWorld.IsNearlyEqual(parent);
	bool loc = !glm::all(glm::epsilonNotEqual(location, worldCLoc, FLT_EPS));
	bool rot = !glm::all(glm::epsilonNotEqual(rotation, worldCRot, FLT_EPS));
	bool sc = !glm::all(glm::epsilonNotEqual(scale, worldCSc, FLT_EPS));


	if ( !mat )
	{
		Log::LogMessage(std::format("Mat failed {} --> {}", Log::to_string(parent.GetTransformMatrix()), Log::to_string(childWorld.GetTransformMatrix())));
	}
	if ( !loc )
	{
		Log::LogMessage(std::format("Loc failed {} --> {}", Log::to_string(parent.GetLocation()), Log::to_string(childWorld.GetLocation())));
	}
	if ( !rot )
	{
		Log::LogMessage(std::format("Rot failed {} --> {}", Log::to_string(parent.GetRotation()), Log::to_string(childWorld.GetRotation())));
	}
	if ( !sc )
	{
		Log::LogMessage(std::format("Sc failed {} --> {}", Log::to_string(parent.GetScale()), Log::to_string(childWorld.GetScale())));
	}
}

void BravoGameInstance::SpawnTriangles()
{
	std::shared_ptr<BravoAssetManager> AssetManager = Engine->GetAssetManager();

	std::shared_ptr<BravoStaticMeshAsset> triangleAsset =
		AssetManager->FindOrLoad<BravoStaticMeshAsset>("triangleAsset", BravoStaticMeshLoadingParams("primitives\\triangle.fbx"));
	BravoUnlitMaterialParams materailLoadingParams;
	materailLoadingParams.bUseVertexColor = true;
	int32 count = 100;

	//std::shared_ptr<BravoStaticMeshAsset> triangleAsset =
	//	AssetManager->FindOrLoad<BravoStaticMeshAsset>("triangleAsset", BravoStaticMeshLoadingParams("primitives\\cube.fbx"));
	//BravoUnlitMaterialParams materailLoadingParams;
	//materailLoadingParams.bUseVertexColor = false;
	//int32 count = 1000;

	for ( int32 i = 0; i < count; ++i )
	{
		if ( auto triangleActor = NewObject<BravoActor>("Triangle") )
		{
			auto triangleMesh = triangleActor->NewObject<BravoStaticMeshComponent>("TriangleMeshComponent");
			triangleMesh->SetMesh(triangleAsset);
			triangleMesh->SetCastShadows(false);

			std::shared_ptr<BravoMaterialUnlit> material = triangleMesh->NewObject<BravoMaterialUnlit>();
			material->Load(materailLoadingParams);
			
			triangleActor->SetLocation(BravoMath::RandVector(10.0));
			triangleActor->SetRotation(BravoMath::RandVector(360.0));
			triangleMesh->SetMaterial(material);
		}
	}
}

void BravoGameInstance::SpawnTriangles_Inst()
{
	std::shared_ptr<BravoAssetManager> AssetManager = Engine->GetAssetManager();

	std::shared_ptr<BravoStaticMeshAsset> triangleAsset =
		AssetManager->FindOrLoad<BravoStaticMeshAsset>("triangleAsset", BravoStaticMeshLoadingParams("primitives\\triangle.fbx"));
	BravoUnlitMaterialParams materailLoadingParams;
	materailLoadingParams.bUseVertexColor = true;
	int32 count = 100;

	//std::shared_ptr<BravoStaticMeshAsset> triangleAsset =
	//	AssetManager->FindOrLoad<BravoStaticMeshAsset>("triangleAsset", BravoStaticMeshLoadingParams("primitives\\cube.fbx"));
	//BravoUnlitMaterialParams materailLoadingParams;
	//materailLoadingParams.bUseVertexColor = false;
	//int32 count = 1000;

	
	if ( auto triangleActor = NewObject<BravoActor>("Triangle") )
	{
		auto triangleMesh = triangleActor->NewObject<BravoStaticMeshComponent>("TriangleMeshComponent");
		triangleMesh->SetMesh(triangleAsset);
		triangleMesh->SetCastShadows(false);

		std::shared_ptr<BravoMaterialUnlit> material = triangleMesh->NewObject<BravoMaterialUnlit>();
		material->Load(materailLoadingParams);
		triangleMesh->SetMaterial(material);
		for ( int32 i = 1; i < count; ++i )
		{
			triangleMesh->AddInstance(BravoTransform(BravoMath::RandVector(10.0),
				glm::vec3(BravoMath::RandVector(360.0f)),
				glm::vec3(1.0f)),
				false);
		}
		triangleMesh->UpdateInstanceBuffer();
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
			pointLightActor->SetLocation(locations[i]);
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
			glm::vec3 newLocation = glm::vec3(0.0f);
			newLocation.x = glm::sin(glm::radians((360.0f / count *i) + 3.14f / 2.0f)) * 20;
			newLocation.z = glm::cos(glm::radians((360.0f / count *i) + 3.14f / 2.0f)) * 20;
			newLocation.y = 10.0f;
			spotLightActor->SetLocation(newLocation);

			spotLightActor->SetDirection(glm::vec3(0.0f, 0.0f, 0.0f) - spotLightActor->GetLocation());
			spotLightActor->SetLightColor(glm::vec3(1.0f));
			BravoSpotLightSettings SpotSettings;
			SpotSettings.CutOff			= 5.0f;
			SpotSettings.OuterCutOff	= 15.0f;
			SpotSettings.Intencity = 700;
			spotLightActor->SetSettings(SpotSettings);

			auto coneMesh = spotLightActor->NewObject<BravoStaticMeshComponent>("SpotLightStaticMesh");
			coneMesh->SetMesh(coneAsset);
			coneMesh->SetRotation(glm::vec3(0.0f, 180.0f, 0.0f));
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
			newLocation.x = glm::sin(glm::radians(360.0f / count *i)) * 5;
			newLocation.z = glm::cos(glm::radians(360.0f / count *i)) * 5;
			sphereMesh->AddInstance(BravoTransform(newLocation, glm::vec3(BravoMath::RandVector(360.0f)), glm::vec3(0.3f)), false);
		}
		sphereMesh->UpdateInstanceBuffer();
		Cubes.push_back(sphereActor);
	}
}

void BravoGameInstance::Tick(float DeltaTime)
{
	//std::shared_ptr<BravoActor> cube = Cubes[0].lock();
	//cube->SetRotation(glm::vec3(LifeTime*10.0f, 0.0f, 90.0f));

	Test();
	
	for ( int32 i = 0; i < pointLights.size(); ++i )
	{
		glm::vec3 newLocation = glm::vec3(0.0f);
		newLocation.y = 20 + glm::sin(LifeTime) * 20;
		pointLights[i]->SetLocation(newLocation);
	}

	for ( int32 i = 0; i < spotLights.size(); ++i )
	{
		glm::vec3 newLocation = glm::vec3(0.0f);
		newLocation.x = glm::sin(LifeTime + glm::radians(360.0f / spotLights.size() *i)) * 30;
		newLocation.z = glm::cos(LifeTime + glm::radians(360.0f / spotLights.size() *i)) * 30;
	
		newLocation.y = 30.0f;

		spotLights[i]->SetLocation(newLocation);
		spotLights[i]->SetDirection(glm::vec3(0.0f) - newLocation);
	}
}
