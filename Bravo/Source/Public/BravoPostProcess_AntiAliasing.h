#pragma once
#include "stdafx.h"
#include "BravoPostProcess.h"

class BravoPostProcess_AntiAliasing : public BravoPostProcess
{
public:
	template <typename... Args>
	BravoPostProcess_AntiAliasing(Args&&... args) :
		BravoPostProcess(std::forward<Args>(args)...)
	{}


	void Setup(uint32 _Texture) { InputTexture = _Texture; }

protected:
	virtual bool Initialize_Internal() override;
	virtual bool EnsureReady() override;
	virtual void Render_Internal() override;

private:
	uint32 InputTexture = 0;
};
