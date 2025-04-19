#include "BravoShaderAsset.h"
#include "BravoEngine.h"
#include "BravoAssetManager.h"
#include "BravoMaterial.h"
#include "BravoTimeManager.h"

#include <windows.h>
#include <fstream>
#include <sstream>
#include <regex>

EAssetLoadingState BravoRenderShaderAsset::Load(const BravoRenderShaderSettings& params)
{
	GLuint VertexShader = 0;
	GLuint FragmentShader = 0;
	GLuint GeometryShader = 0;
	GLuint TessellationControllShader = 0;
	GLuint TessellationEvaluationShader = 0;

	ProgramID = glCreateProgram();

	bool bSuccess = true;
	std::string OutShaderPath = "";

#if SHADER_HOTSWAP
	ShaderPath = params.ShaderPath;
	ShaderDefines = params.ShaderDefines;
#endif

	if ( LoadShader(GL_VERTEX_SHADER, VertexShader, params.ShaderPath, params.ShaderDefines, OutShaderPath) )
	{
#if SHADER_HOTSWAP
		ShaderHotswapInfos.push_back(ShaderHotswapInfo(GL_VERTEX_SHADER, VertexShader, OutShaderPath));
#endif
	}
	else
	{
		bSuccess = false;
		Log::LogMessage(ELog::Error, "Failed to load VERTEX shader: {}", params.ShaderPath);
	}
	glAttachShader(ProgramID, VertexShader);

	if ( LoadShader(GL_FRAGMENT_SHADER, FragmentShader, params.ShaderPath, params.ShaderDefines, OutShaderPath) )
	{
#if SHADER_HOTSWAP
		ShaderHotswapInfos.push_back(ShaderHotswapInfo(GL_FRAGMENT_SHADER, FragmentShader, OutShaderPath));
#endif
	}
	else
	{
		bSuccess = false;
		Log::LogMessage(ELog::Error, "Failed to load FRAGMENT shader: {}", params.ShaderPath);
	}
	glAttachShader(ProgramID, FragmentShader);
	if ( params.bGeometry )
	{
		if ( LoadShader(GL_GEOMETRY_SHADER, GeometryShader, params.ShaderPath, params.ShaderDefines, OutShaderPath) )
		{
#if SHADER_HOTSWAP
			ShaderHotswapInfos.push_back(ShaderHotswapInfo(GL_GEOMETRY_SHADER, GeometryShader, OutShaderPath));
#endif
		}
		else
		{
			bSuccess = false;
			Log::LogMessage(ELog::Error, "Failed to load GEOMETRY shader: {}", params.ShaderPath);
		}
		glAttachShader(ProgramID, GeometryShader);
	}

	if ( params.bTessellation )
	{
		if ( LoadShader(GL_TESS_CONTROL_SHADER, TessellationControllShader, params.ShaderPath, params.ShaderDefines, OutShaderPath) )
		{
#if SHADER_HOTSWAP
			ShaderHotswapInfos.push_back(ShaderHotswapInfo(GL_TESS_CONTROL_SHADER, TessellationControllShader, OutShaderPath));
#endif
		}
		else
		{
			bSuccess = false;
			Log::LogMessage(ELog::Error, "Failed to load TESS_CONTROL shader: {}", params.ShaderPath);
		}
		glAttachShader(ProgramID, TessellationControllShader);

		if ( LoadShader(GL_TESS_EVALUATION_SHADER, TessellationEvaluationShader, params.ShaderPath, params.ShaderDefines, OutShaderPath) )
		{
#if SHADER_HOTSWAP
			ShaderHotswapInfos.push_back(ShaderHotswapInfo(GL_TESS_EVALUATION_SHADER, TessellationEvaluationShader, OutShaderPath));
#endif
		}
		else
		{
			bSuccess = false;
			Log::LogMessage(ELog::Error, "Failed to load TESS_EVALUATION shader: {}", params.ShaderPath);
		}
		glAttachShader(ProgramID, TessellationEvaluationShader);
	}
	if ( !LinkProgramm() )
	{
		bSuccess = false;
		Log::LogMessage(ELog::Error, "Failed to link the program: {}", params.ShaderPath);	
	}
	
	if ( !bSuccess )
	{
		if ( VertexShader ) glDeleteShader(VertexShader);
		if ( FragmentShader ) glDeleteShader(FragmentShader);
		if ( GeometryShader ) glDeleteShader(GeometryShader);
		if ( TessellationControllShader ) glDeleteShader(TessellationControllShader);
		if ( TessellationEvaluationShader ) glDeleteShader(TessellationEvaluationShader);

		glDeleteProgram(ProgramID);
		LoadingState = EAssetLoadingState::Unloaded;
		return LoadingState;
	}

#if SHADER_HOTSWAP
	std::weak_ptr<BravoShaderAsset> WeakThis(Self<BravoShaderAsset>());
	Engine->GetTimeManager()->SetTimer(1.0, true,
		[WeakThis]()
		{
			if ( !WeakThis.expired() )
			{
				WeakThis.lock()->CheckShadersForHotSwap();
			}
		});
#else
	if ( VertexShader ) glDeleteShader(VertexShader);
	if ( FragmentShader ) glDeleteShader(FragmentShader);
	if ( GeometryShader ) glDeleteShader(GeometryShader);
	if ( TessellationControllShader ) glDeleteShader(TessellationControllShader);
	if ( TessellationEvaluationShader ) glDeleteShader(TessellationEvaluationShader);
#endif

	EmptyTexture = Engine->GetAssetManager()->FindOrLoad<BravoTextureAsset>("BlackTextureAsset", BravoTextureSettings("Textures\\black.png"));

	LoadingState = EAssetLoadingState::Loaded;
	return LoadingState;
}

EAssetLoadingState BravoComputeShaderAsset::Load(const BravoComputeShaderSettings& params)
{
	GLuint ComputeShader = 0;
	ProgramID = glCreateProgram();
	bool bSuccess = true;
	std::string OutShaderPath;

#if SHADER_HOTSWAP
	ShaderPath = params.ShaderPath;
	ShaderDefines = params.ShaderDefines;
#endif

	if ( LoadShader(GL_COMPUTE_SHADER, ComputeShader, params.ShaderPath, params.ShaderDefines, OutShaderPath) )
	{
#if SHADER_HOTSWAP
		ShaderHotswapInfos.push_back(ShaderHotswapInfo(GL_COMPUTE_SHADER, ComputeShader, OutShaderPath));
#endif
	}
	else
	{
		bSuccess = false;
		Log::LogMessage(ELog::Error, "Failed to load COMPUTE shader: {}", params.ShaderPath);
	}
	glAttachShader(ProgramID, ComputeShader);

	if ( !LinkProgramm() )
	{
		bSuccess = false;
		Log::LogMessage(ELog::Error, "Failed to link the program: {}", params.ShaderPath);
	}

	if ( !bSuccess )
	{
		if ( ComputeShader ) glDeleteShader(ComputeShader);
		glDeleteProgram(ProgramID);
		LoadingState = EAssetLoadingState::Unloaded;
		return LoadingState;
	}

#if SHADER_HOTSWAP
	std::weak_ptr<BravoShaderAsset> WeakThis(Self<BravoShaderAsset>());
	Engine->GetTimeManager()->SetTimer(1.0, true,
		[WeakThis]()
		{
			if ( !WeakThis.expired() )
			{
				WeakThis.lock()->CheckShadersForHotSwap();
			}
		});
#else
	if ( ComputeShader ) glDeleteShader(ComputeShader);
#endif

	EmptyTexture = Engine->GetAssetManager()->FindOrLoad<BravoTextureAsset>("BlackTextureAsset", BravoTextureSettings("Textures\\black.png"));

	LoadingState = EAssetLoadingState::Loaded;
	return LoadingState;
}

#if SHADER_HOTSWAP
void BravoShaderAsset::CheckShadersForHotSwap()
{
	bool bHotSwapped = false;
	
	Use();
	for ( ShaderHotswapInfo& it : ShaderHotswapInfos )
	{
		std::filesystem::file_time_type ftime = std::filesystem::last_write_time(it.FullPath);
		if ( ftime == it.LastModificationTime )
			continue;


		Log::LogMessage(ELog::Log, "Atempring to hotswap shader: {}", ShaderPath);
		it.LastModificationTime = ftime;

		

		GLuint newShader;
		std::string OutShaderPath;
		if ( !LoadShader(it.ShaderType, newShader, ShaderPath, ShaderDefines, OutShaderPath) )
		{
			if ( newShader ) glDeleteShader(newShader);
			Log::LogMessage(ELog::Error, "Failed to hotswap shader: {}", ShaderPath);
			continue;
		}
		glDetachShader(ProgramID, it.Shader);

		glDeleteShader(it.Shader);
		it.Shader = newShader;

		glAttachShader(ProgramID, newShader);
		
		bHotSwapped = true;
	}

	if ( bHotSwapped )
	{
		if ( !LinkProgramm() )
		{
			Log::LogMessage(ELog::Error, "Failed to link the program: {}", ShaderPath);
			LoadingState = EAssetLoadingState::Unloaded;
		}

		ReapplyCachedUniforms();

		Log::LogMessage(ELog::Success, "Hotswaped {} shader", ShaderPath);
	}

	StopUsage();
}

void BravoShaderAsset::ReapplyCachedUniforms()
{
	std::unordered_map<std::string, UniformValue> Cache = ValueCache;
	ValueCache.clear();
	LocationCache.clear();
	for (const auto& [name, value] : Cache)
	{
		std::visit([&](auto&& val)
			{
				SetValue(name, val);
			}, value);
	}
}
#endif

void BravoShaderAsset::ReleaseFromGPU_Internal()
{
	BravoAsset::ReleaseFromGPU_Internal();
	StopUsage();

#if SHADER_HOTSWAP
	for ( auto it : ShaderHotswapInfos )
	{
		glDeleteShader(it.Shader);
	}
#endif

	if ( ProgramID ) glDeleteProgram(ProgramID);
}

void BravoShaderAsset::Use()
{
	if ( !ProgramID ) return;

	static GLuint CurentShader = 0;
	if ( CurentShader != ProgramID )
	{
		CurentShader = ProgramID;
		glUseProgram(ProgramID);
	}
}

void BravoShaderAsset::StopUsage()
{
	if( EmptyTexture )
		EmptyTexture->StopUsage();
}

bool BravoShaderAsset::LoadShader(GLenum ShaderType, GLuint& OutShader, const std::string& Path, const std::map<std::string, std::string>& ShaderDefines, std::string& OutFullPath)
{
	OutFullPath = "";
	OutShader = 0;
	std::string shaderExtension = ShaderProgrammConstancts::Extension.at(ShaderType);
	const std::string RealShaderName = Engine->GetAssetManager()->FindShader(Path + shaderExtension);
	
	std::ifstream shaderFile(RealShaderName.c_str());
	if ( !shaderFile.is_open() )
	{
		Log::LogMessage(ELog::Error, "Failed to load shader: {}, no such file", Path);
		return false;
	}

	OutFullPath = RealShaderName;

	std::stringstream buffer;
	buffer << shaderFile.rdbuf();
	shaderFile.close();
	std::string ShaderSource = buffer.str();

	for ( auto& define : ShaderDefines )
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
		Log::LogMessage(ELog::Error, "Failed to compile shader: {}", Path);
		Log::LogMessage(ELog::Error, infoLog );
		glDeleteShader(Shader);
		return false;
	}

	Log::LogMessage(ELog::Log, "Loaded shader: {}", Path);
	
	
	OutShader = Shader;

	return true;
}

bool BravoShaderAsset::LinkProgramm()
{
	int32 success;
	int8 infoLog[512];
	Log::LogMessage(ELog::Log, "Linking shader program");
	glLinkProgram(ProgramID);
	// check for linking errors
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(ProgramID, 512, NULL, infoLog);
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

void BravoShaderAsset::SetFloat1(const std::string& name, const float val) const
{
	if ( CheckUniformCache(name, val) ) return;
	glUniform1f(FindUniformLocation(name.c_str()), val);
}
void BravoShaderAsset::SetFloat2(const std::string& name, const glm::vec2& val) const
{
	if ( CheckUniformCache(name, val) ) return;
	glUniform2f(FindUniformLocation(name.c_str()), val.x, val.y);
}
void BravoShaderAsset::SetFloat3(const std::string& name, const glm::vec3& val) const
{
	if ( CheckUniformCache(name, val) ) return;
	glUniform3f(FindUniformLocation(name.c_str()), val.x, val.y, val.z);
}
void BravoShaderAsset::SetFloat4(const std::string& name, const glm::vec4& val) const
{
	if ( CheckUniformCache(name, val) ) return;
	glUniform4f(FindUniformLocation(name.c_str()), val.x, val.y, val.z, val.w);
}

void BravoShaderAsset::SetFloat1v(const std::string& name, const std::vector<float>& val) const
{
	if ( CheckUniformCache(name, val) ) return;
	glUniform1fv(FindUniformLocation(name.c_str()), static_cast<GLsizei>(val.size()), val.data());
}
void BravoShaderAsset::SetFloat2v(const std::string& name, const std::vector<glm::vec2>& val) const
{
	if ( CheckUniformCache(name, val) ) return;
	glUniform2fv(FindUniformLocation(name.c_str()), static_cast<GLsizei>(val.size()), reinterpret_cast<const float*>(val.data()));
}
void BravoShaderAsset::SetFloat3v(const std::string& name, const std::vector<glm::vec3>& val) const
{
	if ( CheckUniformCache(name, val) ) return;
	glUniform3fv(FindUniformLocation(name.c_str()), static_cast<GLsizei>(val.size()), reinterpret_cast<const float*>(val.data()));
}
void BravoShaderAsset::SetFloat4v(const std::string& name, const std::vector<glm::vec4>& val) const
{
	if ( CheckUniformCache(name, val) ) return;
	glUniform4fv(FindUniformLocation(name.c_str()), static_cast<GLsizei>(val.size()), reinterpret_cast<const float*>(val.data()));
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
		location = glGetUniformLocation(ProgramID, name.c_str());
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