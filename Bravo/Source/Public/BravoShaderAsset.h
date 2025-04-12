#pragma once
#include "stdafx.h"
#include "openGL.h"
#include "BravoAsset.h"
#include "BravoTextureAsset.h"
#include "BravoCubemapAsset.h"

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

	void SetFloat1v(const std::string& name, uint32 count, const float* val) const;
	void SetFloat2v(const std::string& name, uint32 count, const float* val) const;
	void SetFloat3v(const std::string& name, uint32 count, const float* val) const;
	void SetFloat4v(const std::string& name, uint32 count, const float* val) const;

	void SetMatrix2d(const std::string& name, const glm::mat2& val) const;
	void SetMatrix3d(const std::string& name, const glm::mat3& val) const;
	void SetMatrix4d(const std::string& name, const glm::mat4& val) const;

protected:

	virtual void ReleaseFromGPU_Internal() override;

	bool LoadShader(GLenum ShaderType, GLuint& OutShader, const std::string& Path, const std::map<std::string, std::string>& ShaderDefines);
	bool LinkProgramm();


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
	GLuint ProgramID = 0;

	mutable std::unordered_map<std::string, GLint> LocationCache;
	mutable std::unordered_map<std::string, std::any> ValueCache;
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