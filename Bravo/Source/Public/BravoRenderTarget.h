#pragma once
#include "BravoObject.h"

class BravoRenderTarget : public BravoObject
{
public:
	void Setup(const glm::ivec2& Size, std::shared_ptr<class BravoShader> InShader);

	void Resize(const glm::ivec2& Size);
	void Clean();

	void Draw();

	void Use();
	void StopUsage();

protected:

	virtual void OnDestroy() override;

	unsigned int FBO, RBO;
	unsigned int PlaneVAO, PlaneVBO;

	unsigned int TextureColorBuffer;

	std::shared_ptr<class BravoShader> Shader;

	glm::ivec2 Size;
};