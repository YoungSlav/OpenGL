#pragma once
#include "stdafx.h"
#include "BravoObject.h"

class BravoRenderTarget : public BravoObject
{
public:
	template <typename... Args>
	BravoRenderTarget(Args&&... args) :
		BravoObject(std::forward<Args>(args)...)
	{}



	void Setup(const glm::ivec2& _Size,
		GLint _InternalFormat,
		GLenum _Format,
		GLenum _Type,
		bool _DepthComponent);

	void Resize(const glm::ivec2& Size);
	void Clean();

	uint32 GetColorTexture() const { return TextureColorBuffer; }
	uint32 GetDepthTexture() const { return TextureDepthBuffer; }

	void Bind();
	void Unbind();

protected:

	virtual void OnDestroy() override;

private:

	uint32 FBO = 0;
	uint32 TextureColorBuffer = 0;
	uint32 TextureDepthBuffer = 0;

	glm::ivec2 Size;

	GLint InternalFormat;
	GLenum Format;
	GLenum Type;
	bool DepthComponent = false;
};