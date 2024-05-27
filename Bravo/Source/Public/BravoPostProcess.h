#pragma once
#include "stdafx.h"
#include "BravoObject.h"


class BravoPostProcess : public BravoObject
{
public:
	bool Render();

protected:
	virtual bool Initialize_Internal() override;
	virtual void OnDestroy() override;

	virtual bool EnsureReady();
	virtual void Render_Internal() = 0;

	void Draw();

	std::shared_ptr<class BravoShaderAsset> Shader;

private:
	uint32 PlaneVAO = 0, PlaneVBO = 0;
};