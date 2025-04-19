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
		BravoObject(std::forward<Args>(args)...),
		RootFolder(GetRootFolder()),
		ProjectName(GetExecutableName())
	{
		ResourcesFolders.reserve(2);
		ResourcesFolders.push_back(RootFolder + std::format(ProjectResourcesRelativePath, ProjectName));
		ResourcesFolders.push_back(RootFolder + EngineResourcesRelativePath);

		ShaderFolders.reserve(2);
		ShaderFolders.push_back(RootFolder + std::format(ProjectShadersRelativePath, ProjectName));
		ShaderFolders.push_back(RootFolder + EngineShadersRelativePath);

	}


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
			EAssetLoadingState LoadingState = asset->Load(std::forward<Args>(args)...);
			assert(LoadingState != EAssetLoadingState::Unloaded);

			if ( LoadingState == EAssetLoadingState::AsyncLoading )
			{
				PendingLoadingAssets.push_back(asset);
			}
			else if ( LoadingState != EAssetLoadingState::Unloaded )
			{
				if ( LoadingState != EAssetLoadingState::Loaded )
					asset->LoadToGPU();

				asset->OnAssetLoaded.Broadcast(asset);
			}
			else if ( LoadingState != EAssetLoadingState::Unloaded )
			{
				asset->Destroy();
				return nullptr;
			}
			return asset;
		}

		return nullptr;
	}

	std::string FindAsset(const std::string& AssetName);
	std::string FindShader(const std::string& ShaderName);
	
	void CheckPendingAssets();

private:

	static bool IfFileExist(const std::string& FileName);

	inline static std::string GetRootFolder()
	{
		char buffer[MAX_PATH];
		GetModuleFileName(NULL, buffer, MAX_PATH);
		std::string fullPath(buffer);

		// Find the base directory by removing up to 3 levels
		for (int i = 0; i < 4; ++i)
		{
			std::string::size_type pos = fullPath.find_last_of("\\/");
			if (pos == std::string::npos) break; // Exit if there are no more levels
			fullPath = fullPath.substr(0, pos);
		}

		return fullPath + "\\";
	}
	inline static std::string GetExecutableName()
	{
		char buffer[MAX_PATH];
		GetModuleFileName(NULL, buffer, MAX_PATH);
		std::string fullPath(buffer);

		// Find the position of the last path separator
		std::string::size_type lastSlashPos = fullPath.find_last_of("\\/");

		// Extract the file name (from the last path separator to the end)
		std::string fileName = fullPath.substr(lastSlashPos + 1);

		// Find the position of the last dot (file extension)
		std::string::size_type lastDotPos = fileName.find_last_of('.');

		// Remove the extension (if present)
		if (lastDotPos != std::string::npos)
		{
			fileName = fileName.substr(0, lastDotPos);
		}

		return fileName;
	}

private:
	std::vector<std::shared_ptr<class BravoAsset>> PendingLoadingAssets;
	std::map<std::string, std::shared_ptr<class BravoAsset>> LoadedAssets;

	const std::string RootFolder;
	const std::string ProjectName;
	const std::string EngineResourcesRelativePath = "Resources\\";
	const std::string EngineShadersRelativePath = "Bravo\\Source\\Shaders\\";
	static constexpr std::string_view ProjectResourcesRelativePath = "Games\\{}\\Resources\\";
	static constexpr std::string_view ProjectShadersRelativePath = "Games\\{}\\Source\\Shaders\\";

	std::vector<std::string> ResourcesFolders;
	std::vector<std::string> ShaderFolders;
};