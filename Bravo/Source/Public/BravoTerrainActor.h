#pragma once
#include "stdafx.h"
#include "BravoActor.h"
#include "IBravoRenderable.h"
#include "BravoVertex.h"

class BravoTerrainActor : public BravoActor, public IBravoRenderable
{
public:
	template <typename... Args>
	BravoTerrainActor(const std::string& _TerrainTexturePath, float _Resolution, Args&&... args) :
		BravoActor(std::forward<Args>(args)...),
		IBravoRenderable(),
		TerrainTexturePath(_TerrainTexturePath),
		Resolution(_Resolution)
	{}

	virtual bool Initialize_Internal() override;
	virtual void OnDestroy() override;

	virtual void Render() override;
	virtual void RenderDepthMap(std::shared_ptr<class BravoShaderAsset> Shader) override;

protected:
	bool GenerateMesh(const std::shared_ptr<class BravoTextureAsset> Hightmap, std::vector<Vertex>& OutVerticies, std::vector<uint32>& OutIndices);

private:
	std::shared_ptr<class BravoShaderAsset> TerrainShader = nullptr;

	const float Resolution = 1.0f;
	glm::ivec2 Size;
	const std::string TerrainTexturePath = "";


	GLuint VAO = 0;
	GLuint VBO = 0;
	GLuint EBO = 0;
};
