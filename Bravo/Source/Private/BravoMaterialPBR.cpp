#include "BravoMaterialPBR.h"
#include "BravoAssetManager.h"
#include "BravoEngine.h"
#include "BravoShaderAsset.h"


bool BravoMaterialPBR::Initialize_Internal()
{
	if ( !BravoMaterial::Initialize_Internal() )
		return false;
	return true;
}

void BravoMaterialPBR::OnDestroy()
{
	StopUsage();
	BravoMaterial::OnDestroy();
}

void BravoMaterialPBR::Load(const BravoPBRMaterialParams& params)
{
	std::shared_ptr<BravoAssetManager> AssetManager = Engine->GetAssetManager();

	ShaderAsset = AssetManager->FindOrLoad<BravoShaderAsset>("PBRShaderModel", BravoShaderLoadingParams("Shaders\\PBRModel"));

	AlbedoTexture = AssetManager->FindOrLoad<BravoTextureAsset>(params.AlbedoTexture, BravoTextureLoadingParams(params.AlbedoTexture, true));
	MetallicTexture = AssetManager->FindOrLoad<BravoTextureAsset>(params.MetallicTexture, BravoTextureLoadingParams(params.MetallicTexture));
	RoughnessTexture = AssetManager->FindOrLoad<BravoTextureAsset>(params.RoughnessTexture, BravoTextureLoadingParams(params.RoughnessTexture));
	AoTexture = AssetManager->FindOrLoad<BravoTextureAsset>(params.AoTexture, BravoTextureLoadingParams(params.AoTexture));
	NormalTexture = AssetManager->FindOrLoad<BravoTextureAsset>(params.NormalTexture, BravoTextureLoadingParams(params.NormalTexture));

	
	AlbedoColor = params.AlbedoColor;
	MetallicColor = params.MetallicColor;
	RoughnessColor = params.RoughnessColor;
	AoColor = params.AoColor;
}

void BravoMaterialPBR::Use()
{
	if ( !ShaderAsset )
		return;

	if ( !ShaderAsset->EnsureGPUReady() )
		return;

	ShaderAsset->Use();
	
	ShaderAsset->SetTexture("material.albedoTexture", AlbedoTexture);
	ShaderAsset->SetTexture("material.metallicTexture", MetallicTexture);
	ShaderAsset->SetTexture("material.roughnessTexture", RoughnessTexture);
	ShaderAsset->SetTexture("material.aoTexture", AoTexture);
	ShaderAsset->SetTexture("material.normalTexture", NormalTexture);

	ShaderAsset->SetBool("material.useAlbedoTexture", AlbedoTexture != nullptr && AlbedoTexture->EnsureGPUReady() );
	ShaderAsset->SetBool("material.useMetallicTexture", MetallicTexture != nullptr && MetallicTexture->EnsureGPUReady());
	ShaderAsset->SetBool("material.useRoughnessTexture", RoughnessTexture != nullptr && RoughnessTexture->EnsureGPUReady());
	ShaderAsset->SetBool("material.useAoTexture", AoTexture != nullptr && AoTexture->EnsureGPUReady());
	ShaderAsset->SetBool("material.useNormalTexture", NormalTexture != nullptr && NormalTexture->EnsureGPUReady());

	ShaderAsset->SetFloat3("material.albedoColor", AlbedoColor);
	ShaderAsset->SetFloat1("material.metallicColor", MetallicColor);
	ShaderAsset->SetFloat1("material.roughnessColor", RoughnessColor);
	ShaderAsset->SetFloat1("material.aoColor", AoColor);
}

void BravoMaterialPBR::StopUsage()
{
	if ( AlbedoTexture )
		AlbedoTexture->StopUsage();
	if ( MetallicTexture )
		MetallicTexture->StopUsage();
	if ( RoughnessTexture )
		RoughnessTexture->StopUsage();
	if ( AoTexture )
		AoTexture->StopUsage();
	if ( NormalTexture )
		NormalTexture->StopUsage();

	if ( ShaderAsset )
		ShaderAsset->StopUsage();
}