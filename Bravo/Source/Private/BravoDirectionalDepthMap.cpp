#include "BravoDirectionalDepthMap.h"
#include "BravoEngine.h"
#include "BravoShader.h"
#include "BravoTextureUnitManager.h"
#include "BravoAssetManager.h"
#include "BravoDirectionalLightActor.h"


bool BravoDirectionalDepthMap::Initialize_Internal()
{
	if ( !BravoDepthMapNew::Initialize_Internal() )
		return false;

	DepthMapShader = Engine->GetAssetManager()->LoadAsset<BravoShader>("Shaders\\DepthMapDir");
	return true;
}
void BravoDirectionalDepthMap::Setup(int32 LayersCount)
{
	ClearGPUData();

	Layers = std::max(LayersCount, 1);

	glGenFramebuffers(1, &DepthMapFBO);

		glGenTextures(1, &DepthMapsTextures);
		glBindTexture(GL_TEXTURE_2D_ARRAY, DepthMapsTextures);
		glTexImage3D(
			GL_TEXTURE_2D_ARRAY, 0, GL_DEPTH_COMPONENT32F, Size, Size, Layers,
			0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

		constexpr float bordercolor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, bordercolor);

		glBindFramebuffer(GL_FRAMEBUFFER, DepthMapFBO);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, DepthMapsTextures, 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void BravoDirectionalDepthMap::Render(int32 Layer, const BravoDirectionalLightShaderData& CasterData)
{
	if ( !DepthMapShader )
		return;
		
	DepthMapShader->Use();
	glBindFramebuffer(GL_FRAMEBUFFER, DepthMapFBO);
		glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, DepthMapsTextures, 0, Layer);

		glViewport(0, 0, Size, Size);
		glClear(GL_DEPTH_BUFFER_BIT);
			
		DepthMapShader->SetMatrix4d("lightSpaceMatrix", CasterData.LightSpaceMatrix);
	
		Engine->RenderDepthMap(DepthMapShader);
			
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	DepthMapShader->StopUsage();
}

void BravoDirectionalDepthMap::OnDestroy()
{
	ClearGPUData();
	BravoDepthMapNew::OnDestroy();
}
	
void BravoDirectionalDepthMap::Use(BravoShaderPtr OnShader)
{
	TextureUnit = BravoTextureUnitManager::BindTexture();
	glActiveTexture(GL_TEXTURE0 + TextureUnit);
    glBindTexture(GL_TEXTURE_2D_ARRAY, DepthMapsTextures);
	OnShader->SetInt("dirDepthMaps", TextureUnit);
}
void BravoDirectionalDepthMap::StopUsage()
{
	BravoTextureUnitManager::UnbindTexture(TextureUnit);
}

void BravoDirectionalDepthMap::ClearGPUData()
{
	BravoTextureUnitManager::UnbindTexture(TextureUnit);
	glDeleteFramebuffers(1, &DepthMapFBO);
	DepthMapFBO = 0;
	glDeleteTextures(1, &DepthMapsTextures);
	DepthMapsTextures = 0;
}