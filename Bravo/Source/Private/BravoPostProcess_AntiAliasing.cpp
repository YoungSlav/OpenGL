#include "BravoPostProcess_AntiAliasing.h"
#include "BravoEngine.h"
#include "BravoAssetManager.h"
#include "BravoTextureUnitManager.h"
#include "BravoShaderAsset.h"

bool BravoPostProcess_AntiAliasing::Initialize_Internal()
{
	if ( !BravoPostProcess::Initialize_Internal() )
		return false;

	Shader = Engine->GetAssetManager()->FindOrLoad<BravoShaderAsset>("AntiAliasingShader", BravoShaderLoadingParams("PostProccess"));
	if ( !Shader )
		return false;
	
	return true;
}

bool BravoPostProcess_AntiAliasing::EnsureReady()
{
	if ( InputTexture == 0)
		return false;

	if ( !BravoPostProcess::EnsureReady() )
		return false;

	return true;
}

void BravoPostProcess_AntiAliasing::Render_Internal()
{
	Shader->Use();

	int32 ColorTextureUnit = BravoTextureUnitManager::BindTexture();
	glActiveTexture(GL_TEXTURE0 + ColorTextureUnit);
	glBindTexture(GL_TEXTURE_2D, InputTexture);
	Shader->SetInt("screenTexture", ColorTextureUnit);
	
	Draw();

	Shader->StopUsage();

	BravoTextureUnitManager::UnbindTexture(ColorTextureUnit);
}