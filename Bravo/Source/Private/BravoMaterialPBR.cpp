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

	ShaderAsset = AssetManager->FindOrLoad<BravoRenderShaderAsset>("PBRShaderModel", BravoRenderShaderSettings("PBRModel", false, false));

	AlbedoTexture = AssetManager->FindOrLoad<BravoTextureAsset>(params.AlbedoTexture, BravoTextureSettings(params.AlbedoTexture, false, true));
	MetallicTexture = AssetManager->FindOrLoad<BravoTextureAsset>(params.MetallicTexture, BravoTextureSettings(params.MetallicTexture));
	RoughnessTexture = AssetManager->FindOrLoad<BravoTextureAsset>(params.RoughnessTexture, BravoTextureSettings(params.RoughnessTexture));
	AoTexture = AssetManager->FindOrLoad<BravoTextureAsset>(params.AoTexture, BravoTextureSettings(params.AoTexture));
	NormalTexture = AssetManager->FindOrLoad<BravoTextureAsset>(params.NormalTexture, BravoTextureSettings(params.NormalTexture));
	HeightTexture = AssetManager->FindOrLoad<BravoTextureAsset>(params.HeightTexture, BravoTextureSettings(params.HeightTexture));

	
	AlbedoColor = params.AlbedoColor;
	MetallicColor = params.MetallicColor;
	RoughnessColor = params.RoughnessColor;
	AoColor = params.AoColor;
	HeightScale = params.HeightScale;
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
	ShaderAsset->SetTexture("material.heightTexture", HeightTexture);

	ShaderAsset->SetBool("material.useAlbedoTexture", AlbedoTexture != nullptr && AlbedoTexture->EnsureGPUReady() );
	ShaderAsset->SetBool("material.useMetallicTexture", MetallicTexture != nullptr && MetallicTexture->EnsureGPUReady());
	ShaderAsset->SetBool("material.useRoughnessTexture", RoughnessTexture != nullptr && RoughnessTexture->EnsureGPUReady());
	ShaderAsset->SetBool("material.useAoTexture", AoTexture != nullptr && AoTexture->EnsureGPUReady());
	ShaderAsset->SetBool("material.useNormalTexture", NormalTexture != nullptr && NormalTexture->EnsureGPUReady());
	ShaderAsset->SetBool("material.useHeightTexture", HeightTexture != nullptr && HeightTexture->EnsureGPUReady());

	ShaderAsset->SetFloat3("material.albedoColor", AlbedoColor);
	ShaderAsset->SetFloat1("material.metallicColor", MetallicColor);
	ShaderAsset->SetFloat1("material.roughnessColor", RoughnessColor);
	ShaderAsset->SetFloat1("material.aoColor", AoColor);
	ShaderAsset->SetFloat1("material.heightScale", HeightScale);
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
	if ( HeightTexture )
		HeightTexture->StopUsage();

	if ( ShaderAsset )
		ShaderAsset->StopUsage();
}