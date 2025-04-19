#include "BravoDirectionalDepthMap.h"
#include "BravoEngine.h"
#include "BravoShaderAsset.h"
#include "BravoTextureUnitManager.h"
#include "BravoAssetManager.h"
#include "BravoDirectionalLightActor.h"
#include "BravoViewport.h"


bool BravoDirectionalDepthMap::Initialize_Internal()
{
	if ( !BravoDepthMap::Initialize_Internal() )
		return false;

	DepthMapShader = Engine->GetAssetManager()->FindOrLoad<BravoRenderShaderAsset>("DirectionalDepthMapShader", BravoRenderShaderSettings("DepthMapDir", false, false));
	return true;
}
void BravoDirectionalDepthMap::Setup(const int32 LayersCount, const uint32 TextureSize)
{
	if ( LayersCount == Layers && TextureSize == Size )
		return;

	ClearGPUData();

	Size = TextureSize;
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

	Engine->GetViewport()->PushFramebuffer(DepthMapFBO, glm::ivec2(Size));
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, DepthMapsTextures, 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);

	Engine->GetViewport()->PopFramebuffer();
}

void BravoDirectionalDepthMap::Render(const std::vector<BravoDirectionalLightShaderData>& CastersData)
{
	if ( !DepthMapShader )
		return;
		
	DepthMapShader->Use();
	Engine->GetViewport()->PushFramebuffer(DepthMapFBO, glm::ivec2(Size));
		for ( size_t layer = 0; layer < CastersData.size(); ++layer )
		{
			glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, DepthMapsTextures, 0, (int32)layer);
			glClear(GL_DEPTH_BUFFER_BIT);

			
			DepthMapShader->SetMatrix4d("lightSpaceMatrix", CastersData[layer].LightSpaceMatrix);
	
			Engine->GetViewport()->RenderDepthMap(DepthMapShader);
		}
			
	Engine->GetViewport()->PopFramebuffer();
	DepthMapShader->StopUsage();
}

void BravoDirectionalDepthMap::OnDestroy()
{
	ClearGPUData();
	BravoDepthMap::OnDestroy();
}
	
void BravoDirectionalDepthMap::Use(std::shared_ptr<BravoShaderAsset> OnShader)
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