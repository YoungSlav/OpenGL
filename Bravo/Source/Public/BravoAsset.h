#pragma once

#include "stdafx.h"
#include "openGL.h"




class BravoAsset
{
public:

	BravoAsset(std::shared_ptr<class BravoAssetManager> _AssetManager);
	virtual ~BravoAsset() = default;

	bool Initialize(const std::string& _Path, const std::vector<std::string>& _Params = std::vector<std::string>());

	virtual void Use() {};
	virtual void StopUsage() {};

	inline bool IsInitialized() const { return bInitialized; }
	inline bool IsLoadedToGPU() const { return bLoadedToGPU; }

	bool LoadToGPU();
	virtual void Render() {}
	void ReleaseFromGPU();

protected:
	virtual bool Initialize_Internal(const std::vector<std::string>& _Params = std::vector<std::string>()) {return true;}

	virtual bool LoadToGPU_Internal() {return true;}
	virtual void ReleaseFromGPU_Internal() {}


	std::shared_ptr<class BravoAssetManager> GetAssetManager() const
	{
		if ( AssetManager.expired() )
			return nullptr;
		return AssetManager.lock();
	}
	
	std::string Path;

private:

	bool bInitialized = false;
	bool bLoadedToGPU = false;

	std::weak_ptr<class BravoAssetManager> AssetManager;

	BravoAsset(const BravoAsset&) = delete;
	BravoAsset& operator=(const BravoAsset& ) = delete;
};