#pragma once

#include "stdafx.h"
#include "BravoAsset.h"
#include "BravoObject.h"
#include "BravoActor.h"
#include "BravoTextureAsset.h"
#include "BravoShaderAsset.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include "assimp/color4.h"
#include <assimp/postprocess.h>
#include "BravoVertex.h"

struct BravoStaticMeshLoadingParams
{
	BravoStaticMeshLoadingParams(const std::string& _Path) :
		MeshPath(_Path)
	{}

	std::string MeshPath;
};

class BravoStaticMeshAsset : public BravoAsset
{
public:
	template <typename... Args>
	BravoStaticMeshAsset(Args&&... args) :
		BravoAsset(std::forward<Args>(args)...)
	{}


	EAssetLoadingState Load(const std::string& ResourcesPath, const BravoStaticMeshLoadingParams& params);

	const std::vector<Vertex>& GetVerticies() const { return Vertices; }
	const std::vector<uint32>& GetIndices() const { return Indices; }

	GLuint GetVBO() const { return VBO; }
	GLuint GetEBO() const { return EBO; }
 
protected:
	void AsyncLoad(const std::string& ResourcesPath, const BravoStaticMeshLoadingParams& params);

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