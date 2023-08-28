#include "stdafx.h"
#include "BravoAsset.h"

bool BravoAsset::EnsureReady()
{
	if ( !bLoadedToGPU )
		return LoadToGPU();
	
	return true;
}

void BravoAsset::OnDestroy()
{
	ReleaseFromGPU();
	BravoObject::OnDestroy();
}

bool BravoAsset::LoadToGPU()
{
	if ( !bLoadedToGPU )
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