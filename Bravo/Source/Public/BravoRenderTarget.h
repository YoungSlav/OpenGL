#pragma once
#include "BravoObject.h"

class BravoRenderTarget : public BravoObject
{
public:
	BravoRenderTarget(std::shared_ptr<class BravoEngine> _Engine, const BravoHandle& _Handle) :
		BravoObject(_Engine, _Handle)
	{}

	void Setup(const glm::ivec2& Size, std::shared_ptr<class BravoShader> InShader);

	void Resize(const glm::ivec2& Size);
	void Clean();

	void Render();

	void Use();
	void StopUsage();

protected:

	virtual void OnDestroy() override;

	unsigned int FBO = 0, RBO = 0;
	unsigned int PlaneVAO = 0, PlaneVBO = 0;

	unsigned int TextureColorBuffer = 0;

	std::shared_ptr<class BravoShader> Shader;

	glm::ivec2 Size;
};