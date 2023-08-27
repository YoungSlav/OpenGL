#include "BravoSpotDepthMap.h"
#include "BravoEngine.h"
#include "BravoTextureUnitManager.h"
#include "BravoAssetManager.h"
#include "BravoShader.h"
#include "BravoSpotLightActor.h"

bool BravoSpotDepthMap::Initialize_Internal()
{
	if ( !BravoDepthMapNew::Initialize_Internal() )
		return false;

	DepthMapShader = Engine->GetAssetManager()->LoadAsset<BravoShader>("Shaders\\DepthMapSpot");

	return true;
}

void BravoSpotDepthMap::Setup(int32 CastersCount)
{
	ClearGPUData();
	
	Layers = std::max(CastersCount, 1);

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

void BravoSpotDepthMap::ClearGPUData()
{
	BravoTextureUnitManager::UnbindTexture(TextureUnit);
	glDeleteFramebuffers(1, &DepthMapFBO);
	DepthMapFBO = 0;
	glDeleteTextures(1, &DepthMapsTextures);
	DepthMapsTextures = 0;
}

void BravoSpotDepthMap::OnDestroy()
{
	ClearGPUData();
	BravoDepthMapNew::OnDestroy();
}

void BravoSpotDepthMap::Render(uint32 ShaderDataSSBO, int32 Count)
{
	if ( !DepthMapShader )
		return;
		
	DepthMapShader->Use();
	glBindFramebuffer(GL_FRAMEBUFFER, DepthMapFBO);
		//glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, DepthMapsTextures, 0, Layer);

		glViewport(0, 0, Size, Size);
		glClear(GL_DEPTH_BUFFER_BIT);
		
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ShaderDataSSBO);
		DepthMapShader->SetInt("spotLightCount", Count);
	
		Engine->RenderDepthMap(DepthMapShader);
			
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	DepthMapShader->StopUsage();
}

void BravoSpotDepthMap::Use(BravoShaderPtr OnShader)
{
	TextureUnit = BravoTextureUnitManager::BindTexture();
	glActiveTexture(GL_TEXTURE0 + TextureUnit);
    glBindTexture(GL_TEXTURE_2D_ARRAY, DepthMapsTextures);
	OnShader->SetInt("spotDepthMaps", TextureUnit);
}
void BravoSpotDepthMap::StopUsage()
{
	BravoTextureUnitManager::UnbindTexture(TextureUnit);
}

