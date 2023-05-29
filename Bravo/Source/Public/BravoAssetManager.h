#pragma once
#include "stdafx.h"
#include "SharedFromThis.h"
#include "BravoAsset.h"

class BravoAssetManager : public SharedFromThis
{
public:
	BravoAssetManager() = default;
	BravoAssetManager(const BravoAssetManager&) = default;

	template<typename ClassName>
	std::shared_ptr<ClassName> LoadAsset(const std::string& _Path, const std::vector<std::string>& _Params = std::vector<std::string>())
	{
		static_assert(std::is_base_of_v<BravoAsset, ClassName>);
		
		const std::string Path = GetRunningDir() + ResourcesFolderPath + _Path;

		auto it = LoadedAssets.find(Path);
		if ( it != LoadedAssets.end() )
		{
			return std::dynamic_pointer_cast<ClassName>(it->second);
		}

		if ( std::shared_ptr<BravoAsset> asset = std::shared_ptr<ClassName>(new ClassName(Self<BravoAssetManager>())) )
		{
			if ( asset->Initialize(Path, _Params) && asset->LoadToGPU() )
			{
				LoadedAssets.insert( {Path, asset} );
				return std::dynamic_pointer_cast<ClassName>(asset);
			}
		}
		return nullptr;
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
	std::map<std::string, std::shared_ptr<class BravoAsset>> LoadedAssets;

	const std::string ResourcesFolderPath = "..\\..\\Resources\\";
};