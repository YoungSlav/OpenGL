#include "stdafx.h"
#include "BravoAsset.h"

BravoAsset::BravoAsset(std::shared_ptr<class BravoAssetManager> _AssetManager) : 
	AssetManager(_AssetManager)
{
}

bool BravoAsset::EnsureReady()
{
	if ( !bInitialized )
		return false;
	if ( !bLoadedToGPU )
		return LoadToGPU();
}

bool BravoAsset::Initialize(const std::string& _Path, const std::vector<std::string>& _Params)
{
	if ( bInitialized )
	{
		Log::LogMessage("Trying to initialize asset " + Path + " twice!", ELog::Warning);
		return false;
	}

	Path = _Path;
	if ( Initialize_Internal(_Params) )
	{
		bInitialized = true;
	}
	return bInitialized;
}

bool BravoAsset::LoadToGPU()
{
	if ( IsInitialized() && !IsLoadedToGPU() )
	{
		if ( LoadToGPU_Internal() )
			bLoadedToGPU = true;
	}
	return bLoadedToGPU;
}
void BravoAsset::ReleaseFromGPU()
{
	ReleaseFromGPU_Internal();
	bLoadedToGPU = false;
}