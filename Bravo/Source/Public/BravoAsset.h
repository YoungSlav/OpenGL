#pragma once

#include "stdafx.h"
#include "BravoObject.h"
#include "openGL.h"


enum EAssetLoadingState : int32
{
	Failed = 0,
	AsyncLoading = 1,
	InRAM = 2,
	Loaded = 3
};

class BravoAsset : public BravoObject
{
public:
	template <typename... Args>
	BravoAsset(Args&&... args) :
		BravoObject(std::forward<Args>(args)...)
	{}


	void Load(const std::string& ResourcesPath) { }

	virtual void Use() {};
	virtual void StopUsage() {};

	bool EnsureGPUReady();
	inline EAssetLoadingState GetLoadingState() const { return LoadingState; }
	 
	bool LoadToGPU();
	void ReleaseFromGPU();

protected:


	virtual void OnDestroy() override;


	virtual bool LoadToGPU_Internal() { return false; }
	virtual void ReleaseFromGPU_Internal() {}



protected:
	EAssetLoadingState LoadingState = EAssetLoadingState::Failed;
};