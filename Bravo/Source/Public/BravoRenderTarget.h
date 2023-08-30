#pragma once
#include "BravoObject.h"

class BravoRenderTarget : public BravoObject
{
public:

	void Setup(const glm::ivec2& Size, bool HDR, std::shared_ptr<class BravoShaderAsset> InShader);

	void Resize(const glm::ivec2& Size);
	void Clean();

	void Render();

	void Use();
	void StopUsage();

protected:

	virtual void OnDestroy() override;
	std::shared_ptr<class BravoShaderAsset> GetShader() const { return Shader.expired() ? nullptr : Shader.lock(); }

private:

	uint32 FBO = 0, RBO = 0;
	uint32 PlaneVAO = 0, PlaneVBO = 0;

	uint32 TextureColorBuffer = 0;

	std::weak_ptr<class BravoShaderAsset> Shader;

	glm::ivec2 Size;

	bool HDR = false;
};