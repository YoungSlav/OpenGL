#pragma once
#include "stdafx.h"
#include "BravoWidget.h"

class BravoWidget_Text : public BravoWidget
{
public:
	BravoWidget_Text(std::shared_ptr<class BravoHUD> _HUD) : 
		BravoWidget(_HUD)
	{}

	void SetText(const std::string& _Text);
	const std::string& GetText() const { return Text; }

	void SetTextSize(uint32 _TextSize);
	uint32 GetTextSize() const { return TextSize; }

	void SetTextColor(const glm::vec4& _TextColor);
	const glm::vec4& GetTextColor() const { return TextColor; }

	void SetFont(std::shared_ptr<class BravoFont> _Font);
	const std::shared_ptr<class BravoFont> GetFont() const { return Font.expired() ? nullptr : Font.lock(); }

protected:
	virtual void Render_Internal() override;

private:
	std::string Text = "";
	uint32 TextSize = 0;
	glm::vec4 TextColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	std::weak_ptr<class BravoFont> Font;
};
