#pragma once
#include "stdafx.h"
#include "BravoPostProcess.h"

class BravoPostProcess_AntiAliasing : public BravoPostProcess
{
public:
	template <typename... Args>
	BravoPostProcess_AntiAliasing(uint32 _InputTexture, Args&&... args) :
		BravoPostProcess(std::forward<Args>(args)...),
		InputTexture(_InputTexture)
	{}

protected:
	virtual bool Initialize_Internal() override;
	virtual bool EnsureReady() override;
	virtual void Render_Internal() override;

private:
	uint32 InputTexture = 0;
};
