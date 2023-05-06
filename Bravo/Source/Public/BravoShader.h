#pragma once
#include "stdafx.h"
#include "openGL.h"
#include "BravoAsset.h"
#include "BravoTexture.h"
#include "BravoCubemap.h"

namespace ShaderProgrammConstancts
{
	const std::string VertexShaderExtension = ".vert";
	const std::string FragmentShaderExtension = ".frag";
	const std::string GeometryShaderExtension = ".geom";
}


enum EBravoTextureType : int
{
	diffuse,
	specular,
	height,
	ambient,
	normal,

	NUM
};

struct BravoMaterial
{
	void Use()
	{
		for ( int i = 0; i < EBravoTextureType::NUM; ++i )
		{
			if ( Textures[i] )
				Textures[i]->Use();
		}
	}

	void StopUsage()
	{
		for ( int i = 0; i < EBravoTextureType::NUM; ++i )
		{
			if ( Textures[i] )
				Textures[i]->StopUsage();
		}
	}

	BravoTexturePtr Textures[NUM];
	
	float Shininess;
};

typedef std::shared_ptr<BravoMaterial> BravoMaterialPtr;


class BravoShader : public BravoAsset
{
public:
	BravoShader(std::shared_ptr<class BravoAssetManager> _AssetManager) : 
		BravoAsset(_AssetManager)
	{}
	~BravoShader();

	struct Light
	{
		glm::vec3 position;

		glm::vec3 ambient;
		glm::vec3 diffuse;
		glm::vec3 specular;
	};


	
	virtual void Use() override;
	virtual void StopUsage() override;

	void SetTexture(const std::string& name, BravoTexturePtr val) const;
	void SetCubemap(const std::string& name, BravoCubemapPtr val) const;
	void SetBool(const std::string& name, const bool val) const;
	void SetInt(const std::string& name, const int val) const;
	void SetVector1d(const std::string& name, const float val) const;
	void SetVector2d(const std::string& name, const glm::vec2& val) const;
	void SetVector3d(const std::string& name, const glm::vec3& val) const;
	void SetVector4d(const std::string& name, const glm::vec4& val) const;

	void SetMatrix2d(const std::string& name, const glm::mat2& val) const;
	void SetMatrix3d(const std::string& name, const glm::mat3& val) const;
	void SetMatrix4d(const std::string& name, const glm::mat4& val) const;

	void SetMaterial(const std::string& name, const BravoMaterialPtr& val) const;

protected:
	virtual bool Initialize_Internal(const std::vector<std::string>& _Params = std::vector<std::string>()) override;

	bool LoadShader(GLenum ShaderType, int& OutShader);
	bool LinkProgramm();

private:

	BravoTexturePtr EmptyTexture = nullptr;

	unsigned int ShaderID = 0;

};

typedef std::shared_ptr<BravoShader> BravoShaderPtr;