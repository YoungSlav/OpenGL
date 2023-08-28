#pragma once
#include "stdafx.h"
#include "openGL.h"
#include "BravoObject.h"
#include "BravoAsset.h"

class BravoAssetManager : public BravoObject
{
public:
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
			if ( asset->Load(ResourcesFolderPath, std::forward<Args>(args)...) && asset->LoadToGPU() )
			{
				LoadedAssets.insert({Name, asset});
				return asset;
			}
		}

		return nullptr;
	}
	
	const std::string& GetResourceFolder()
	{
		return ResourcesFolderPath;
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
	std::map<std::string, std::shared_ptr<class BravoAsset>> LoadedAssets;

	const std::string ResourcesFolderRelativePath = "..\\..\\Resources\\";
	std::string ResourcesFolderPath;
};