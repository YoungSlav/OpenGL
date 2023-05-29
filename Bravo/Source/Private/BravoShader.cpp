#include "BravoShader.h"
#include "BravoAssetManager.h"

#include <windows.h>
#include <fstream>
#include <sstream>


bool BravoShader::Initialize_Internal(const std::vector<std::string>& _Params)
{
	ShaderID = glCreateProgram();
	int32 VertShader = 0;
	int32 GeomShader = 0;
	int32 FragShader = 0;

	EmptyTexture = GetAssetManager()->LoadAsset<BravoTexture>("Textures\\black.png");

	if ( LoadShader(GL_VERTEX_SHADER, VertShader) &&
		LoadShader(GL_FRAGMENT_SHADER, FragShader) &&
		LoadShader(GL_GEOMETRY_SHADER, GeomShader) &&
		LinkProgramm() )
	{
		if ( VertShader ) glDeleteShader(VertShader);
		if ( GeomShader ) glDeleteShader(GeomShader);
		if ( FragShader ) glDeleteShader(FragShader);

		return true;
	}
	return false;
}

BravoShader::~BravoShader()
{
	glUseProgram(0);
	glDeleteProgram(ShaderID);
}

void BravoShader::Use()
{
	glUseProgram(ShaderID);
}

void BravoShader::StopUsage()
{
	glUseProgram(0);
	if( EmptyTexture )
		EmptyTexture->StopUsage();
}

bool BravoShader::LoadShader(GLenum ShaderType, int32& OutShader)
{
	OutShader = 0;
	std::string RealShaderName;

	if ( ShaderType == GL_VERTEX_SHADER )
	{
		RealShaderName = Path + ShaderProgrammConstancts::VertexShaderExtension;
	}
	else if ( ShaderType == GL_FRAGMENT_SHADER )
	{
		RealShaderName = Path + ShaderProgrammConstancts::FragmentShaderExtension;
	}
	else if ( ShaderType == GL_GEOMETRY_SHADER )
	{
		RealShaderName = Path + ShaderProgrammConstancts::GeometryShaderExtension;
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

bool BravoShader::LinkProgramm()
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


void BravoShader::SetTexture(const std::string& name, BravoTexturePtr val) const
{
	if ( val )
	{
		val->Use();
		glUniform1i(glGetUniformLocation(ShaderID, name.c_str()), val->GetTextureUnit());
	}
	else
	{
		Log::LogMessage("BravoShader::SetMaterial() value is none!");
	}
}

void BravoShader::SetCubemap(const std::string& name, BravoCubemapPtr val) const
{
	if ( val )
	{
		val->Use();
		glUniform1i(glGetUniformLocation(ShaderID, name.c_str()), val->GetTextureUnit());
	}
	else
	{
		Log::LogMessage("BravoShader::SetMaterial() value is none!");
	}
}

void BravoShader::SetBool(const std::string& name, const bool val) const
{
	glUniform1i(glGetUniformLocation(ShaderID, name.c_str()), (int32)val); 
}

void BravoShader::SetInt(const std::string& name, const size_t val) const
{
	glUniform1i(glGetUniformLocation(ShaderID, name.c_str()), (int32)val);
}

void BravoShader::SetInt(const std::string& name, const int32 val) const
{
	glUniform1i(glGetUniformLocation(ShaderID, name.c_str()), val);
}

void BravoShader::SetVector1d(const std::string& name, const float val) const
{
	glUniform1f(glGetUniformLocation(ShaderID, name.c_str()), val);
}

void BravoShader::SetVector2d(const std::string& name, const glm::vec2& val) const
{
	glUniform2f(glGetUniformLocation(ShaderID, name.c_str()), val.x, val.y);
}

void BravoShader::SetVector3d(const std::string& name, const glm::vec3& val) const
{
	glUniform3f(glGetUniformLocation(ShaderID, name.c_str()), val.x, val.y, val.z);
}

void BravoShader::SetVector4d(const std::string& name, const glm::vec4& val) const
{
	glUniform4f(glGetUniformLocation(ShaderID, name.c_str()), val.x, val.y, val.z, val.w);
}

void BravoShader::SetMatrix2d(const std::string& name, const glm::mat2& val) const
{
	glUniformMatrix2fv(glGetUniformLocation(ShaderID, name.c_str()), 1, GL_FALSE, glm::value_ptr(val));
}

void BravoShader::SetMatrix3d(const std::string& name, const glm::mat3& val) const
{
	glUniformMatrix3fv(glGetUniformLocation(ShaderID, name.c_str()), 1, GL_FALSE, glm::value_ptr(val));
}

void BravoShader::SetMatrix4d(const std::string& name, const glm::mat4& val) const
{
	glUniformMatrix4fv(glGetUniformLocation(ShaderID, name.c_str()), 1, GL_FALSE, glm::value_ptr(val));
}

void BravoShader::SetMaterial(const std::string& name, const BravoMaterialPtr& val) const
{
	if ( !val )
	{
		Log::LogMessage("BravoShader::SetMaterial() value is none!");
		return;
	}


	for ( int32 i = 0; i < EBravoTextureType::NUM; ++i )
	{
		EBravoTextureType type = (EBravoTextureType)i;
		std::string textureName;
		switch(type)
		{
		case EBravoTextureType::diffuse:
			textureName = "diffuse";
			break;
		case EBravoTextureType::specular:
			textureName = "specular";
			break;
		case EBravoTextureType::height:
			textureName = "height";
			break;
		case EBravoTextureType::ambient:
			textureName = "ambient";
			break;
		case EBravoTextureType::normal:
			textureName = "normal";
			break;
		}
		if ( val->Textures[i] )
			SetTexture(name + "." + textureName, val->Textures[i]);
		else
			SetTexture(name + "." + textureName, EmptyTexture);
	}
	SetVector1d(name + ".shininess", val->Shininess);

}
