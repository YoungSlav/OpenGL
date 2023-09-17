#pragma once
#include "stdafx.h"
#include "BravoObject.h"

class BravoViewport : public BravoObject
{
public:
protected:
	virtual bool Initialize_Internal() override;
	virtual void OnDestroy() override;

private:
	std::shared_ptr<class BravoCamera> Camera;
	std::vector< std::shared_ptr<class IBravoRenderable> > RenderableObjects;
	glm::ivec2 ViewportSize;

	uint64 RenderCount = 0;
};
