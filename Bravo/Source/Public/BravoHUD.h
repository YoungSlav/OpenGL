#pragma once
#include "stdafx.h"

#include "BravoEngine.h"
#include "BravoObject.h"

typedef MulticastDelegate<const glm::ivec2&> OnHUDResizedSignature;

class BravoHUD : public BravoObject
{
public:
	template <typename... Args>
	BravoHUD(Args&&... args) :
		BravoObject(std::forward<Args>(args)...)
	{}



	void Render(float DeltaTime);
	glm::vec2 GetTargetScale() const { return Size / TargetSize; };
	void SetSize(const glm::vec2& _Size);
	const glm::vec2& GetSize() const { return Size; }
	const glm::vec2& GetTargetSize() const { return TargetSize; }
	const glm::mat4& GetModelMatrix() const { return ModelMatrix; }

	void AddScreen(std::shared_ptr<class BravoScreen> _Screen);
	void RemoveScreen(std::shared_ptr<class BravoScreen> _Screen);

	OnHUDResizedSignature OnHUDResized;

protected:
	virtual bool Initialize_Internal() override;

	void SetupimGUIStyle();


private:
	glm::vec2 Size = glm::ivec2(0.0f, 0.0f);
	glm::vec2 TargetSize = glm::vec2(1024.0f, 768.0f);
	glm::mat4 ModelMatrix;

	std::vector<std::shared_ptr<class BravoScreen>> Screens;
};