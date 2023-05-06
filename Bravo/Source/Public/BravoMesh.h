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

class BravoMeshPart
{
public:
	BravoMeshPart(aiMesh *mesh, const aiScene *scene);
	~BravoMeshPart();


	void LoadToGPU();
	void Render();
	void ReleaseFromGPU();

private:
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

	unsigned int VAO = 0;	// vertex array buffer (holds both vertex attributes and indices)
	unsigned int VBO = 0;	// vertex attribute buffer
	unsigned int EBO = 0;	// index buffer
};

class BravoMesh : public BravoAsset
{
public:
	BravoMesh(std::shared_ptr<class BravoAssetManager> _AssetManager) : 
		BravoAsset(_AssetManager)
	{}
	~BravoMesh();
 
	BravoMaterialPtr GetMaterial() const;
	void SetMaterial(BravoMaterialPtr _Material);
	
	virtual void Render() override;

protected:
	virtual bool Initialize_Internal(const std::vector<std::string>& _Params = std::vector<std::string>()) override;
	virtual bool LoadToGPU_Internal() override;
	virtual void ReleaseFromGPU_Internal() override;


private:
	void ProcessNode(aiNode *node, const aiScene *scene);
	std::shared_ptr<BravoMeshPart> ProcessMesh(aiMesh *mesh, const aiScene *scene);

protected:
	std::vector<std::shared_ptr<BravoMeshPart>>	MeshParts;
	BravoMaterialPtr Material;
};

typedef std::shared_ptr<BravoMesh> BravoMeshPtr;