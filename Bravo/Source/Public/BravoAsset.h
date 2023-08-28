#pragma once

#include "stdafx.h"
#include "BravoObject.h"
#include "openGL.h"


class BravoAsset : public BravoObject
{
public:
	bool Load(const std::string& ResourcesPath) { return false; }

	virtual void Use() {};
	virtual void StopUsage() {};

	bool EnsureReady();
	inline bool IsLoadedToGPU() const { return bLoadedToGPU; }

	bool LoadToGPU();
	void ReleaseFromGPU();

protected:

	virtual void OnDestroy() override;

	virtual bool LoadToGPU_Internal() {return true;}
	virtual void ReleaseFromGPU_Internal() {}



private:
	bool bLoadedToGPU = false;
};