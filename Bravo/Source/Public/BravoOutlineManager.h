#pragma once
#include "stdafx.h"
#include "BravoObject.h"
#include "BravoSelectionManager.h"

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
	std::vector<BravoSelection> ActiveSelections;
	std::shared_ptr<class BravoRenderTarget> OutlineRenderTarget;

	uint32 PlaneVAO = 0, PlaneVBO = 0;
	std::shared_ptr<class BravoShaderAsset> OutlinePostProccessShader = nullptr;
};
