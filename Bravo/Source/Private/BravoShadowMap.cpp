#include "stdafx.h"
#include "openGL.h"
#include "BravoShadowMap.h"
#include "BravoShader.h"
#include "BravoEngine.h"
#include "BravoAssetManager.h"
#include "BravoLightActor.h"
#include "BravoTextureUnitManager.h"


void BravoShadowMap::OnDestroy()
{
	if ( DepthMapShader )
		DepthMapShader->ReleaseFromGPU();
	DepthMapShader.reset();

	BravoObject::OnDestroy();
}

void BravoShadowMap_Texture::Setup(const uint32 InSize)
{
	Size = InSize;

	glGenFramebuffers(1, &DepthMapFBO);
	glGenTextures(1, &DepthMap);
	glBindTexture(GL_TEXTURE_2D, DepthMap);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, Size, Size, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER); 
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

		glBindFramebuffer(GL_FRAMEBUFFER, DepthMapFBO);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, DepthMap, 0);
			glDrawBuffer(GL_NONE);
			glReadBuffer(GL_NONE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//glBindTexture(GL_TEXTURE_2D, 0);
	DepthMapShader = Engine->GetAssetManager()->LoadAsset<BravoShader>("Shaders\\ShadowMapDir");
}

void BravoShadowMap_Texture::OnDestroy()
{
	BravoShadowMap::OnDestroy();
	glDeleteFramebuffers(1, &DepthMapFBO);
	glDeleteTextures(1, &DepthMap);
}


void BravoShadowMap_Texture::Use(BravoShaderPtr OnShader, const std::string& Path)
{
	TextureUnit = BravoTextureUnitManager::BindTexture();
	glActiveTexture(GL_TEXTURE0 + TextureUnit);
	glBindTexture(GL_TEXTURE_2D,  DepthMap);

	OnShader->SetMatrix4d(Path + "lightSpaceMatrix", LightSpaceMatrix);
	OnShader->SetInt(Path + "shadowMap", TextureUnit);
}
void BravoShadowMap_Texture::StopUsage()
{
	BravoTextureUnitManager::UnbindTexture(TextureUnit);
	//glBindTexture(GL_TEXTURE_2D,  0);
}



void BravoShadowMap_Cube::Setup(const uint32 InSize)
{
	glGenFramebuffers(1, &DepthMapFBO);
	glGenTextures(1, &DepthCubemap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, DepthCubemap);

		for (uint32 i = 0; i < 6; ++i)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
				InSize, InSize, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		}

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);


		glBindFramebuffer(GL_FRAMEBUFFER, DepthMapFBO);
			glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, DepthCubemap, 0);
			glDrawBuffer(GL_NONE);
			glReadBuffer(GL_NONE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	DepthMapShader = Engine->GetAssetManager()->LoadAsset<BravoShader>("Shaders\\ShadowMapPoint");
}

void BravoShadowMap_Cube::Use(BravoShaderPtr OnShader, const std::string& Path)
{
	TextureUnit = BravoTextureUnitManager::BindTexture();
}
void BravoShadowMap_Cube::StopUsage()
{
	BravoTextureUnitManager::UnbindTexture(TextureUnit);
}

void BravoShadowMap_Cube::OnDestroy()
{
	BravoShadowMap::OnDestroy();
	glDeleteFramebuffers(1, &DepthMapFBO);
	glDeleteTextures(1, &DepthCubemap);
}



void BravoShadowMap_Spot::Render(std::shared_ptr<class BravoLightActor> Owner)
{
	if ( !DepthMapShader )
		return;

	float NearPlane = 0.1f;
	float FarPlane = 1000.0f;

	glm::vec3 LightPosition = Owner->GetLocation();
	
	float FOV = glm::radians(90.0f);
	if ( std::shared_ptr<BravoSpotLightActor> asSpotLight = std::dynamic_pointer_cast<BravoSpotLightActor>(Owner) )
		FOV = asSpotLight->OuterCutOff;

	float AspectRatio = 1.0f;
	
	
	glm::mat4 ShadowProjection = glm::perspective(FOV, AspectRatio, NearPlane, FarPlane);
	glm::mat4 LightView = glm::lookAt(LightPosition, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
	LightSpaceMatrix = ShadowProjection * LightView;
	

	DepthMapShader->Use();
	DepthMapShader->SetMatrix4d("lightSpaceMatrix", LightSpaceMatrix);
	glViewport(0, 0, Size, Size);
	glBindFramebuffer(GL_FRAMEBUFFER, DepthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		Engine->RenderDepthMap(DepthMapShader);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	DepthMapShader->StopUsage();
}
void BravoShadowMap_Point::Render(std::shared_ptr<class BravoLightActor> Owner)
{
	if ( !DepthMapShader )
		return;

	glm::mat4 ShadowProjection;
	
	glm::vec3 LightPosition = Owner->GetLocation();

	float AspectRatio = 1.0f;
	float NearPlane = 1.0f;
	float FarPlane = 1000.0f;
	ShadowProjection = glm::perspective(glm::radians(90.0f), AspectRatio, NearPlane, FarPlane);

	ShadowTransforms.push_back(ShadowProjection * 
		glm::lookAt(LightPosition, LightPosition + glm::vec3( 1.0, 0.0, 0.0), glm::vec3(0.0,-1.0, 0.0)));
	ShadowTransforms.push_back(ShadowProjection * 
		glm::lookAt(LightPosition, LightPosition + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0,-1.0, 0.0)));
	ShadowTransforms.push_back(ShadowProjection * 
		glm::lookAt(LightPosition, LightPosition + glm::vec3( 0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
	ShadowTransforms.push_back(ShadowProjection * 
		glm::lookAt(LightPosition, LightPosition + glm::vec3( 0.0,-1.0, 0.0), glm::vec3(0.0, 0.0,-1.0)));
	ShadowTransforms.push_back(ShadowProjection * 
		glm::lookAt(LightPosition, LightPosition + glm::vec3( 0.0, 0.0, 1.0), glm::vec3(0.0,-1.0, 0.0)));
	ShadowTransforms.push_back(ShadowProjection * 
		glm::lookAt(LightPosition, LightPosition + glm::vec3( 0.0, 0.0,-1.0), glm::vec3(0.0,-1.0, 0.0)));

	glViewport(0, 0, Size, Size);
	glBindFramebuffer(GL_FRAMEBUFFER, DepthMapFBO);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		DepthMapShader->Use();
		for (uint32 i = 0; i < 6; ++i)
			DepthMapShader->SetMatrix4d("shadowMatrices[" + std::to_string(i) + "]", ShadowTransforms[i]);

		DepthMapShader->SetVector1d("far_plane", FarPlane);
		DepthMapShader->SetVector3d("lightPos", LightPosition);
		
		// render scene
		Engine->RenderDepthMap(DepthMapShader);

		DepthMapShader->StopUsage();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}