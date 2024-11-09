#include "BravoBoundingBox.h"
#include "BravoAssetManager.h"
#include "BravoEngine.h"
#include "BravoCamera.h"
#include "BravoShaderAsset.h"

static const glm::vec3 cubeVertices[] = {
	glm::vec3(-0.5f, -0.5f, -0.5f),
    glm::vec3( 0.5f, -0.5f, -0.5f),
    glm::vec3( 0.5f,  0.5f, -0.5f),
    glm::vec3(-0.5f,  0.5f, -0.5f),
    glm::vec3(-0.5f, -0.5f,  0.5f),
    glm::vec3( 0.5f, -0.5f,  0.5f),
    glm::vec3( 0.5f,  0.5f,  0.5f),
    glm::vec3(-0.5f,  0.5f,  0.5f)
};

bool BravoBoundingBox::Initialize_Internal()
{
	if ( !BravoComponent::Initialize_Internal() )
		return false;

	OnTransformUpdated.AddRaw(this, &BravoBoundingBox::UpdateVertices);

	uint32 cubeIndices[] = {
		// Front face
		0, 1, 2,  0, 2, 3,
		// Right face
		1, 5, 6,  1, 6, 2,
		// Back face
		5, 4, 7,  5, 7, 6,
		// Left face
		4, 0, 3,  4, 3, 7,
		// Top face
		3, 2, 6,  3, 6, 7,
		// Bottom face
		4, 5, 1,  4, 1, 0
	};

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * 8, &cubeVertices, GL_STATIC_DRAW);
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32) * 36,	&cubeIndices, GL_STATIC_DRAW);	
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	
	Shader = Engine->GetAssetManager()->FindOrLoad<BravoShaderAsset>("BoundingBoxShader",
		BravoShaderLoadingParams("Shaders\\BoundingBox"));

	UpdateVertices(nullptr);
	SetScale(glm::vec3(10.0f));

	return true;
}

void BravoBoundingBox::UpdateVertices(const IBravoTransformable* )
{
	std::vector<glm::vec3> Corners;
	Corners.reserve(8);
	const glm::mat4 model = GetTransform_World().GetTransformMatrix();

	for (const glm::vec3& corner : cubeVertices)
	{
		glm::vec4 transformed = model * glm::vec4(corner, 1.0f);
		Corners.push_back(glm::vec3(transformed));
	}

	Shader->Use();
		Shader->SetFloat3v("corners", 8, &Corners[0].x);
	Shader->StopUsage();
}

void BravoBoundingBox::Render()
{
	if ( !Shader || !VAO )
		return;

	const std::shared_ptr<BravoCamera> camera = Engine->GetCamera();
	if ( !camera )
		return;
	
	const glm::mat4 CameraProjection = camera->GetProjectionMatrix();
	const glm::mat4 CameraView = camera->GetViewMatrix();

	const glm::mat4 model = GetTransform_World().GetTransformMatrix();

	Shader->Use();
		Shader->SetMatrix4d("projection", CameraProjection);
		Shader->SetMatrix4d("view", CameraView);
		Shader->SetMatrix4d("model", model);

		Shader->SetFloat1("EdgeSize", EdgeSize);
		Shader->SetFloat3("EdgeColor", Color);

		glDisable(GL_CULL_FACE);

		glBindVertexArray(VAO);
			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		glEnable(GL_CULL_FACE);
	Shader->StopUsage();

}


void BravoBoundingBox::OnDestroy()
{
	glDeleteVertexArrays(6, &VAO);
	VAO = 0;
	
	if ( Shader ) Shader->ReleaseFromGPU();
}
