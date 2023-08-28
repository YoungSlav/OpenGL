#include "BravoDirectionalLightActor.h"

#include "BravoShaderAsset.h"
#include "BravoEngine.h"
#include "BravoMath.h"
#include "BravoAssetManager.h"
#include "BravoTextureUnitManager.h"



bool BravoDirectionalLightActor::Initialize_Internal()
{
	if ( !BravoLightActor::Initialize_Internal() )
		return false;

	return true;
}

void BravoDirectionalLightActor::GetShaderData(std::vector<BravoDirectionalLightShaderData>& OutShaderData) const
{
	int32 Offset = (int32)OutShaderData.size();
	float Near = Engine->GetCamera()->GetMinDrawingDistance();
	float Far = Engine->GetCamera()->GetMaxDrawingDistance();
	for ( size_t i = 0; i < Settings.ShadowCascadeLevels.size(); ++i )
	{
		BravoDirectionalLightShaderData newData;
		newData.AmbientLight = LightColor.ambient;
		newData.DiffuseLight = LightColor.diffuse;
		newData.SpecularLight = LightColor.specular;
		newData.Direction = GetDirection();
		newData.LayerOffset = Offset;
		newData.CascadeCount = (int32)Settings.ShadowCascadeLevels.size();

		newData.cascadePlaneDistance = Settings.ShadowCascadeLevels[i] * Far;
		newData.LightSpaceMatrix = GetLightSpaceMatrix(i == 0 ? Near : Settings.ShadowCascadeLevels[i - 1] * Far, Settings.ShadowCascadeLevels[i] * Far);
		OutShaderData.push_back(newData);
	}
}

glm::mat4 BravoDirectionalLightActor::GetLightSpaceMatrix(const float nearPlane, const float farPlane) const
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

	const auto lightView = glm::lookAt(center - GetDirection(), center, glm::vec3(FLT_EPS, 1.0f, FLT_EPS));

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
	const float zMult = Settings.FrustrumMultiplicator;
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
