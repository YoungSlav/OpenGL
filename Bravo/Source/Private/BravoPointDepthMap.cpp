#include "BravoPointDepthMap.h"
#include "BravoEngine.h"
#include "BravoTextureUnitManager.h"
#include "BravoAssetManager.h"
#include "BravoShaderAsset.h"
#include "BravoPointLightActor.h"

bool BravoPointDepthMap::Initialize_Internal()
{
	if ( !BravoDepthMapNew::Initialize_Internal() )
		return false;

	DepthMapShader = Engine->GetAssetManager()->FindOrLoad<BravoShaderAsset>("PointDepthMapShader", BravoShaderLoadingParams("Shaders\\DepthMapPoint"));

	return true;
}

void BravoPointDepthMap::Setup(const int32 LayersCount, const uint32 TextureSize)
{
	if ( LayersCount == Layers && TextureSize == Size )
		return;

	ClearGPUData();

	Layers = std::max(LayersCount, 6);
	Size = TextureSize;

	glGenFramebuffers(1, &DepthMapFBO);
	glGenTextures(1, &DepthMapsTextures);
	glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, DepthMapsTextures);
		glTexStorage3D(GL_TEXTURE_CUBE_MAP_ARRAY, 1, GL_DEPTH_COMPONENT32F, Size, Size, Layers);
		

		glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		constexpr float bordercolor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glTexParameterfv(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_BORDER_COLOR, bordercolor);

		Engine->PushFramebuffer(DepthMapFBO, glm::ivec2(Size));
			glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, DepthMapsTextures, 0);
			glDrawBuffer(GL_NONE);
			glReadBuffer(GL_NONE);
		Engine->PopFramebuffer();
}

void BravoPointDepthMap::ClearGPUData()
{
	BravoTextureUnitManager::UnbindTexture(TextureUnit);
	glDeleteFramebuffers(1, &DepthMapFBO);
	DepthMapFBO = 0;
	glDeleteTextures(1, &DepthMapsTextures);
	DepthMapsTextures = 0;
}

void BravoPointDepthMap::OnDestroy()
{
	ClearGPUData();
	BravoDepthMapNew::OnDestroy();
}

void BravoPointDepthMap::Render(const std::vector<BravoPointLightShaderData>& CastersData)
{
	if ( !DepthMapShader )
		return;

	DepthMapShader->Use();
	Engine->PushFramebuffer(DepthMapFBO, glm::ivec2(Size));
		glClear(GL_DEPTH_BUFFER_BIT);

		for ( size_t cIdx = 0; cIdx < CastersData.size(); ++cIdx )
		{
			std::vector<glm::mat4> lightSpaceMatricies;
			glm::mat4 ShadowProjection;
			glm::vec3 LightPosition = CastersData[cIdx].Position;

			float AspectRatio = 1.0f;
			float NearPlane = 0.1f;
			float FarPlane = CastersData[cIdx].FarPlane;
			ShadowProjection = glm::perspective(glm::radians(90.0f), AspectRatio, NearPlane, FarPlane);

			lightSpaceMatricies.push_back(ShadowProjection * 
				glm::lookAt(LightPosition, LightPosition + glm::vec3( 1.0, 0.0, 0.0), glm::vec3(0.0,-1.0, 0.0)));
			lightSpaceMatricies.push_back(ShadowProjection * 
				glm::lookAt(LightPosition, LightPosition + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0,-1.0, 0.0)));
			lightSpaceMatricies.push_back(ShadowProjection * 
				glm::lookAt(LightPosition, LightPosition + glm::vec3( 0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
			lightSpaceMatricies.push_back(ShadowProjection * 
				glm::lookAt(LightPosition, LightPosition + glm::vec3( 0.0,-1.0, 0.0), glm::vec3(0.0, 0.0,-1.0)));
			lightSpaceMatricies.push_back(ShadowProjection * 
				glm::lookAt(LightPosition, LightPosition + glm::vec3( 0.0, 0.0, 1.0), glm::vec3(0.0,-1.0, 0.0)));
			lightSpaceMatricies.push_back(ShadowProjection * 
				glm::lookAt(LightPosition, LightPosition + glm::vec3( 0.0, 0.0,-1.0), glm::vec3(0.0,-1.0, 0.0)));

			for (uint32 i = 0; i < 6; ++i)
				DepthMapShader->SetMatrix4d("lightSpaceMatricies[" + std::to_string(i) + "]", lightSpaceMatricies[i]);

			DepthMapShader->SetInt("layerOffset", cIdx);
			DepthMapShader->SetVector1d("farPlane", FarPlane);
			DepthMapShader->SetVector3d("position", LightPosition);
	
			Engine->RenderDepthMap(DepthMapShader);
		}

	Engine->PopFramebuffer();
	DepthMapShader->StopUsage();
}

void BravoPointDepthMap::Use(std::shared_ptr<BravoShaderAsset> OnShader)
{
	TextureUnit = BravoTextureUnitManager::BindTexture();
	glActiveTexture(GL_TEXTURE0 + TextureUnit);
    glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, DepthMapsTextures);
	OnShader->SetInt("pointDepthMaps", TextureUnit);
}

void BravoPointDepthMap::StopUsage()
{
	BravoTextureUnitManager::UnbindTexture(TextureUnit);
}
