#include "stdafx.h"
#include "BravoAsset.h"

bool BravoAsset::EnsureReady()
{
	if ( LoadingState != EAssetLoadingState::Loaded )
		return LoadToGPU();

	return LoadingState == EAssetLoadingState::Loaded;
}

void BravoAsset::OnDestroy()
{
	ReleaseFromGPU();
	BravoObject::OnDestroy();
}

bool BravoAsset::LoadToGPU()
{
	if ( LoadingState == EAssetLoadingState::InRAM )
	{
		if ( LoadToGPU_Internal() )
			LoadingState = EAssetLoadingState::Loaded;
	}
	return LoadingState == EAssetLoadingState::Loaded;
}
void BravoAsset::ReleaseFromGPU()
{
	ReleaseFromGPU_Internal();
	LoadingState = EAssetLoadingState::InRAM;
}