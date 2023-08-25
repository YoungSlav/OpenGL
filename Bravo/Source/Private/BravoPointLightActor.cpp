#include "stdafx.h"

#include "BravoPointLightActor.h"
#include "BravoShader.h"
#include "BravoEngine.h"
#include "BravoMath.h"
#include "BravoAssetManager.h"
#include "BravoTextureUnitManager.h"

bool BravoDepthMapPoint::Initialize_Internal()
{
	if ( !BravoDepthMap::Initialize_Internal() )
		return false;

	PointLightOwner = std::dynamic_pointer_cast<BravoPointLightActor>(GetOwner());
	if ( !PointLightOwner )
	{
		Log::LogMessage("Owner of point depth map can be only point light actor! " + GetName(), ELog::Error);
		return false;
	}

	return true;
}


void BravoDepthMapPoint::Setup(const uint32 InSize)
{
	Size = InSize;

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

	DepthMapShader = Engine->GetAssetManager()->LoadAsset<BravoShader>("Shaders\\DepthMapPoint");
}

void BravoDepthMapPoint::OnDestroy()
{
	PointLightOwner.reset();
	BravoDepthMap::OnDestroy();
	glDeleteFramebuffers(1, &DepthMapFBO);
	glDeleteTextures(1, &DepthCubemap);
}


void BravoDepthMapPoint::Use(BravoShaderPtr OnShader, const std::string& Path)
{
	TextureUnit = BravoTextureUnitManager::BindTexture();
	glActiveTexture(GL_TEXTURE0 + TextureUnit);
	glBindTexture(GL_TEXTURE_CUBE_MAP,  DepthCubemap);
	OnShader->SetInt(Path + "depthMap", TextureUnit);
}

void BravoDepthMapPoint::StopUsage()
{
	BravoTextureUnitManager::UnbindTexture(TextureUnit);
}


void BravoDepthMapPoint::Render(std::shared_ptr<class BravoLightActor> Owner)
{
	if ( !DepthMapShader )
		return;

	glm::mat4 ShadowProjection;
	
	glm::vec3 LightPosition = Owner->GetLocation();

	float AspectRatio = 1.0f;
	float NearPlane = 0.1f;
	float FarPlane = PointLightOwner->LightAttenuationConstants.Distance[PointLightOwner->GetSettings().Intencity];
	ShadowProjection = glm::perspective(glm::radians(90.0f), AspectRatio, NearPlane, FarPlane);

	LightSpaceMatricies.push_back(ShadowProjection * 
		glm::lookAt(LightPosition, LightPosition + glm::vec3( 1.0, 0.0, 0.0), glm::vec3(0.0,-1.0, 0.0)));
	LightSpaceMatricies.push_back(ShadowProjection * 
		glm::lookAt(LightPosition, LightPosition + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0,-1.0, 0.0)));
	LightSpaceMatricies.push_back(ShadowProjection * 
		glm::lookAt(LightPosition, LightPosition + glm::vec3( 0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
	LightSpaceMatricies.push_back(ShadowProjection * 
		glm::lookAt(LightPosition, LightPosition + glm::vec3( 0.0,-1.0, 0.0), glm::vec3(0.0, 0.0,-1.0)));
	LightSpaceMatricies.push_back(ShadowProjection * 
		glm::lookAt(LightPosition, LightPosition + glm::vec3( 0.0, 0.0, 1.0), glm::vec3(0.0,-1.0, 0.0)));
	LightSpaceMatricies.push_back(ShadowProjection * 
		glm::lookAt(LightPosition, LightPosition + glm::vec3( 0.0, 0.0,-1.0), glm::vec3(0.0,-1.0, 0.0)));

	DepthMapShader->Use();
	
	for (uint32 i = 0; i < 6; ++i)
		DepthMapShader->SetMatrix4d("lightSpaceMatricies[" + std::to_string(i) + "]", LightSpaceMatricies[i]);

	DepthMapShader->SetVector1d("far_plane", FarPlane);
	DepthMapShader->SetVector3d("lightPos", LightPosition);

	glViewport(0, 0, Size, Size);
	glBindFramebuffer(GL_FRAMEBUFFER, DepthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		Engine->RenderDepthMap(DepthMapShader);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	DepthMapShader->StopUsage();
}

bool BravoPointLightActor::Initialize_Internal()
{
	if ( !BravoLightActor::Initialize_Internal() )
		return false;

	DepthMap = NewObject<BravoDepthMapPoint>();
	DepthMap->Setup(2048);

	return true;
}

void BravoPointLightActor::SetSettings(BravoPointLightSettings _Settings)
{
	Settings = _Settings;
	Settings.Intencity = std::min(Settings.Intencity, (uint32)(LightAttenuationConstants.Distance.size()-1));
}

void BravoPointLightActor::Use(BravoShaderPtr OnShader)
{
	BravoLightActor::Use(OnShader);
	OnShader->SetVector3d(Path + "light.ambient", LightColor.ambient);
	OnShader->SetVector3d(Path + "light.diffuse", LightColor.diffuse);
	OnShader->SetVector3d(Path + "light.ambient", LightColor.ambient);
	
	OnShader->SetVector3d(Path + "position", GetLocation());
	OnShader->SetVector1d(Path + "constant", LightAttenuationConstants.Constant[Settings.Intencity]);
	OnShader->SetVector1d(Path + "linear", LightAttenuationConstants.Linear[Settings.Intencity]);
	OnShader->SetVector1d(Path + "quadratic", LightAttenuationConstants.Quadratic[Settings.Intencity]);

	OnShader->SetVector1d(Path + "farPlane", LightAttenuationConstants.Distance[Settings.Intencity]);
}