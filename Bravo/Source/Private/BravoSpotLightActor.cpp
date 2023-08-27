#include "BravoSpotLightActor.h"

#include "BravoShader.h"
#include "BravoEngine.h"
#include "BravoMath.h"
#include "BravoAssetManager.h"
#include "BravoTextureUnitManager.h"

bool BravoDepthMapSpot::Initialize_Internal()
{
	if ( !BravoDepthMap::Initialize_Internal() )
		return false;

	SpotLightOwner = std::dynamic_pointer_cast<BravoSpotLightActor>(GetOwner());
	if ( !SpotLightOwner )
	{
		Log::LogMessage("Owner of spot depth map can be only spot light actor! " + GetName(), ELog::Error);
		return false;
	}

	return true;
}

void BravoDepthMapSpot::Setup(const uint32 InSize)
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

	DepthMapShader = Engine->GetAssetManager()->LoadAsset<BravoShader>("Shaders\\DepthMapSpot");
}

void BravoDepthMapSpot::OnDestroy()
{
	SpotLightOwner.reset();
	BravoDepthMap::OnDestroy();
	glDeleteFramebuffers(1, &DepthMapFBO);
	glDeleteTextures(1, &DepthMap);
}


void BravoDepthMapSpot::Apply(BravoShaderPtr OnShader, const std::string& Path)
{
	TextureUnit = BravoTextureUnitManager::BindTexture();
	glActiveTexture(GL_TEXTURE0 + TextureUnit);
	glBindTexture(GL_TEXTURE_2D,  DepthMap);
	OnShader->SetInt(Path + "depthMap", TextureUnit);

	//OnShader->SetMatrix4d(Path + "lightSpaceMatrix", LightSpaceMatrix);
}
void BravoDepthMapSpot::StopUsage()
{
	BravoTextureUnitManager::UnbindTexture(TextureUnit);
}


void BravoDepthMapSpot::Render(std::shared_ptr<class BravoLightActor> Owner)
{
	if ( !DepthMapShader )
		return;

	
	

	DepthMapShader->Use();
	//DepthMapShader->SetMatrix4d("lightSpaceMatrix", LightSpaceMatrix);
	glViewport(0, 0, Size, Size);
	glBindFramebuffer(GL_FRAMEBUFFER, DepthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		Engine->RenderDepthMap(DepthMapShader);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	DepthMapShader->StopUsage();
}


bool BravoSpotLightActor::Initialize_Internal()
{
	if ( !BravoLightActor::Initialize_Internal() )
		return false;

	return true;
}

void BravoSpotLightActor::SetSettings(BravoSpotLightSettings _Settings)
{
	Settings = _Settings;
	Settings.Intencity = std::min(Settings.Intencity, (uint32)(LightAttenuationConstants.Distance.size()-1));
}

const glm::mat4& BravoSpotLightActor::GetLightSpaceTransformationMatrix() const
{
	const float NearPlane = 0.1f;	
	const float FarPlane = LightAttenuationConstants.Distance[Settings.Intencity];

	const float FOV = glm::radians(Settings.OuterCutOff * 2.0f);
	const float AspectRatio = 1.0f;
	
	const glm::mat4 ShadowProjection = glm::perspective(FOV, AspectRatio, NearPlane, FarPlane);
	const glm::mat4 LightView = glm::lookAt(GetLocation(), GetLocation() + GetDirection(), glm::vec3(0.0f, 1.0, 0.0f));

	LightSpaceMatrix = ShadowProjection * LightView;
	
	return LightSpaceMatrix;
}

void BravoSpotLightActor::Apply(BravoShaderPtr OnShader)
{
	BravoLightActor::Apply(OnShader);
	OnShader->SetInt(Path + "depthMapLayer", DepthMapLayer);
	OnShader->SetVector3d(Path + "light.ambient", LightColor.ambient);
	OnShader->SetVector3d(Path + "light.diffuse", LightColor.diffuse);
	OnShader->SetVector3d(Path + "light.ambient", LightColor.ambient);
	
	OnShader->SetVector3d(Path + "position", GetLocation());
	OnShader->SetVector3d(Path + "direction", GetDirection());
	OnShader->SetVector1d(Path + "cutOff", glm::cos(glm::radians(Settings.CutOff)));
	OnShader->SetVector1d(Path + "outerCutOff", glm::cos(glm::radians(Settings.OuterCutOff)));
	OnShader->SetVector1d(Path + "constant", LightAttenuationConstants.Constant[Settings.Intencity]);
	OnShader->SetVector1d(Path + "linear", LightAttenuationConstants.Linear[Settings.Intencity]);
	OnShader->SetVector1d(Path + "quadratic", LightAttenuationConstants.Quadratic[Settings.Intencity]);
	OnShader->SetVector1d(Path + "farPlane", LightAttenuationConstants.Distance[Settings.Intencity]);

	OnShader->SetMatrix4d(Path + "lightSpaceMatrix", LightSpaceMatrix);
}