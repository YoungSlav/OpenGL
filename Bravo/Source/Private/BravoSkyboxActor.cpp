#include "BravoSkyboxActor.h"
#include "BravoAssetManager.h"
#include "BravoEngine.h"

void BravoSkyboxActor::SetCubemap(BravoCubemapPtr InCubemap)
{
	Cubemap = InCubemap;
}

void BravoSkyboxActor::Init()
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
	Shader = GetEngine()->GetAssetManager()->LoadAsset<BravoShader>("Shaders\\Skybox");

	glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
}

void BravoSkyboxActor::Render(const glm::vec3& CameraLocation, const glm::mat4& CameraProjection, const glm::mat4& CameraView) const
{
	Shader->Use();
	Cubemap->Use();

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
	Shader->ReleaseFromGPU();
	Cubemap->ReleaseFromGPU();

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
}