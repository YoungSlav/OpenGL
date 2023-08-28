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

class BravoShaderAsset : public BravoAsset
{
public:
	bool Load(const std::string& ResourcesPath, const BravoShaderLoadingParams& params);

	struct Light
	{
		glm::vec3 position;

		glm::vec3 ambient;
		glm::vec3 diffuse;
		glm::vec3 specular;
	};


	
	virtual void Use() override;
	virtual void StopUsage() override;

	void SetTexture(const std::string& name, std::shared_ptr<BravoTextureAsset> val) const;
	void SetCubemap(const std::string& name, std::shared_ptr<BravoCubemapAsset> val) const;
	void SetBool(const std::string& name, const bool val) const;
	void SetInt(const std::string& name, const int32 val) const;
	void SetInt(const std::string& name, const size_t val) const;
	void SetVector1d(const std::string& name, const float val) const;
	void SetVector2d(const std::string& name, const glm::vec2& val) const;
	void SetVector3d(const std::string& name, const glm::vec3& val) const;
	void SetVector4d(const std::string& name, const glm::vec4& val) const;

	void SetMatrix2d(const std::string& name, const glm::mat2& val) const;
	void SetMatrix3d(const std::string& name, const glm::mat3& val) const;
	void SetMatrix4d(const std::string& name, const glm::mat4& val) const;

	void SetMaterial(const std::string& name, std::shared_ptr<class BravoMaterialAsset> val) const;

protected:
	bool LoadShader(GLenum ShaderType, int32& OutShader, const std::string& ResourcesPath, const BravoShaderLoadingParams& params);
	bool LinkProgramm();

private:

	std::shared_ptr<BravoTextureAsset> EmptyTexture = nullptr;

	GLuint ShaderID = 0;
};