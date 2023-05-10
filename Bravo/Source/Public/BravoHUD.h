#pragma once
#include "stdafx.h"

#include "BravoEngine.h"
#include "BravoObject.h"



class BravoHUD : public BravoObject
{
public:
	BravoHUD(std::shared_ptr<class BravoEngine> _Engine, const BravoHandle& _Handle) :
		BravoObject(_Engine, _Handle)
	{}

	void Render();
	float GetTargetScale() const { return Size.y / TargetSize.y; };
	void SetSize(const glm::vec2& _Size);
	const glm::mat4& GetModelMatrix() const { return ModelMatrix; }

protected:
	virtual void Init() override;

	std::shared_ptr<class BravoWidget_Text> Test;

protected:
	glm::vec2 Size = glm::ivec2(0.0f, 0.0f);

	glm::vec2 TargetSize = glm::vec2(1024.0f, 768.0f);

	glm::mat4 ModelMatrix;
};