#pragma once
#include "stdafx.h"
#include "openGL.h"
#include "BravoAsset.h"
#include "BravoTextureAsset.h"
#include "BravoCubemapAsset.h"

namespace ShaderProgrammConstancts
{
	const std::string VertexShaderExtension = ".vert";
	const std::string FragmentShaderExtension = ".frag";
	const std::string GeometryShaderExtension = ".geom";
	const std::string TessellationControllShaderExtension = ".tesc";
	const std::string TessellationEvaluationShaderExtension = ".tese";
	const std::string ComputeShaderExtension = ".comp";
}

struct BravoShaderLoadingParams
{
	BravoShaderLoadingParams(const std::string& _Path) :
		ShaderPath(_Path),
		ShaderDefines()
	{}
	BravoShaderLoadingParams(const std::string& _Path, const std::map<std::string, std::string>& _ShaderDefines) :
		ShaderPath(_Path),
		ShaderDefines(_ShaderDefines)
	{}

	std::string ShaderPath;
	std::map<std::string, std::string> ShaderDefines;
};

// TODO: refactor this asset. don't like the loading algo

class BravoShaderAsset : public BravoAsset
{
public:
	template <typename... Args>
	BravoShaderAsset(Args&&... args) :
		BravoAsset(std::forward<Args>(args)...)
	{}


	EAssetLoadingState Load(const std::string& ResourcesPath, const BravoShaderLoadingParams& params);
		
	virtual void Use() override;
	virtual void StopUsage() override;

	void SetTexture(const std::string& name, std::shared_ptr<BravoTextureAsset> val) const;
	void SetCubemap(const std::string& name, std::shared_ptr<BravoCubemapAsset> val) const;
	void SetBool(const std::string& name, const bool val) const;
	void SetInt(const std::string& name, const int32 val) const;
	void SetInt(const std::string& name, const size_t val) const;
	void SetInt(const std::string& name, const GLuint val) const;
	void SetFloat1(const std::string& name, const float val) const;
	void SetFloat2(const std::string& name, const glm::vec2& val) const;
	void SetFloat3(const std::string& name, const glm::vec3& val) const;
	void SetFloat4(const std::string& name, const glm::vec4& val) const;

	void SetFloat1v(const std::string& name, uint32 count, const float* val) const;
	void SetFloat2v(const std::string& name, uint32 count, const float* val) const;
	void SetFloat3v(const std::string& name, uint32 count, const float* val) const;
	void SetFloat4v(const std::string& name, uint32 count, const float* val) const;

	void SetMatrix2d(const std::string& name, const glm::mat2& val) const;
	void SetMatrix3d(const std::string& name, const glm::mat3& val) const;
	void SetMatrix4d(const std::string& name, const glm::mat4& val) const;

protected:
	bool LoadShader(GLenum ShaderType, int32& OutShader, const std::string& ResourcesPath, const BravoShaderLoadingParams& params);
	bool LinkProgramm();

private:

	GLint FindUniformLocation(const std::string& name) const;
	
	template<typename T>
	bool CheckUniformCache(const std::string& name, const T& val) const
	{
		auto valueIt = ValueCache.find(name);
		if (valueIt != ValueCache.end())
		{
			const T& cachedValue = std::any_cast<const T&>(valueIt->second);
            if (cachedValue == val)
			{
				return true;
			}
		}
		ValueCache[name] = val;
		return false;
	}

	std::shared_ptr<BravoTextureAsset> EmptyTexture = nullptr;
	GLuint ShaderID = 0;

	mutable std::unordered_map<std::string, GLint> LocationCache;
	mutable std::unordered_map<std::string, std::any> ValueCache;
};