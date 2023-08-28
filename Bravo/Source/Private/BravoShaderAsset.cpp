#include "BravoShaderAsset.h"
#include "BravoEngine.h"
#include "BravoAssetManager.h"
#include "BravoMaterialAsset.h"

#include <windows.h>
#include <fstream>
#include <sstream>
#include <regex>

bool BravoShaderAsset::Load(const std::string& ResourcesPath, const BravoShaderLoadingParams& params)
{
	ShaderID = glCreateProgram();
	int32 VertShader = 0;
	int32 GeomShader = 0;
	int32 FragShader = 0;
	

	EmptyTexture = Engine->GetAssetManager()->FindOrLoad<BravoTextureAsset>("BlackTextureAsset", BravoTextureLoadingParams("Textures\\black.png"));

	if ( LoadShader(GL_VERTEX_SHADER, VertShader, ResourcesPath, params) &&
		LoadShader(GL_FRAGMENT_SHADER, FragShader, ResourcesPath, params) &&
		LoadShader(GL_GEOMETRY_SHADER, GeomShader, ResourcesPath, params) &&
		LinkProgramm() )
	{
		if ( VertShader ) glDeleteShader(VertShader);
		if ( GeomShader ) glDeleteShader(GeomShader);
		if ( FragShader ) glDeleteShader(FragShader);

		return true;
	}
	return false;
}
void BravoShaderAsset::Use()
{
	glUseProgram(ShaderID);
}

void BravoShaderAsset::StopUsage()
{
	glUseProgram(0);
	if( EmptyTexture )
		EmptyTexture->StopUsage();
}

bool BravoShaderAsset::LoadShader(GLenum ShaderType, int32& OutShader, const std::string& ResourcesPath, const BravoShaderLoadingParams& params)
{
	OutShader = 0;
	std::string RealShaderName;

	if ( ShaderType == GL_VERTEX_SHADER )
	{
		RealShaderName = ResourcesPath + params.ShaderPath + ShaderProgrammConstancts::VertexShaderExtension;
	}
	else if ( ShaderType == GL_FRAGMENT_SHADER )
	{
		RealShaderName = ResourcesPath + params.ShaderPath + ShaderProgrammConstancts::FragmentShaderExtension;
	}
	else if ( ShaderType == GL_GEOMETRY_SHADER )
	{
		RealShaderName = ResourcesPath + params.ShaderPath + ShaderProgrammConstancts::GeometryShaderExtension;
	}
	
	
	std::ifstream shaderFile(RealShaderName.c_str());
	if ( !shaderFile.is_open() )
	{
		if ( ShaderType == GL_GEOMETRY_SHADER )
		{
			return true;
		}
		else
		{
			Log::LogMessage("Failed to load shader: " + RealShaderName + ", no such file", ELog::Error);
			return false;
		}
	}


	std::stringstream buffer;
	buffer << shaderFile.rdbuf();
	shaderFile.close();
	std::string ShaderSource = buffer.str();

	for ( auto& define : params.ShaderDefines )
	{
		size_t index = 0;
		while (true)
		{
			/* Locate the substring to replace. */
			index = ShaderSource.find(define.first, index);
			if (index == std::string::npos) break;

			/* Make the replacement. */
			ShaderSource.replace(index, define.first.length(), define.second);

			/* Advance index forward so the next iteration doesn't pick it up as well. */
			index += define.second.length();
		}
	}

	int32 Shader = glCreateShader(ShaderType);
	const int8 *c_str = ShaderSource.c_str();
	glShaderSource(Shader, 1, &c_str, NULL);
	glCompileShader(Shader);
	// check for shader compile errors
	int32 success;
	int8 infoLog[512];
	glGetShaderiv(Shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(Shader, 512, NULL, infoLog);
		Log::LogMessage("Failed to compile shader: " + RealShaderName, ELog::Error);
		Log::LogMessage(infoLog, ELog::Error);
		glDeleteShader(Shader);
		return false;
	}
		
	glAttachShader(ShaderID, Shader);
	OutShader = Shader;

	return true;
}

bool BravoShaderAsset::LinkProgramm()
{
	int32 success;
	int8 infoLog[512];
	glLinkProgram(ShaderID);
    // check for linking errors
    glGetProgramiv(ShaderID, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(ShaderID, 512, NULL, infoLog);
		Log::LogMessage("PROGRAM::LINKING_FAILED", ELog::Error);
		Log::LogMessage(infoLog, ELog::Error);
		return false;
    }
	return true;
}


void BravoShaderAsset::SetTexture(const std::string& name, std::shared_ptr<BravoTextureAsset> val) const
{
	if ( val )
	{
		val->Use();
		glUniform1i(glGetUniformLocation(ShaderID, name.c_str()), val->GetTextureUnit());
	}
	else
	{
		Log::LogMessage("BravoShader::SetTexture() value is none!");
	}
}

void BravoShaderAsset::SetCubemap(const std::string& name, std::shared_ptr<BravoCubemapAsset> val) const
{
	if ( val )
	{
		val->Use();
		glUniform1i(glGetUniformLocation(ShaderID, name.c_str()), val->GetTextureUnit());
	}
	else
	{
		Log::LogMessage("BravoShader::SetCubemap() value is none!");
	}
}

void BravoShaderAsset::SetBool(const std::string& name, const bool val) const
{
	glUniform1i(glGetUniformLocation(ShaderID, name.c_str()), (int32)val); 
}

void BravoShaderAsset::SetInt(const std::string& name, const size_t val) const
{
	glUniform1i(glGetUniformLocation(ShaderID, name.c_str()), (int32)val);
}

void BravoShaderAsset::SetInt(const std::string& name, const int32 val) const
{
	glUniform1i(glGetUniformLocation(ShaderID, name.c_str()), val);
}

void BravoShaderAsset::SetVector1d(const std::string& name, const float val) const
{
	glUniform1f(glGetUniformLocation(ShaderID, name.c_str()), val);
}

void BravoShaderAsset::SetVector2d(const std::string& name, const glm::vec2& val) const
{
	glUniform2f(glGetUniformLocation(ShaderID, name.c_str()), val.x, val.y);
}

void BravoShaderAsset::SetVector3d(const std::string& name, const glm::vec3& val) const
{
	glUniform3f(glGetUniformLocation(ShaderID, name.c_str()), val.x, val.y, val.z);
}

void BravoShaderAsset::SetVector4d(const std::string& name, const glm::vec4& val) const
{
	glUniform4f(glGetUniformLocation(ShaderID, name.c_str()), val.x, val.y, val.z, val.w);
}

void BravoShaderAsset::SetMatrix2d(const std::string& name, const glm::mat2& val) const
{
	glUniformMatrix2fv(glGetUniformLocation(ShaderID, name.c_str()), 1, GL_FALSE, glm::value_ptr(val));
}

void BravoShaderAsset::SetMatrix3d(const std::string& name, const glm::mat3& val) const
{
	glUniformMatrix3fv(glGetUniformLocation(ShaderID, name.c_str()), 1, GL_FALSE, glm::value_ptr(val));
}

void BravoShaderAsset::SetMatrix4d(const std::string& name, const glm::mat4& val) const
{
	glUniformMatrix4fv(glGetUniformLocation(ShaderID, name.c_str()), 1, GL_FALSE, glm::value_ptr(val));
}

void BravoShaderAsset::SetMaterial(const std::string& name, std::shared_ptr<class BravoMaterialAsset> val) const
{
	if ( !val )
	{
		Log::LogMessage("BravoShader::SetMaterial() value is none!");
		return;
	}
	
	val->Use();

	BravoMaterialShaderData data = val->GetShadeData();

	SetInt(name + ".textureArray", val->GetTextureUnit());
	SetInt(name + ".diffuseTexture", data.DiffuseTextureLayer);
	SetInt(name + ".specularTexture", data.SpecularTextureLayer);
	SetInt(name + ".normalTexture", data.NormalTextureLayer);
	SetVector3d(name + ".diffuseColor", data.DiffuseColor);
	SetVector3d(name + ".specularColor", data.SpecularColor);
	SetVector1d(name + ".shininess", data.Shininess);
}
