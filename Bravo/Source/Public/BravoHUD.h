#pragma once
#include "stdafx.h"

#include "BravoEngine.h"
#include "BravoObject.h"



class BravoHUD : public BravoObject, public BravoTickable
{
public:
	BravoHUD(std::shared_ptr<class BravoEngine> _Engine, const BravoHandle& _Handle) :
		BravoObject(_Engine, _Handle)
	{}

	void Render();
	glm::vec2 GetTargetScale() const { return Size / TargetSize; };
	void SetSize(const glm::vec2& _Size);
	const glm::vec2& GetSize() const { return Size; }
	const glm::mat4& GetModelMatrix() const { return ModelMatrix; }

protected:
	virtual void Initialize_Internal() override;
	virtual void Tick(float DeltaTime) override;

	std::vector<std::shared_ptr<class BravoWidget_Text>> Widgets;
	std::weak_ptr<BravoWidget_Text> fps;

protected:
	glm::vec2 Size = glm::ivec2(0.0f, 0.0f);
	glm::vec2 TargetSize = glm::vec2(1024.0f, 768.0f);
	glm::mat4 ModelMatrix;



	float FPSUpdateRate = 0.5f;
	float FPSUpdateTimer = 0.0f;
};