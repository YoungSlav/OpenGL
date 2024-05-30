#pragma once
#include "stdafx.h"
#include "openGL.h"
#include "BravoObject.h"
#include "BravoAsset.h"

class BravoAssetManager : public BravoObject
{
public:
	template <typename... Args>
	BravoAssetManager(Args&&... args) :
		BravoObject(std::forward<Args>(args)...)
	{}


	template<typename ClassName, typename... Args>
	std::shared_ptr<ClassName> FindOrLoad(const std::string& Name, Args&&... args)
	{
		static_assert(std::is_base_of_v<BravoAsset, ClassName>);
		
		auto it = LoadedAssets.find(Name);
		if ( it != LoadedAssets.end() )
		{
			return std::dynamic_pointer_cast<ClassName>(it->second);
		}

		if ( std::shared_ptr<ClassName> asset = NewObject<ClassName>(Name) )
		{
			LoadedAssets.insert({Name, asset});
			EAssetLoadingState LoadingState = asset->Load(ResourcesFolderPath, std::forward<Args>(args)...);
			assert(LoadingState != EAssetLoadingState::Failed);

			if ( LoadingState == EAssetLoadingState::AsyncLoading )
			{
				PendingLoadingAssets.push_back(asset);
			}
			else if ( LoadingState != EAssetLoadingState::Failed )
			{
				if ( LoadingState != EAssetLoadingState::Loaded )
					asset->LoadToGPU();

				asset->OnAssetLoaded.Broadcast(asset);
			}
			return asset;
		}

		return nullptr;
	}
	
	const std::string& GetResourceFolder()
	{
		return ResourcesFolderPath;
	}

	void CheckPendingAssets()
	{
		for ( int32 i = (int32)PendingLoadingAssets.size()-1; i >= 0; --i )
		{
			//if ( PendingLoadingAssets[i]->GetLoadingState() == EAssetLoadingState::InRAM )
			//{
			//	PendingLoadingAssets[i]->LoadToGPU();
			//}

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



protected:
	virtual bool Initialize_Internal() override
	{
		if ( !BravoObject::Initialize_Internal() )
			return false;

		ResourcesFolderPath = GetRunningDir() + ResourcesFolderRelativePath;
		return true;
	}

private:
	inline static std::string GetRunningDir()
	{
		int8 buffer[MAX_PATH];
		GetModuleFileName( NULL, buffer, MAX_PATH );
		std::string::size_type pos = std::string( buffer ).find_last_of( "\\/" );
		return std::string( buffer ).substr( 0, pos) + "\\";
	}

private:
	std::vector<std::shared_ptr<class BravoAsset>> PendingLoadingAssets;
	std::map<std::string, std::shared_ptr<class BravoAsset>> LoadedAssets;

	const std::string ResourcesFolderRelativePath = "..\\..\\..\\Resources\\";
	std::string ResourcesFolderPath;
};