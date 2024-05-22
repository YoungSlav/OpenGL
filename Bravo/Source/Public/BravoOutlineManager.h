#pragma once
#include "stdafx.h"
#include "BravoObject.h"
#include "BravoSelectionManager.h"

// TODO: refactor into post process
class BravoOutlineManager : public BravoObject
{
public:

	void RenderSelections();

protected:
	virtual bool Initialize_Internal() override;
	virtual void OnDestroy() override;

	void OnViewportResized(const glm::ivec2& _Size);
	void OnSelectionChanged(const BravoSelection& Selection);

private:
	
	
	std::shared_ptr<class BravoRenderTarget> OutlineRenderTarget;

	uint32 PlaneVAO = 0, PlaneVBO = 0;
	std::shared_ptr<class BravoShaderAsset> OutlinePostProccessShader = nullptr;

	glm::vec3 OutlineColor = glm::vec3(1.0f, 1.0f, 0.0f);
};
