#include "BravoLightActor.h"
#include "BravoEngine.h"

#include "BravoAsset.h"


void BravoLightActor::Draw(const glm::vec3& CameraLocation, const glm::mat4& CameraProjection, const glm::mat4& CameraView) const
{
	if ( Mesh )
	{
		std::vector<std::shared_ptr<BravoMeshPart>>	MeshParts = Mesh->GetMeshParts();
		for ( std::shared_ptr<BravoMeshPart>& part : MeshParts )
		{
			glm::mat4 model = TransformModelMatrix();
	
			if ( Shader )
			{
				if ( part->VAO != -1 )
				{
					Shader->Use();
					Shader->SetMatrix4d("projection", CameraProjection);
					Shader->SetMatrix4d("view", CameraView);
					Shader->SetMatrix4d("model", model);
					Shader->SetVector3d("lightColor", LightColor);
        
					// draw mesh
					glBindVertexArray(part->VAO);
					glDrawElements(GL_TRIANGLES, part->Indices.size(), GL_UNSIGNED_INT, 0);

					glBindVertexArray(0);
					glActiveTexture(0);
					
					Shader->StopUsage();
				}
			}
		}
	}
}

void BravoLightActor::Init()
{
	
	Shader = BravoAsset::Load<BravoShader>("LightSource");
	
}

void BravoLightActor::UpdateShadowMap()
{
	ShadowMap.lock()->Draw(Self<BravoLightActor>());
}

void BravoLightActor::Use(BravoShaderPtr OnShader)
{
	ShadowMap.lock()->Use(OnShader, Path);
}
void BravoLightActor::StopUsage()
{
	ShadowMap.lock()->StopUsage();
}


void BravoDirLightActor::Init()
{
	BravoLightActor::Init();
	Mesh = BravoAsset::Load<BravoMesh>("primitives\\cone.fbx");

	ShadowMap = GetEngine()->SpawnObject<BravoShadowMap_Directional>();
	ShadowMap.lock()->Setup(glm::ivec2(2048));
}
void BravoDirLightActor::OnDestroy()
{
	Mesh->UnLoad();
	Shader->UnLoad();
}

void BravoDirLightActor::Use(BravoShaderPtr OnShader)
{
	BravoLightActor::Use(OnShader);
	OnShader->SetVector3d(Path + "direction", GetDirection());
	OnShader->SetVector3d(Path + "lightColor", glm::vec3(1.0));
}
void BravoDirLightActor::StopUsage()
{
	BravoLightActor::StopUsage();
}


void BravoPointLightActor::Init()
{
	BravoLightActor::Init();
	Mesh = BravoAsset::Load<BravoMesh>("primitives\\sphere.fbx");

	ShadowMap = GetEngine()->SpawnObject<BravoShadowMap_Point>();
	ShadowMap.lock()->Setup(glm::ivec2(2048));
}

void BravoPointLightActor::OnDestroy()
{
	Mesh->UnLoad();
	Shader->UnLoad();
}

void BravoPointLightActor::Use(BravoShaderPtr OnShader)
{
	BravoLightActor::Use(OnShader);
	// TODO
}
void BravoPointLightActor::StopUsage()
{
	BravoLightActor::StopUsage();
	// TODO
}






void BravoSpotLightActor::Init()
{
	BravoLightActor::Init();
	Mesh = BravoAsset::Load<BravoMesh>("primitives\\cone.fbx");

	ShadowMap = GetEngine()->SpawnObject<BravoShadowMap_Spot>();
	ShadowMap.lock()->Setup(glm::ivec2(2048));
}

void BravoSpotLightActor::OnDestroy()
{
	Mesh->UnLoad();
	Shader->UnLoad();
}


void BravoSpotLightActor::Use(BravoShaderPtr OnShader)
{
	BravoLightActor::Use(OnShader);
	OnShader->SetVector3d(Path + "lightColor", glm::vec3(1.0));
	OnShader->SetVector3d(Path + "position", GetLocation());
	OnShader->SetVector3d(Path + "direction", GetDirection());
	OnShader->SetVector1d(Path + "cutOff", CutOff);
	OnShader->SetVector1d(Path + "outerCutOff", OuterCutOff);
	OnShader->SetVector1d(Path + "constant", Constant);
	OnShader->SetVector1d(Path + "linear", Linear);
	OnShader->SetVector1d(Path + "quadratic", Quadratic);
}
void BravoSpotLightActor::StopUsage()
{
	BravoLightActor::StopUsage();
}