#pragma once

#include "stdafx.h"
#include "BravoAsset.h"
#include "BravoObject.h"
#include "BravoActor.h"
#include "BravoTexture.h"
#include "BravoShader.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include "assimp/color4.h"
#include <assimp/postprocess.h>


struct Vertex
{
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
	glm::vec3 Tangent;
	glm::vec3 Bitangent;
	glm::vec4 Color;
};

class BravoMesh : public BravoAsset
{
public:
	BravoMesh(std::shared_ptr<class BravoAssetManager> _AssetManager) : 
		BravoAsset(_AssetManager)
	{}
	~BravoMesh();

	const std::vector<Vertex>& GetVerticies() const { return Vertices; }
	const std::vector<uint32>& GetIndices() const { return Indices; }

	GLuint GetVBO() const { return VBO; }
	GLuint GetEBO() const { return EBO; }
 
protected:
	virtual bool Initialize_Internal(const std::vector<std::string>& _Params = std::vector<std::string>()) override;
	virtual bool LoadToGPU_Internal() override;
	virtual void ReleaseFromGPU_Internal() override;


private:
	void ProcessNode(aiNode *node, const aiScene *scene);

private:

	std::vector<Vertex> Vertices;
	std::vector<uint32> Indices;

	GLuint VBO = 0;	// vertex attribute buffer
	GLuint EBO = 0;	// index buffer
};

typedef std::shared_ptr<BravoMesh> BravoMeshPtr;