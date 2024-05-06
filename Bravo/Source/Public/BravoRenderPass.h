#pragma once
#include "stdafx.h"
#include "BravoObject.h"

class BravoRenderPass : public BravoObject
{
public:
	void SetRenderTarget(std::shared_ptr<class BravoRenderTarget> _RenderTarget) { RenderTarget = _RenderTarget; }
	std::shared_ptr<class BravoRenderTarget> GetRenderTarget() const { return RenderTarget; }

	virtual void Render() = 0;

protected:
	virtual bool Initialize_Internal() override;
	virtual void OnDestroy() override;

private:
	std::shared_ptr<class BravoRenderTarget> RenderTarget;
};