#pragma once
#include "stdafx.h"
#include "BravoWidget.h"

class BravoWidget_Plane : public BravoWidget
{
public:
	BravoWidget_Plane(std::shared_ptr<class BravoEngine> _Engine, const BravoHandle& _Handle, std::shared_ptr<BravoObject> _Owner) : 
		BravoWidget(_Engine, _Handle, _Owner)
	{}

	void SetColor(const glm::vec4& _Color);
	const glm::vec4& GetColor() const { return Color; }

	void SetTexture(std::shared_ptr<class BravoTexture> _Texture);
	std::shared_ptr<class BravoTexture> GetTexture() const { return Texture.expired() ? nullptr : Texture.lock(); }

	void SetShader(std::shared_ptr<class BravoShader> _Shader);

protected:
	virtual bool Initialize_Internal() override;
	virtual void Render_Internal() override;

	const std::shared_ptr<class BravoShader> GetShader() const { return Shader.expired() ? nullptr : Shader.lock(); }

private:
	glm::vec4 Color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

	std::weak_ptr<class BravoShader> Shader;
	std::weak_ptr<class BravoTexture> Texture;
};