#pragma once
#include "stdafx.h"
#include "BravoPostProcess.h"
#include "BravoSelectionManager.h"


class BravoPostProcess_Outline : public BravoPostProcess
{
public:
	template <typename... Args>
	BravoPostProcess_Outline(Args&&... args) :
		BravoPostProcess(std::forward<Args>(args)...)
	{}



protected:
	virtual bool Initialize_Internal() override;

	void OnViewportResized(const glm::ivec2& _Size);
	void OnSelectionChanged(const BravoSelection& Selection);

	virtual bool EnsureReady() override;
	virtual void Render_Internal() override;

private:
	
	std::shared_ptr<class BravoRenderTarget> OutlineRenderTarget;


	glm::vec3 OutlineColor = glm::vec3(1.0f, 1.0f, 0.0f);
};
