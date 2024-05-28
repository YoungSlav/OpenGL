#pragma once
#include "stdafx.h"
#include "BravoActor.h"
#include "IBravoRenderable.h"
#include "BravoVertex.h"

class BravoTerrainActor : public BravoActor, public IBravoRenderable
{
public:
	template <typename... Args>
	BravoTerrainActor(const std::string& _TerrainTexturePath, uint32 _Resolution, Args&&... args) :
		BravoActor(std::forward<Args>(args)...),
		IBravoRenderable(ERenderPriority::Early),
		TerrainTexturePath(_TerrainTexturePath),
		Resolution(_Resolution)
	{}

	virtual bool Initialize_Internal() override;
	virtual void OnDestroy() override;

	virtual void Render() override;
	virtual void RenderDepthMap(std::shared_ptr<class BravoShaderAsset> Shader) override;

protected:
	bool EnsureReady() const;
	void OnHeightmapLoaded(std::shared_ptr<class BravoAsset> Heightmap);
	bool GenerateMesh(std::vector<Vertex>& OutVerticies);

private:
	std::shared_ptr<class BravoShaderAsset> TerrainShader = nullptr;
	std::shared_ptr<class BravoTextureAsset> HeightmapTexture = nullptr;


	uint32 IndiciesNum = 0;
	
	glm::ivec2 Size = glm::ivec2(0);;
	const std::string TerrainTexturePath = "";
	const uint32 Resolution = 20;


	GLuint VAO = 0;
	GLuint VBO = 0;

	const uint32 nPatchParts = 4;
};
