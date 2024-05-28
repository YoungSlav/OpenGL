#include "BravoShaderAsset.h"
#include "BravoEngine.h"
#include "BravoAssetManager.h"
#include "BravoMaterial.h"

#include <windows.h>
#include <fstream>
#include <sstream>
#include <regex>

EAssetLoadingState BravoShaderAsset::Load(const std::string& ResourcesPath, const BravoShaderLoadingParams& params)
{
	ShaderID = glCreateProgram();
	int32 VertShader = 0;
	int32 GeomShader = 0;
	int32 FragShader = 0;
	int32 TessContrShader = 0;
	int32 TessEvalShader = 0;
	

	EmptyTexture = Engine->GetAssetManager()->FindOrLoad<BravoTextureAsset>("BlackTextureAsset", BravoTextureLoadingParams("Textures\\black.png"));

	if ( LoadShader(GL_VERTEX_SHADER, VertShader, ResourcesPath, params) &&
		LoadShader(GL_FRAGMENT_SHADER, FragShader, ResourcesPath, params) &&
		LoadShader(GL_GEOMETRY_SHADER, GeomShader, ResourcesPath, params) &&
		LoadShader(GL_TESS_CONTROL_SHADER, TessContrShader, ResourcesPath, params) &&
		LoadShader(GL_TESS_EVALUATION_SHADER, TessEvalShader, ResourcesPath, params) &&
		LinkProgramm() )
	{
		if ( VertShader ) glDeleteShader(VertShader);
		if ( GeomShader ) glDeleteShader(GeomShader);
		if ( FragShader ) glDeleteShader(FragShader);
		if ( TessContrShader ) glDeleteShader(TessContrShader);
		if ( TessEvalShader ) glDeleteShader(TessEvalShader);

		LoadingState = EAssetLoadingState::Loaded;
		return LoadingState;
	}
	LoadingState = EAssetLoadingState::Failed;
	return LoadingState;
}
void BravoShaderAsset::Use()
{
	static GLuint CurentShader = 0;
	if ( CurentShader != ShaderID )
	{
		CurentShader = ShaderID;
		glUseProgram(ShaderID);
	}
}

void BravoShaderAsset::StopUsage()
{
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
	else if ( ShaderType == GL_TESS_CONTROL_SHADER )
	{
		RealShaderName = ResourcesPath + params.ShaderPath + ShaderProgrammConstancts::TessellationControllShaderExtension;
	}
	else if ( ShaderType == GL_TESS_EVALUATION_SHADER )
	{
		RealShaderName = ResourcesPath + params.ShaderPath + ShaderProgrammConstancts::TessellationEvaluationShaderExtension;
	}
	
	
	std::ifstream shaderFile(RealShaderName.c_str());
	if ( !shaderFile.is_open() )
	{
		if ( ShaderType == GL_GEOMETRY_SHADER || ShaderType == GL_TESS_CONTROL_SHADER || ShaderType == GL_TESS_EVALUATION_SHADER)
		{
			return true;
		}
		else
		{
			Log::LogMessage(ELog::Error, "Failed to load shader: {}, no such file", RealShaderName);
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
		Log::LogMessage(ELog::Error, "Failed to compile shader: {}", RealShaderName);
		Log::LogMessage(ELog::Error, infoLog );
		glDeleteShader(Shader);
		return false;
	}

	Log::LogMessage(ELog::Log, "Loaded shader: {}", RealShaderName);
	
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
		Log::LogMessage(ELog::Error, "PROGRAM::LINKING_FAILED");
		Log::LogMessage(ELog::Error, infoLog );
		return false;
    }
	return true;
}


void BravoShaderAsset::SetTexture(const std::string& name, std::shared_ptr<BravoTextureAsset> val) const
{
	if ( val->EnsureGPUReady() )
	{
		val->Use();
		SetInt(name, val->GetTextureUnit());
	}
	else if ( EmptyTexture->EnsureGPUReady() )
	{
		EmptyTexture->Use();
		SetInt(name, EmptyTexture->GetTextureUnit());
	}
}

void BravoShaderAsset::SetCubemap(const std::string& name, std::shared_ptr<BravoCubemapAsset> val) const
{
	if ( val )
	{
		val->Use();
		SetInt(name, val->GetTextureUnit());
	}
	else
	{
		Log::LogMessage(ELog::Warning, "BravoShader::SetCubemap() value is none!");
	}
}

void BravoShaderAsset::SetBool(const std::string& name, const bool val) const
{
	SetInt(name, GLuint(val ? 1 : 0));
}

void BravoShaderAsset::SetInt(const std::string& name, const size_t val) const
{
	SetInt(name, (GLuint)val);
}

void BravoShaderAsset::SetInt(const std::string& name, const int32 val) const
{
	SetInt(name, (GLuint)val);
}

void BravoShaderAsset::SetInt(const std::string& name, const GLuint val) const
{
	if ( CheckUniformCache(name, val) ) return;
	glUniform1i(FindUniformLocation(name.c_str()), val);
}

void BravoShaderAsset::SetVector1d(const std::string& name, const float val) const
{
	if ( CheckUniformCache(name, val) ) return;
	glUniform1f(FindUniformLocation(name.c_str()), val);
}

void BravoShaderAsset::SetVector2d(const std::string& name, const glm::vec2& val) const
{
	if ( CheckUniformCache(name, val) ) return;
	glUniform2f(FindUniformLocation(name.c_str()), val.x, val.y);
}

void BravoShaderAsset::SetVector3d(const std::string& name, const glm::vec3& val) const
{
	if ( CheckUniformCache(name, val) ) return;
	glUniform3f(FindUniformLocation(name.c_str()), val.x, val.y, val.z);
}

void BravoShaderAsset::SetVector4d(const std::string& name, const glm::vec4& val) const
{
	if ( CheckUniformCache(name, val) ) return;
	glUniform4f(FindUniformLocation(name.c_str()), val.x, val.y, val.z, val.w);
}

void BravoShaderAsset::SetMatrix2d(const std::string& name, const glm::mat2& val) const
{
	if ( CheckUniformCache(name, val) ) return;
	glUniformMatrix2fv(FindUniformLocation(name.c_str()), 1, GL_FALSE, glm::value_ptr(val));
}

void BravoShaderAsset::SetMatrix3d(const std::string& name, const glm::mat3& val) const
{
	if ( CheckUniformCache(name, val) ) return;
	glUniformMatrix3fv(FindUniformLocation(name.c_str()), 1, GL_FALSE, glm::value_ptr(val));
}

void BravoShaderAsset::SetMatrix4d(const std::string& name, const glm::mat4& val) const
{
	if ( CheckUniformCache(name, val) ) return;
	glUniformMatrix4fv(FindUniformLocation(name.c_str()), 1, GL_FALSE, glm::value_ptr(val));
}

GLint BravoShaderAsset::FindUniformLocation(const std::string& name) const
{
	auto locationIt = LocationCache.find(name);
	GLint location = -1;
	if (locationIt == LocationCache.end())
	{
		location = glGetUniformLocation(ShaderID, name.c_str());
		if (location == -1)
		{
			Log::LogMessage(ELog::Warning, "Could not find param {} in {} shader", name, GetName());
			return location;
		}
		LocationCache[name] = location;
	}
	else
	{
		location = locationIt->second;
	}
	return location;
}