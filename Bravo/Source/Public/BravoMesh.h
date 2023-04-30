#pragma once

#include "stdafx.h"
#include "BravoAsset.h"
#include "BravoObject.h"
#include "BravoActor.h"
#include "BravoTexture.h"
#include "BravoShader.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace MeshConstants
{
	const std::string ResourcesFolderPath = "Resources\\";
}

struct BravoMeshPart
{
	void Init();

	struct Vertex
	{
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec2 TexCoords;
		glm::vec3 Tangent;
		glm::vec3 Bitangent;
	};

	std::vector<Vertex> Vertices;
	std::vector<unsigned int> Indices;
	BravoMaterialPtr Material;
	

	unsigned int VAO = -1;
	unsigned int VBO = -1;
	unsigned int EBO = -1;
};

class BravoMesh : public BravoAsset
{
public:
	virtual void Use() override;
	virtual void StopUsage() override;
	 
	inline std::vector<std::shared_ptr<BravoMeshPart>>& GetMeshParts() { return MeshParts; }

	void SetMaterial(unsigned int MeshIndex, BravoMaterialPtr Material);
	
protected:
	virtual bool Load_Internal() override;
	virtual void UnLoad_Internal() override;
	

private:
	void ProcessNode(aiNode *node, const aiScene *scene);
	std::shared_ptr<BravoMeshPart> ProcessMesh(aiMesh *mesh, const aiScene *scene);

protected:
	std::vector<std::shared_ptr<BravoMeshPart>>	MeshParts;
};

typedef std::shared_ptr<BravoMesh> BravoMeshPtr;