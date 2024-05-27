#include "BravoTerrainActor.h"
#include "BravoEngine.h"
#include "BravoAssetManager.h"
#include "BravoShaderAsset.h"
#include "BravoTextureAsset.h"
#include "BravoTextureData.h"




bool BravoTerrainActor::Initialize_Internal()
{
	if ( !BravoActor::Initialize_Internal() )
		return false;

	auto AssetManager = Engine->GetAssetManager();
	if ( !AssetManager )
		return false;

	TerrainShader = AssetManager->FindOrLoad<BravoShaderAsset>("TerrainShader", BravoShaderLoadingParams("Shaders\\Unlit"));
	if ( !TerrainShader )
		return false;

	auto HeightmapTexture = AssetManager->FindOrLoad<BravoTextureAsset>(TerrainTexturePath, BravoTextureLoadingParams(TerrainTexturePath));
	if ( !HeightmapTexture )
		return false;

	std::vector<Vertex> Vertices;
	std::vector<uint32> Indices;


	if ( !GenerateMesh(HeightmapTexture, Vertices, Indices) )
		return false;

	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	// load data into vertex buffers
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, Vertices.size() * sizeof(Vertex), &Vertices[0], GL_STATIC_DRAW);  

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, Indices.size() * sizeof(uint32), &Indices[0], GL_STATIC_DRAW);

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
		
		glBindBuffer(GL_ARRAY_BUFFER,  VBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

		// vertex Positions
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
		glEnableVertexAttribArray(0);
		// vertex normals
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Vertex::Normal));
		glEnableVertexAttribArray(1);
		// vertex texture coords
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Vertex::TexCoords));
		glEnableVertexAttribArray(2);
		// vertex tangent
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Vertex::Tangent));
		glEnableVertexAttribArray(3);
		// vertex bitangent
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Vertex::Bitangent));
		glEnableVertexAttribArray(4);
		// vertex colors
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Vertex::Color));
		glEnableVertexAttribArray(5);

	glBindVertexArray(0);


	return true;
}

bool BravoTerrainActor::GenerateMesh(const std::shared_ptr<class BravoTextureAsset> Hightmap, std::vector<Vertex>& OutVerticies, std::vector<uint32>& OutIndices)
{
	const std::shared_ptr<BravoTextureData> TextureData = Hightmap->GetTextureData();
	OutVerticies.resize(TextureData->SizeX * TextureData->SizeY);
	OutIndices.resize(3);

	Size = glm::ivec2(TextureData->SizeX, TextureData->SizeY);

	int32 Index = 0;

	for ( int32 x = 0; x < TextureData->SizeX; ++x )
	{
		for ( int32 z = 0; z < TextureData->SizeY; ++z )
		{
			assert(Index < OutVerticies.size());
			float y = TextureData->TextureData[Index];
			OutVerticies[Index++].Position = glm::vec3(x, y, z);
		}
	}

	return true;
}

void BravoTerrainActor::OnDestroy()
{
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteVertexArrays(6, &VAO);
}

void BravoTerrainActor::Render()
{
	glBindVertexArray(VAO);
		glDrawArrays(GL_POINT, 0, Size.x * Size.y);
	glBindVertexArray(0);
}

void BravoTerrainActor::RenderDepthMap(std::shared_ptr<class BravoShaderAsset> Shader)
{
	// TODO
}