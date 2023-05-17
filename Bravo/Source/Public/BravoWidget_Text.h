#pragma once
#include "stdafx.h"
#include "BravoWidget.h"

// TODO: multiline support
// TODO: fit to size
class BravoWidget_Text : public BravoWidget
{
public:
	BravoWidget_Text(std::shared_ptr<class BravoEngine> _Engine, const BravoHandle& _Handle) : 
		BravoWidget(_Engine, _Handle)
	{}

	void SetText(const std::string& _Text);
	const std::string& GetText() const { return Text; }

	void SetTextSize(uint32 _TextSize);
	uint32 GetTextSize() const { return TextSize; }

	void SetTextColor(const glm::vec4& _TextColor);
	const glm::vec4& GetTextColor() const { return TextColor; }

	// Margin in pixels
	void SetMargin(const glm::vec2& _Margin);
	const glm::vec2& GetMargin() const { return Margin; }

	void SetFont(std::shared_ptr<class BravoFont> _Font);

	void SetShader(std::shared_ptr<class BravoShader> _Shader);

	virtual glm::vec2 GetActualSize() const override;

protected:
	virtual bool Initialize_Internal() override;
	virtual void Render_Internal() override;
	
	const std::shared_ptr<class BravoFont> GetFont() const { return Font.expired() ? nullptr : Font.lock(); }
	const std::shared_ptr<class BravoShader> GetShader() const { return Shader.expired() ? nullptr : Shader.lock(); }


private:
	std::string Text = "";
	uint32 TextSize = 0;
	glm::vec4 TextColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

	glm::vec2 Margin = glm::vec2(0.0f, 0.0f);
	

	std::weak_ptr<class BravoFont> Font;
	std::weak_ptr<class BravoShader> Shader;
};
