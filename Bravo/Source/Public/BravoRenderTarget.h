#pragma once
#include "stdafx.h"
#include "BravoObject.h"

class BravoRenderTarget : public BravoObject
{
public:

	void Setup(const glm::ivec2& _Size,
		GLint _InternalFormat,
		GLenum _Format,
		GLenum _Type,
		bool _DepthStencil,
		std::shared_ptr<class BravoShaderAsset> _Shader = nullptr);

	void Resize(const glm::ivec2& Size);
	void Clean();

	// TODO: move render out of here. the owner of render target is responsible for rendering it, or do what ever with it
	void Render();

	void Use();
	void StopUsage();

protected:

	virtual void OnDestroy() override;

private:

	uint32 FBO = 0, RBO = 0;
	uint32 PlaneVAO = 0, PlaneVBO = 0;

	uint32 TextureColorBuffer = 0;

	glm::ivec2 Size;

	std::shared_ptr<class BravoShaderAsset> Shader;
	GLint InternalFormat;
	GLenum Format;
	GLenum Type;
	bool DepthStencil;
};