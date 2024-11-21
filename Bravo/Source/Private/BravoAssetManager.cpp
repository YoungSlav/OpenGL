#include <fstream>
#include <iostream>
#include "BravoAssetManager.h"

bool BravoAssetManager::IfFileExist(const std::string& FileName)
{
	std::ifstream file(FileName);
	if ( file )
		return true;
	else
		return false;
}

std::string BravoAssetManager::FindAsset(const std::string& AssetName)
{
	for ( const std::string& folder : ResourcesFolders )
	{
		std::string Candidate = folder + AssetName;
		if ( IfFileExist(Candidate) )
			return Candidate;
	}
	return "";
}
std::string BravoAssetManager::FindShader(const std::string& ShaderName)
{
	for ( const std::string& folder : ShaderFolders )
	{
		std::string Candidate = folder + ShaderName;
		if ( IfFileExist(Candidate) )
			return Candidate;
	}
	return "";
}

void BravoAssetManager::CheckPendingAssets()
{
	{
		for ( int32 i = (int32)PendingLoadingAssets.size()-1; i >= 0; --i )
		{
			auto asset = PendingLoadingAssets[i];
			EAssetLoadingState LoadingState = asset->GetLoadingState();
			if (LoadingState != EAssetLoadingState::AsyncLoading )
			{
				if ( LoadingState != EAssetLoadingState::Failed )
				{
					if ( LoadingState != EAssetLoadingState::Loaded )
						asset->LoadToGPU();
					asset->OnAssetLoaded.Broadcast(asset);
				}

				PendingLoadingAssets.erase(PendingLoadingAssets.begin()+i);
			}
		}
	}
}