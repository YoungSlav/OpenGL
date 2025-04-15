#pragma once
#include "stdafx.h"
#include "openGL.h"
#include "BravoAsset.h"
#include "BravoTextureAsset.h"
#include "BravoCubemapAsset.h"

#define SHADER_HOTSWAP 1
#if SHADER_HOTSWAP
#include <filesystem>
#include <chrono>
#endif

namespace ShaderProgrammConstancts
{
	const std::map<GLenum, std::string> Extension = {
		{GL_VERTEX_SHADER, ".vert"},
		{GL_FRAGMENT_SHADER, ".frag"},
		{GL_GEOMETRY_SHADER, ".geom"},
		{GL_TESS_CONTROL_SHADER, ".tesc"},
		{GL_TESS_EVALUATION_SHADER, ".tese"},
		{GL_COMPUTE_SHADER, ".comp"}
	};
};

struct BravoRenderShaderLoadingParams
{
	BravoRenderShaderLoadingParams(const std::string& _Path, bool _bGeometry, bool _bTessellation) :
		ShaderPath(_Path),
		ShaderDefines(),
		bGeometry(_bGeometry),
		bTessellation(_bTessellation)
	{}
	BravoRenderShaderLoadingParams(const std::string& _Path, bool _bGeometry, bool _bTessellation, const std::map<std::string, std::string>& _ShaderDefines) :
		ShaderPath(_Path),
		ShaderDefines(_ShaderDefines),
		bGeometry(_bGeometry),
		bTessellation(_bTessellation)
	{}

	std::string ShaderPath;
	std::map<std::string, std::string> ShaderDefines;
	bool bGeometry = false;
	bool bTessellation = false;
};

struct BravoComputeShaderLoadingParams
{
	BravoComputeShaderLoadingParams(const std::string& _Path) :
		ShaderPath(_Path),
		ShaderDefines()
	{}
	BravoComputeShaderLoadingParams(const std::string& _Path, const std::map<std::string, std::string>& _ShaderDefines) :
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

	void SetFloat1v(const std::string& name, const std::vector<float>& val) const;
	void SetFloat2v(const std::string& name, const std::vector<glm::vec2>& val) const;
	void SetFloat3v(const std::string& name, const std::vector<glm::vec3>& val) const;
	void SetFloat4v(const std::string& name, const std::vector<glm::vec4>& val) const;

	void SetMatrix2d(const std::string& name, const glm::mat2& val) const;
	void SetMatrix3d(const std::string& name, const glm::mat3& val) const;
	void SetMatrix4d(const std::string& name, const glm::mat4& val) const;

protected:

	virtual void OnDestroy() override;
	virtual void ReleaseFromGPU_Internal() override;

	bool LoadShader(GLenum ShaderType, GLuint& OutShader, const std::string& Path, const std::map<std::string, std::string>& ShaderDefines, std::string& OutFullPath);
	bool LinkProgramm();


	GLint FindUniformLocation(const std::string& name) const;
	
	using UniformValue = std::variant<
		bool, int32, size_t, GLuint, float,
		glm::vec2, glm::vec3, glm::vec4,
		glm::mat2, glm::mat3, glm::mat4,
		std::shared_ptr<BravoTextureAsset>,
		std::shared_ptr<BravoCubemapAsset>,
		std::vector<float>,
		std::vector<glm::vec2>,
		std::vector<glm::vec3>,
		std::vector<glm::vec4>
		>;
	
	template<typename T>
	bool CheckUniformCache(const std::string& name, const T& val) const
	{
		auto it = ValueCache.find(name);
		if (it != ValueCache.end())
		{
			if (std::holds_alternative<T>(it->second))
			{
				if (std::get<T>(it->second) == val)
					return true;
			}
		}
		ValueCache[name] = val;
		return false;
	}

	std::shared_ptr<BravoTextureAsset> EmptyTexture = nullptr;
	GLuint ProgramID = 0;

	mutable std::unordered_map<std::string, GLint> LocationCache;
	mutable std::unordered_map<std::string, UniformValue> ValueCache;

	

#if SHADER_HOTSWAP
	struct ShaderHotswapInfo
	{
		ShaderHotswapInfo( GLenum _ShaderType, GLuint _Shader, const std::string& _FullPath) :
			ShaderType(_ShaderType),
			Shader(_Shader),
			FullPath(_FullPath)
		{
			LastModificationTime = std::filesystem::last_write_time(FullPath);
		}

		GLenum ShaderType;
		GLuint Shader;
		std::filesystem::path FullPath;
		std::filesystem::file_time_type LastModificationTime;
	};

	std::vector<ShaderHotswapInfo> ShaderHotswapInfos;

	std::string ShaderPath;
	std::map<std::string, std::string> ShaderDefines;

public:
	void CheckShadersForHotSwap();
private:
	void ReapplyCachedUniforms();
protected:
	template<typename T>
	void SetValue(const std::string& name, const T& val) const
	{
		if constexpr (std::is_same_v<T, bool>)
			SetBool(name, val);
		else if constexpr (std::is_same_v<T, int32>)
			SetInt(name, val);
		else if constexpr (std::is_same_v<T, size_t>)
			SetInt(name, val);
		else if constexpr (std::is_same_v<T, GLuint>)
			SetInt(name, val);
		else if constexpr (std::is_same_v<T, float>)
			SetFloat1(name, val);
		else if constexpr (std::is_same_v<T, glm::vec2>)
			SetFloat2(name, val);
		else if constexpr (std::is_same_v<T, glm::vec3>)
			SetFloat3(name, val);
		else if constexpr (std::is_same_v<T, glm::vec4>)
			SetFloat4(name, val);
		else if constexpr (std::is_same_v<T, glm::mat2>)
			SetMatrix2d(name, val);
		else if constexpr (std::is_same_v<T, glm::mat3>)
			SetMatrix3d(name, val);
		else if constexpr (std::is_same_v<T, glm::mat4>)
			SetMatrix4d(name, val);
		else if constexpr (std::is_same_v<T, std::shared_ptr<BravoTextureAsset>>)
			SetTexture(name, val);
		else if constexpr (std::is_same_v<T, std::shared_ptr<BravoCubemapAsset>>)
			SetCubemap(name, val);
		else if constexpr (std::is_same_v<T, std::vector<float>>)
			SetFloat1v(name, val);
		else if constexpr (std::is_same_v<T, std::vector<glm::vec2>>)
			SetFloat2v(name, val);
		else if constexpr (std::is_same_v<T, std::vector<glm::vec3>>)
			SetFloat3v(name, val);
		else if constexpr (std::is_same_v<T, std::vector<glm::vec4>>)
			SetFloat4v(name, val);
		else
			Log::LogMessage(ELog::Error, "Unsupported variant type while re-applying cached values in shader");
	}
#endif

};

class BravoRenderShaderAsset : public BravoShaderAsset
{
public:
	template <typename... Args>
	BravoRenderShaderAsset(Args&&... args) :
		BravoShaderAsset(std::forward<Args>(args)...)
	{}

	EAssetLoadingState Load(const BravoRenderShaderLoadingParams& params);

protected:
	virtual void ReleaseFromGPU_Internal() override;

private:
	GLuint VertexShader = 0;
	GLuint FragmentShader = 0;
	GLuint GeometryShader = 0;
	GLuint TessellationControllShader = 0;
	GLuint TessellationEvaluationShader = 0;
};



class BravoComputeShaderAsset : public BravoShaderAsset
{
public:
	template <typename... Args>
	BravoComputeShaderAsset(Args&&... args) :
		BravoShaderAsset(std::forward<Args>(args)...)
	{}

	EAssetLoadingState Load(const BravoComputeShaderLoadingParams& params);

protected:
	virtual void ReleaseFromGPU_Internal() override;

private:
	GLuint ComputeShader = 0;
};