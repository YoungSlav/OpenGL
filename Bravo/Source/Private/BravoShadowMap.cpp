#include "stdafx.h"
#include "openGL.h"
#include "BravoShadowMap.h"
#include "BravoShader.h"
#include "BravoEngine.h"

#include "BravoLightActor.h"


void BravoShadowMap::OnDestroy()
{
	Shader->UnLoad();
}

void BravoShadowMap_Texture::Setup(const glm::ivec2& InSize)
{
	Size = InSize;

	glGenFramebuffers(1, &DepthMapFBO);
	glGenTextures(1, &DepthMap);

	glBindTexture(GL_TEXTURE_2D, DepthMap);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, Size.x, Size.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
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

	glBindTexture(GL_TEXTURE_2D, 0);

	Shader = BravoAsset::Load<BravoShader>("ShadowMapDir");
}

void BravoShadowMap_Texture::OnDestroy()
{
	BravoShadowMap::OnDestroy();
	glDeleteFramebuffers(1, &DepthMapFBO);
	glDeleteTextures(1, &DepthMap);
}


void BravoShadowMap_Texture::Use(BravoShaderPtr OnShader, const std::string& Path)
{
	TextureUnit = TextureUnitSelector::BindTexture();
	glActiveTexture(GL_TEXTURE0 + TextureUnit);
	glBindTexture(GL_TEXTURE_2D,  DepthMap);

	OnShader->SetMatrix4d(Path + "lightSpaceMatrix", LightSpaceMatrix);
	OnShader->SetInt(Path + "shadowMap", TextureUnit);
}
void BravoShadowMap_Texture::StopUsage()
{
	TextureUnitSelector::UnbindTexture(TextureUnit);
	glBindTexture(GL_TEXTURE_2D,  0);
}



void BravoShadowMap_Cube::Setup(const glm::ivec2& InSize)
{
	Size = InSize;

	glGenFramebuffers(1, &DepthMapFBO);
	glGenTextures(1, &DepthCubemap);

	glBindTexture(GL_TEXTURE_CUBE_MAP, DepthCubemap);

		for (unsigned int i = 0; i < 6; ++i)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
				Size.x, Size.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
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

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	Shader = BravoAsset::Load<BravoShader>("ShadowMapPoint");
}

void BravoShadowMap_Cube::Use(BravoShaderPtr OnShader, const std::string& Path)
{
	TextureUnit = TextureUnitSelector::BindTexture();
}
void BravoShadowMap_Cube::StopUsage()
{
	TextureUnitSelector::UnbindTexture(TextureUnit);
}

void BravoShadowMap_Cube::OnDestroy()
{
	BravoShadowMap::OnDestroy();
	glDeleteFramebuffers(1, &DepthMapFBO);
	glDeleteTextures(1, &DepthCubemap);
}

void BravoShadowMap_Directional::Draw(std::shared_ptr<class BravoLightActor> Owner)
{
	float NearPlane = 1.0f;
	float FarPlane = 50.0f;

	glm::vec3 LightPosition = Owner->GetLocation();

	glm::mat4 ShadowProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, NearPlane, FarPlane);
	glm::mat4 LightView = glm::lookAt(LightPosition, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
	LightSpaceMatrix = ShadowProjection * LightView;
	

	Shader->Use();
	Shader->SetMatrix4d("lightSpaceMatrix", LightSpaceMatrix);
	glViewport(0, 0, Size.x, Size.y);
	glBindFramebuffer(GL_FRAMEBUFFER, DepthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		glCullFace(GL_FRONT);
		GetEngine()->DrawDepthMap(Shader, LightPosition);
		glCullFace(GL_BACK);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	Shader->StopUsage();
}

void BravoShadowMap_Spot::Draw(std::shared_ptr<class BravoLightActor> Owner)
{
	float NearPlane = 0.1f;
	float FarPlane = 50.0f;

	glm::vec3 LightPosition = Owner->GetLocation();
	
	float FOV = glm::radians(90.0f);
	if ( std::shared_ptr<BravoSpotLightActor> asSpotLight = std::dynamic_pointer_cast<BravoSpotLightActor>(Owner) )
		FOV = asSpotLight->OuterCutOff;

	float AspectRatio = (float)(Size.x)/(float)(Size.y);
	
	
	glm::mat4 ShadowProjection = glm::perspective(FOV, AspectRatio, NearPlane, FarPlane);
	glm::mat4 LightView = glm::lookAt(LightPosition, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
	LightSpaceMatrix = ShadowProjection * LightView;
	

	Shader->Use();
	Shader->SetMatrix4d("lightSpaceMatrix", LightSpaceMatrix);
	glViewport(0, 0, Size.x, Size.y);
	glBindFramebuffer(GL_FRAMEBUFFER, DepthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		glCullFace(GL_FRONT);
		GetEngine()->DrawDepthMap(Shader, LightPosition);
		glCullFace(GL_BACK);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	Shader->StopUsage();
}
void BravoShadowMap_Point::Draw(std::shared_ptr<class BravoLightActor> Owner)
{
	glm::mat4 ShadowProjection;
	
	glm::vec3 LightPosition = Owner->GetLocation();

	float AspectRatio = (float)(Size.x)/(float)(Size.y);
	float NearPlane = 1.0f;
	float FarPlane = 100.0f;
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

	glViewport(0, 0, Size.x, Size.y);
	glBindFramebuffer(GL_FRAMEBUFFER, DepthMapFBO);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		Shader->Use();
		for (unsigned int i = 0; i < 6; ++i)
			Shader->SetMatrix4d("shadowMatrices[" + std::to_string(i) + "]", ShadowTransforms[i]);

		Shader->SetVector1d("far_plane", FarPlane);
		Shader->SetVector3d("lightPos", LightPosition);
		
		// render scene
		GetEngine()->DrawDepthMap(Shader, LightPosition);

		Shader->StopUsage();
	//glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}