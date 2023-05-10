#pragma once
#include "stdafx.h"
#include "BravoWidget.h"

enum EBravoTextAlignment
{
	Left,
	Center,
	Right
};

class BravoWidget_Text : public BravoWidget
{
public:
	BravoWidget_Text(std::shared_ptr<class BravoHUD> _HUD, std::shared_ptr<class BravoAssetManager> _AssetManager) : 
		BravoWidget(_HUD, _AssetManager)
	{}

	void SetText(const std::string& _Text);
	const std::string& GetText() const { return Text; }

	void SetTextSize(uint32 _TextSize);
	uint32 GetTextSize() const { return TextSize; }

	void SetTextColor(const glm::vec4& _TextColor);
	const glm::vec4& GetTextColor() const { return TextColor; }

	void SetTextAlignment(EBravoTextAlignment _TextAlignment);
	EBravoTextAlignment GetTextAlignment() const { return TextAlignment; }

	void SetFont(std::shared_ptr<class BravoFont> _Font);

	float GetActualSizeX() const;
	float GetActualSizeY(bool bIncludeLinegap = false) const;
	glm::vec2 GetActualSize() const { return glm::vec2(GetActualSizeX(), GetActualSizeY()); }

protected:
	virtual bool Initialize_Internal() override;
	virtual void Render_Internal() override;
	
	const std::shared_ptr<class BravoFont> GetFont() const { return Font.expired() ? nullptr : Font.lock(); }
	const std::shared_ptr<class BravoShader> GetShader() const { return Shader.expired() ? nullptr : Shader.lock(); }


protected:
	std::string Text = "";
	uint32 TextSize = 0;
	glm::vec4 TextColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	
	EBravoTextAlignment TextAlignment = EBravoTextAlignment::Left;

	std::weak_ptr<class BravoFont> Font;
	std::weak_ptr<class BravoShader> Shader;
};
