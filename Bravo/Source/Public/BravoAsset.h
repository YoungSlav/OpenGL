#pragma once

#include "stdafx.h"
#include "openGL.h"


namespace BravoAssetConstants
{
	const std::string ResourcesFolderPath = "..\\Resources\\";
	const std::string ShadersFolderPath = "Shaders\\";
}

class TextureUnitSelector
{
public:
	TextureUnitSelector();
	~TextureUnitSelector();
	static int BindTexture();
	static void UnbindTexture(int TextureUnit);

protected:
	static TextureUnitSelector& Instance()
	{
		static TextureUnitSelector UnitSelector;
		return UnitSelector;
	}

	int FindEmptyTextureUnit();

private:
	int* TextureUnitsStatus;
	int TextureUnitCount;
};

class BravoAsset
{
private:

	struct BravoAssetManager
	{
		static BravoAssetManager& Instance()
		{
			static BravoAssetManager BAM;
			return BAM;
		}
		std::map<std::string, std::shared_ptr<BravoAsset>> LoadedAssets;
	};
	
public:

	template<typename ClassName>
	static std::shared_ptr<ClassName> Load(const std::string& Path, const std::vector<std::string>& InAdditionalParams = std::vector<std::string>())
	{
		ClassName* newAsset = new ClassName();
		if ( BravoAsset* asBravoAsset = dynamic_cast<BravoAsset*>(newAsset) )
		{
			asBravoAsset->SetPath(Path);
			asBravoAsset->AdditionalParams = InAdditionalParams;
			auto it = BravoAssetManager::Instance().LoadedAssets.find(newAsset->Path);
			if ( it != BravoAssetManager::Instance().LoadedAssets.end() )
			{
				delete newAsset;
				return std::dynamic_pointer_cast<ClassName>(it->second);
			}
			else
			{
				if ( asBravoAsset->Load_Internal() )
				{
					std::shared_ptr<ClassName> asShared;
					asShared = std::shared_ptr<ClassName>(newAsset);
					BravoAssetManager::Instance().LoadedAssets.insert(std::pair<std::string, std::shared_ptr<BravoAsset>>(asShared->Path, asShared));
					return asShared;
				}
			}
		}
		delete newAsset;
		return std::shared_ptr<ClassName>(nullptr);
	}

	void UnLoad()
	{
		UnLoad_Internal();
		auto it = BravoAssetManager::Instance().LoadedAssets.find(Path);
		if ( it != BravoAssetManager::Instance().LoadedAssets.end()  )
		{
			BravoAssetManager::Instance().LoadedAssets.erase(it);
		}
	}

	~BravoAsset()
	{
		
	}

	virtual void Use() = 0;
	virtual void StopUsage() = 0;

protected:
	virtual void SetPath(const std::string& InPath);
	virtual bool Load_Internal() = 0;
	virtual void UnLoad_Internal() = 0;

	inline static std::string GetRunningDir()
	{
		char buffer[MAX_PATH];
		GetModuleFileName( NULL, buffer, MAX_PATH );
		std::string::size_type pos = std::string( buffer ).find_last_of( "\\/" );
		return std::string( buffer ).substr( 0, pos) + "\\";
	}

	std::string Path;
	std::vector<std::string> AdditionalParams;
	unsigned int AssetHandle;
};