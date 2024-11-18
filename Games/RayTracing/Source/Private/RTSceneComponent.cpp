#include "RTSceneComponent.h"
#include "BravoEngine.h"
#include "BravoAssetManager.h"
#include "BravoMaterialPBR.h"
#include "BravoStaticMeshComponent.h"
#include "BravoStaticMeshAsset.h"

bool RTSceneComponent::Initialize_Internal()
{
	if ( !BravoComponent::Initialize_Internal() )
		return false;

	OnTransformUpdated.AddSP(Self<RTSceneComponent>(), &RTSceneComponent::OnMyTransformUpdated);

	std::shared_ptr<BravoAssetManager> AssetManager = Engine->GetAssetManager();

	std::shared_ptr<BravoStaticMeshAsset> meshAsset = ComponentType == ERTComponentType::ERTComponentType_Sphere ?
		AssetManager->FindOrLoad<BravoStaticMeshAsset>("SphereAsset", BravoStaticMeshLoadingParams("primitives\\sphere.fbx")) :
		AssetManager->FindOrLoad<BravoStaticMeshAsset>("PlaneAsset", BravoStaticMeshLoadingParams("primitives\\plane.fbx"));
	
		PreviewComponent = NewObject<BravoStaticMeshComponent>();
	PreviewComponent->SetCastShadows(false);
	PreviewComponent->SetMesh(meshAsset);

	BravoPBRMaterialParams MaterialParams;
	MaterialParams.AlbedoColor = glm::vec3(1.0f);
	std::shared_ptr<BravoMaterialPBR> previewMaterial = PreviewComponent->NewObject<BravoMaterialPBR>();
	previewMaterial->Load(MaterialParams);

	PreviewComponent->SetMaterial(previewMaterial);

	return true;
}

void RTSceneComponent::OnMyTransformUpdated(const IBravoTransformable*)
{
	OnComponentUpdatedDelegate.Broadcast(this);
}

const RTMaterial& RTSceneComponent::GetMaterial() const
{
	return Material;
}

void RTSceneComponent::SetMaterial(const RTMaterial& NewMaterial)
{
	Material = NewMaterial;
	OnComponentUpdatedDelegate.Broadcast(this);
}