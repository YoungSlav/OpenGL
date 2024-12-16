#include "FluidContainer.h"
#include "BravoAssetManager.h"
#include "BravoEngine.h"
#include "BravoShaderAsset.h"
#include "BravoCamera.h"
#include "FluidSimulation.h"
#include "BravoInput.h"
#include "BravoViewport.h"

bool FluidContainer::Initialize_Internal()
{
	if ( !BravoObject::Initialize_Internal() )
		return false;

	Shader = Engine->GetAssetManager()->FindOrLoad<BravoShaderAsset>("ContainerShader", BravoShaderLoadingParams("Container"));

	// vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
	static const float Vertices[] = {
        // positions   // texCoords
        -0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f, -0.5f,  1.0f, 0.0f,

        -0.5f,  0.5f,  0.0f, 1.0f,
         0.5f, -0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  1.0f, 1.0f
    };

	glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), &Vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	return true;
}

const glm::vec2& FluidContainer::GetSize(bool Inside) const
{
	if ( !Inside )
		return Transform.GetScale()*0.5f;
	return Transform.GetScale()*0.5f - glm::vec2(BorderWidth);
}

void FluidContainer::Render()
{
	const std::shared_ptr<BravoCamera> camera = Engine->GetCamera();
	if ( !camera )
		return;

	assert( Simulation != nullptr );
	
	Shader->Use();

		glm::mat4 CameraProjection = camera->GetProjectionMatrix();
		glm::mat4 CameraView = camera->GetViewMatrix();

		glm::mat4 mCamera = CameraProjection * CameraView;
		glm::mat4 model  = Transform.GetTransformMatrix();

		Shader->SetMatrix4d("model", model);
		Shader->SetMatrix4d("camera", mCamera);
		Shader->SetFloat2("containerSize", Transform.GetScale());
		Shader->SetFloat3("outlineColor", OutlineColor);
		Shader->SetFloat1("borderWidth", BorderWidth);

		glm::vec2 mousePos = Engine->GetInput()->GetMousePosition();
		const glm::ivec2 vSize = Engine->GetViewport()->GetViewportSize();
		const glm::vec2 rPos = (glm::vec2(mousePos.x / vSize.x, mousePos.y / vSize.y ) * 2.0f) - glm::vec2(1.0f);

		const glm::vec2 halfWorldSize = GetSize();

		glm::vec2 mWorldPos = rPos * halfWorldSize;
		mWorldPos.y = mWorldPos.y * -1.0f;

		Shader->SetFloat2("mousePos", mWorldPos);

		Shader->SetFloat1("interactionRadius", Simulation->InteractionRadius);
		Shader->SetFloat1("interactionForce", Simulation->InteractionAcceleration);
		Shader->SetFloat1("gravityForce", Simulation->Gravity);

	
		glBindVertexArray(VAO);
			glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
	
	Shader->StopUsage();
}