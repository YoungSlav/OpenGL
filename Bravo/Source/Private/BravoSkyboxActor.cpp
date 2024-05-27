#include "BravoSkyboxActor.h"
#include "BravoAssetManager.h"
#include "BravoEngine.h"
#include "BravoCamera.h"

void BravoSkyboxActor::SetCubemap(std::shared_ptr<BravoCubemapAsset> InCubemap)
{
	Cubemap = InCubemap;
}

bool BravoSkyboxActor::Initialize_Internal()
{
	const float skyboxVertices[] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		+1.0f, -1.0f, -1.0f,
		+1.0f, -1.0f, -1.0f,
		+1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		+1.0f, -1.0f, -1.0f,
		+1.0f, -1.0f,  1.0f,
		+1.0f,  1.0f,  1.0f,
		+1.0f,  1.0f,  1.0f,
		+1.0f,  1.0f, -1.0f,
		+1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		+1.0f,  1.0f,  1.0f,
		+1.0f,  1.0f,  1.0f,
		+1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		+1.0f,  1.0f, -1.0f,
		+1.0f,  1.0f,  1.0f,
		+1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		+1.0f, -1.0f, -1.0f,
		+1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		+1.0f, -1.0f,  1.0f
	};
	Shader = Engine->GetAssetManager()->FindOrLoad<BravoShaderAsset>("SkyboxShader", BravoShaderLoadingParams("Shaders\\Skybox"));

	glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	return true;
}

void BravoSkyboxActor::Render()
{
	if ( !Shader || !Cubemap || !VAO )
		return;

	Shader->Use();
	Cubemap->Use();
	glm::mat4 CameraProjection;
	glm::mat4 CameraView;
	if ( std::shared_ptr<BravoCamera> camera = Engine->GetCamera() )
	{
		CameraProjection = camera->GetProjectionMatrix();
		CameraView = camera->GetViewMatrix();
	}
	else
	{
		return;
	}

	glm::mat4 view = glm::mat4(glm::mat3(CameraView));  
	Shader->SetMatrix4d("view", view);
	Shader->SetMatrix4d("projection", CameraProjection);
	Shader->SetCubemap("skybox", Cubemap);

	glDepthFunc(GL_LEQUAL);
	
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	glBindVertexArray(0);
	glDepthFunc(GL_LESS);

	Shader->StopUsage();
	Cubemap->StopUsage();
}

void BravoSkyboxActor::OnDestroy()
{
	if ( Shader )
		Shader->ReleaseFromGPU();
	if ( Cubemap )
		Cubemap->ReleaseFromGPU();

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
}