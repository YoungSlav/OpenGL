#include "BravoMaterialUnlit.h"
#include "BravoAssetManager.h"
#include "BravoEngine.h"
#include "BravoShaderAsset.h"


bool BravoMaterialUnlit::Initialize_Internal()
{
	if ( !BravoMaterial::Initialize_Internal() )
		return false;
	return true;
}

void BravoMaterialUnlit::OnDestroy()
{
	StopUsage();
	BravoMaterial::OnDestroy();
}

void BravoMaterialUnlit::Load(const BravoUnlitMaterialParams& params)
{
	std::shared_ptr<BravoAssetManager> AssetManager = Engine->GetAssetManager();

	ShaderAsset = AssetManager->FindOrLoad<BravoRenderShaderAsset>("UnlitShader", BravoRenderShaderSettings("Unlit", false, false));
	AlbedoTexture = AssetManager->FindOrLoad<BravoTextureAsset>(params.AlbedoTexture, BravoTextureSettings(params.AlbedoTexture, true));
	AlbedoColor = params.AlbedoColor;

	bUseVertexColor = params.bUseVertexColor;
}

void BravoMaterialUnlit::Use()
{
	if ( !ShaderAsset )
		return;

	if ( !ShaderAsset->EnsureGPUReady() )
		return;

	ShaderAsset->Use();
	
	ShaderAsset->SetTexture("material.Texture", AlbedoTexture);

	ShaderAsset->SetBool("material.useTexture", AlbedoTexture != nullptr && AlbedoTexture->EnsureGPUReady() );
	ShaderAsset->SetBool("material.useVertexColor", bUseVertexColor);

	ShaderAsset->SetFloat3("material.Color", AlbedoColor);
}

void BravoMaterialUnlit::StopUsage()
{
	if ( AlbedoTexture )
		AlbedoTexture->StopUsage();

	if ( ShaderAsset )
		ShaderAsset->StopUsage();
}