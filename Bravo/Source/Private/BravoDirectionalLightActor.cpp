#include "BravoDirectionalLightActor.h"

#include "BravoShader.h"
#include "BravoEngine.h"
#include "BravoMath.h"
#include "BravoAssetManager.h"
#include "BravoTextureUnitManager.h"


bool BravoDepthMapDirectional::Initialize_Internal()
{
	if ( !BravoDepthMap::Initialize_Internal() )
		return false;

	DirLightOwner = std::dynamic_pointer_cast<BravoDirectionalLightActor>(GetOwner());
	if ( !DirLightOwner )
	{
		Log::LogMessage("Owner of directional depth map can be only directional light actor! " + GetName(), ELog::Error);
		return false;
	}

	return true;
}

void BravoDepthMapDirectional::Setup(const uint32 InSize)
{
	Size = InSize;

	// configure light FBO
	// -----------------------
	glGenFramebuffers(1, &DepthMapFBO);

	glGenTextures(1, &DepthMapsTextures);
	
	glBindTexture(GL_TEXTURE_2D_ARRAY, DepthMapsTextures);
	glTexImage3D(
		GL_TEXTURE_2D_ARRAY, 0, GL_DEPTH_COMPONENT32F, Size, Size, int32(DirLightOwner->GetSettings().ShadowCascadeLevels.size()),
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

	//glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// configure UBO
    // --------------------
    glGenBuffers(1, &MatricesUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, MatricesUBO);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4x4) * 16, nullptr, GL_STATIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, MatricesUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

	DepthMapShader = Engine->GetAssetManager()->LoadAsset<BravoShader>("Shaders\\DepthMapDir", {"CASCADE_LEVELS=" + std::to_string(DirLightOwner->GetSettings().ShadowCascadeLevels.size())});
}

void BravoDepthMapDirectional::Apply(BravoShaderPtr OnShader, const std::string& Path)
{
	OnShader->SetInt(Path + "cascadeCount", DirLightOwner->GetSettings().ShadowCascadeLevels.size());
    for (size_t i = 0; i < DirLightOwner->GetSettings().ShadowCascadeLevels.size(); ++i)
    {
        OnShader->SetVector1d(Path + "cascadePlaneDistances[" + std::to_string(i) + "]", DirLightOwner->GetSettings().ShadowCascadeLevels[i] * Engine->GetCamera()->GetMaxDrawingDistance());
		OnShader->SetMatrix4d(Path + "lightSpaceMatrices[" + std::to_string(i) + "]", CacheLightMatrices[i]);
    }

	TextureUnit = BravoTextureUnitManager::BindTexture();
	glActiveTexture(GL_TEXTURE0 + TextureUnit);
    glBindTexture(GL_TEXTURE_2D_ARRAY, DepthMapsTextures);
	OnShader->SetInt(Path + "depthMap", TextureUnit);
}
void BravoDepthMapDirectional::StopUsage()
{
	BravoTextureUnitManager::UnbindTexture(TextureUnit);
}

void BravoDepthMapDirectional::OnDestroy()
{
	DirLightOwner.reset();
	BravoDepthMap::OnDestroy();
}

void BravoDepthMapDirectional::Render(std::shared_ptr<class BravoLightActor> Owner)
{
	// 0. UBO setup
	CacheLightMatrices = GetLightSpaceMatrices(DirLightOwner->GetDirection());
	glBindBuffer(GL_UNIFORM_BUFFER, MatricesUBO);
	for (size_t i = 0; i < CacheLightMatrices.size(); ++i)
	{
		glBufferSubData(GL_UNIFORM_BUFFER, i * sizeof(glm::mat4x4), sizeof(glm::mat4x4), &CacheLightMatrices[i]);
	}
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	DepthMapShader->Use();

	glBindFramebuffer(GL_FRAMEBUFFER, DepthMapFBO);
		glViewport(0, 0, Size, Size);
		glClear(GL_DEPTH_BUFFER_BIT);
		Engine->RenderDepthMap(DepthMapShader);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	DepthMapShader->StopUsage();
}

std::vector<glm::mat4> BravoDepthMapDirectional::GetLightSpaceMatrices(const glm::vec3& LightDirection) const
{
	float Near = Engine->GetCamera()->GetMinDrawingDistance();
	float Far = Engine->GetCamera()->GetMaxDrawingDistance();
	std::vector<glm::mat4> ret;
	for (size_t i = 0; i < DirLightOwner->GetSettings().ShadowCascadeLevels.size(); ++i)
	{
		if (i == 0)
		{
			ret.push_back(GetLightSpaceMatrix(Near, DirLightOwner->GetSettings().ShadowCascadeLevels[i] * Far, LightDirection));
		}
		else if (i < DirLightOwner->GetSettings().ShadowCascadeLevels.size())
		{
			ret.push_back(GetLightSpaceMatrix(DirLightOwner->GetSettings().ShadowCascadeLevels[i - 1] * Far, DirLightOwner->GetSettings().ShadowCascadeLevels[i] * Far, LightDirection));
		}
	}
	return ret;
}

glm::mat4 BravoDepthMapDirectional::GetLightSpaceMatrix(const float nearPlane, const float farPlane, const glm::vec3& LightDirection) const
{
	std::vector<glm::vec4> FrustumCorners;
	{
		const glm::ivec2 ViewportSize = Engine->GetViewportSize();
		const float AspectRatio = ViewportSize.y > 0.0f ? float(ViewportSize.x) / float(ViewportSize.y) : 0.0f;
		glm::mat4 proj = glm::perspective(glm::radians(Engine->GetCamera()->GetFOV()), AspectRatio,  nearPlane, farPlane);
		BravoMath::GetFrustumCornersWorldSpace(proj * Engine->GetCamera()->GetViewMatrix(), FrustumCorners);
	}

	glm::vec3 center = glm::vec3(0, 0, 0);
	for (const glm::vec4& v : FrustumCorners)
	{
		center += glm::vec3(v);
	}
	center /= FrustumCorners.size();

	const auto lightView = glm::lookAt(center - LightDirection, center, glm::vec3(FLT_EPS, 1.0f, FLT_EPS));

	float minX = std::numeric_limits<float>::max();
	float maxX = std::numeric_limits<float>::lowest();
	float minY = std::numeric_limits<float>::max();
	float maxY = std::numeric_limits<float>::lowest();
	float minZ = std::numeric_limits<float>::max();
	float maxZ = std::numeric_limits<float>::lowest();
	for (const auto& v : FrustumCorners)
	{
		const auto trf = lightView * v;
		minX = std::min(minX, trf.x);
		maxX = std::max(maxX, trf.x);
		minY = std::min(minY, trf.y);
		maxY = std::max(maxY, trf.y);
		minZ = std::min(minZ, trf.z);
		maxZ = std::max(maxZ, trf.z);
	}

	// Tune this parameter according to the scene
	const float zMult = DirLightOwner->GetSettings().FrustrumMultiplicator;
	if (minZ < 0)
	{
		minZ *= zMult;
	}
	else
	{
		minZ /= zMult;
	}
	if (maxZ < 0)
	{
		maxZ /= zMult;
	}
	else
	{
		maxZ *= zMult;
	}

	const glm::mat4 lightProjection = glm::ortho(minX, maxX, minY, maxY, minZ, maxZ);
	return lightProjection * lightView;
}




bool BravoDirectionalLightActor::Initialize_Internal()
{
	if ( !BravoLightActor::Initialize_Internal() )
		return false;

	DepthMap = NewObject<BravoDepthMapDirectional>();
	DepthMap->Setup(4096);

	return true;
}

void BravoDirectionalLightActor::Apply(BravoShaderPtr OnShader)
{
	BravoLightActor::Apply(OnShader);
	OnShader->SetVector3d(Path + "direction", GetDirection());
	OnShader->SetVector3d(Path + "light.ambient", LightColor.ambient);
	OnShader->SetVector3d(Path + "light.diffuse", LightColor.diffuse);
	OnShader->SetVector3d(Path + "light.ambient", LightColor.ambient);
}