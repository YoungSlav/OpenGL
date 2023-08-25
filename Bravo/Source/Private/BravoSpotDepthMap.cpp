#include "BravoSpotDepthMap.h"
#include "BravoEngine.h"
#include "BravoTextureUnitManager.h"
#include "BravoLightManager.h"
#include "BravoAssetManager.h"
#include "BravoShader.h"
#include "BravoSpotLightActor.h"

bool BravoSpotDepthMap::Initialize_Internal()
{
	if ( !BravoDepthMap2::Initialize_Internal() )
		return false;

	glGenFramebuffers(1, &DepthMapFBO);

		glGenTextures(1, &DepthMapsTextures);
	
		glBindTexture(GL_TEXTURE_2D_ARRAY, DepthMapsTextures);
		glTexImage3D(
			GL_TEXTURE_2D_ARRAY, 0, GL_DEPTH_COMPONENT32F, Size, Size, Engine->GetLightManager()->GetSpotDepthMapLayersCount(),
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


	DepthMapShader = Engine->GetAssetManager()->LoadAsset<BravoShader>("Shaders\\DepthMapSpot");

	return true;
}

void BravoSpotDepthMap::OnDestroy()
{
	BravoTextureUnitManager::UnbindTexture(TextureUnit);
	glDeleteFramebuffers(1, &DepthMapFBO);
	glDeleteTextures(1, &DepthMapsTextures);

	BravoDepthMap2::OnDestroy();
}

void BravoSpotDepthMap::Render(std::shared_ptr<class BravoLightActor> Caster)
{
	if ( !DepthMapShader )
		return;
	
	std::shared_ptr<BravoSpotLightActor> SpotCaster = std::dynamic_pointer_cast<BravoSpotLightActor>(Caster);
	if ( !SpotCaster )
	{
		Log::LogMessage("Invalid caster for spot depth map rendering!", ELog::Error);
		return;
	}
	int32 DepthMapLayer = SpotCaster->GetDepthMapLayer();
	if ( DepthMapLayer < 0 || DepthMapLayer >= Engine->GetLightManager()->GetSpotDepthMapLayersCount() )
	{
		Log::LogMessage("Invalid depthMap layer specified for " + Caster->GetName(), ELog::Error);
		return;
	}

	const glm::mat4 LightSpaceMatrix = SpotCaster->GetLightSpaceTransformationMatrix();
	
	DepthMapShader->Use();
		DepthMapShader->SetMatrix4d("lightSpaceMatrix", LightSpaceMatrix);
		DepthMapShader->SetInt("depthMapLayer", DepthMapLayer);
		glBindFramebuffer(GL_FRAMEBUFFER, DepthMapFBO);
			glViewport(0, 0, Size, Size);
			glClear(GL_DEPTH_BUFFER_BIT);
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

