#include "BravoTerrainActor.h"
#include "BravoEngine.h"
#include "BravoAssetManager.h"
#include "BravoShaderAsset.h"
#include "BravoTextureAsset.h"
#include "BravoTextureData.h"
#include "BravoCamera.h"




bool BravoTerrainActor::Initialize_Internal()
{
	if ( !BravoActor::Initialize_Internal() )
		return false;

	auto AssetManager = Engine->GetAssetManager();
	if ( !AssetManager )
		return false;

	TerrainShader = AssetManager->FindOrLoad<BravoRenderShaderAsset>("TerrainShader", BravoRenderShaderLoadingParams("Terrain", false, true));
	if ( !TerrainShader )
		return false;

	HeightmapTexture = AssetManager->FindOrLoad<BravoTextureAsset>("TerrainTexture", BravoTextureLoadingParams(TerrainTexturePath));
	if ( !HeightmapTexture )
		return false;
	if ( HeightmapTexture->GetLoadingState() == AsyncLoading )
	{
		HeightmapTexture->OnAssetLoaded.AddSP(Self<BravoTerrainActor>(), &BravoTerrainActor::OnHeightmapLoaded);
	}
	else
	{
		BravoTerrainActor::OnHeightmapLoaded(HeightmapTexture);
	}

	return true;
}

void BravoTerrainActor::OnHeightmapLoaded(std::shared_ptr<BravoAsset> )
{
	if ( !HeightmapTexture )
		return;

	std::vector<Vertex> Vertices;

	if ( !GenerateMesh(Vertices) )
		return;

	// load data into vertex buffers
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
		
		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, Vertices.size() * sizeof(Vertex), &Vertices[0], GL_STATIC_DRAW);  

		// vertex Positions
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
		glEnableVertexAttribArray(0);
		// vertex texture coords
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Vertex::TexCoords));
		glEnableVertexAttribArray(1);

		glPatchParameteri(GL_PATCH_VERTICES, nPatchParts);
		
	glBindVertexArray(0);
}

bool BravoTerrainActor::GenerateMesh(std::vector<Vertex>& OutVerticies)
{
	const std::shared_ptr<BravoTextureData> TextureData = HeightmapTexture->GetTextureData();
	Size = glm::ivec2(TextureData->SizeX, TextureData->SizeY);
	
	OutVerticies.resize(Resolution * Resolution * 4);
	uint32 vIndex = 0;

	const float width = (float)Size.y;
	const float height = (float)Size.y;
	float fRez = (float)Resolution;
	float x, y, z;
	for(uint32 i = 0; i <= Resolution-1; i++)
	{
		for(uint32 j = 0; j <= Resolution-1; j++)
		{
			x = -width/2.0f + width * i / fRez;
			y = 0.0f;
			z = -height/2.0f + height * j / fRez;
			OutVerticies[vIndex].Position = glm::vec3(z, y, x);
			OutVerticies[vIndex].TexCoords = glm::vec2(i / fRez, j / fRez);
			vIndex++;

			x = -width/2.0f + width*(i+1)/fRez;
			y = 0.0f;
			z = -height/2.0f + height*j/fRez;
			OutVerticies[vIndex].Position = glm::vec3(z, y, x);
			OutVerticies[vIndex].TexCoords = glm::vec2((i+1) / fRez, j / fRez);
			vIndex++;

			x = -width/2.0f + width*i/fRez;
			y = 0.0f;
			z = -height/2.0f + height*(j+1)/fRez;
			OutVerticies[vIndex].Position = glm::vec3(z, y, x);
			OutVerticies[vIndex].TexCoords = glm::vec2(i / fRez, (j+1) / fRez);
			vIndex++;

			x = -width/2.0f + width*(i+1)/fRez;
			y = 0.0f;
			z = -height/2.0f + height*(j+1)/fRez;
			OutVerticies[vIndex].Position = glm::vec3(z, y, x);
			OutVerticies[vIndex].TexCoords = glm::vec2((i+1) / fRez, (j+1) / fRez);
			vIndex++;
		}
	}	

	return true;
}

void BravoTerrainActor::OnDestroy()
{
	glDeleteBuffers(1, &VBO);
	glDeleteVertexArrays(1, &VAO);
}

bool BravoTerrainActor::EnsureReady() const 
{
	if ( VAO == 0 || VBO == 0 )
		return false;
	if ( !TerrainShader )
		return false;

	return true;
}


void BravoTerrainActor::Render()
{

	SetScale(glm::vec3(2.0f));
	
	if ( !EnsureReady() )
		return;

	const std::shared_ptr<BravoCamera> camera = Engine->GetCamera();
	if ( !camera )
		return;

	const glm::mat4 CameraProjection = camera->GetProjectionMatrix();
	const glm::mat4 CameraView = camera->GetViewMatrix();
	const glm::vec3 CameraLocation = camera->GetLocation();
	
	const glm::mat4 model = GetTransform_World().GetTransformMatrix();

	TerrainShader->Use();
	HeightmapTexture->Use();

		TerrainShader->SetTexture("heightMap", HeightmapTexture);

		TerrainShader->SetMatrix4d("projection", CameraProjection);
		TerrainShader->SetMatrix4d("view", CameraView);
		TerrainShader->SetMatrix4d("model", model);
			
		glBindVertexArray(VAO);
			glDrawArrays(GL_PATCHES, 0, nPatchParts*Resolution*Resolution);
		glBindVertexArray(0);

	HeightmapTexture->StopUsage();
	TerrainShader->StopUsage();
}

void BravoTerrainActor::RenderDepthMap(std::shared_ptr<class BravoShaderAsset> Shader)
{
	// TODO
}