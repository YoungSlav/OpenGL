#pragma once
#include "stdafx.h"
#include "BravoWidget.h"

class BravoWidget_Plane : public BravoWidget
{
public:

	void SetColor(const glm::vec4& _Color);
	const glm::vec4& GetColor() const { return Color; }

	void SetTexture(std::shared_ptr<class BravoTextureAsset> _Texture);
	std::shared_ptr<class BravoTextureAsset> GetTexture() const { return Texture.expired() ? nullptr : Texture.lock(); }

	void SetShader(std::shared_ptr<class BravoShaderAsset> _Shader);

protected:
	virtual bool Initialize_Internal() override;
	virtual void Render_Internal() override;

	const std::shared_ptr<class BravoShaderAsset> GetShader() const { return Shader.expired() ? nullptr : Shader.lock(); }

private:
	glm::vec4 Color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

	std::weak_ptr<class BravoShaderAsset> Shader;
	std::weak_ptr<class BravoTextureAsset> Texture;
};