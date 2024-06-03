#include "FluidContainer.h"
#include "BravoAssetManager.h"
#include "BravoEngine.h"
#include "BravoShaderAsset.h"
#include "BravoCamera.h"
#include "FluidSimulation.h"


bool FluidContainer::Initialize_Internal()
{
	if ( !BravoObject::Initialize_Internal() )
		return false;

	Shader = Engine->GetAssetManager()->FindOrLoad<BravoShaderAsset>("ContainerShader", BravoShaderLoadingParams("Shaders\\Container"));

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

bool FluidContainer::CheckRoundCollision(glm::vec2& Location, float Radius, glm::vec2& OutVelocityModify) const
{
	bool bResult = false;
	glm::vec2 Bounds = GetSize() - glm::vec2(Radius);
	if ( glm::abs(Location.x) > Bounds.x )
	{
		Location.x = Bounds.x * glm::sign(Location.x);
		OutVelocityModify.x = -1;
		bResult = true;
	}
	if ( glm::abs(Location.y) > Bounds.y )
	{
		Location.y = Bounds.y * glm::sign(Location.y);
		OutVelocityModify.y = -1;
		bResult = true;
	}
	return bResult;
}

const glm::vec2& FluidContainer::GetSize(bool Inside) const
{
	if ( !Inside )
		return Transform.GetScale()*0.5f;
	return Transform.GetScale()*0.5f - glm::vec2(BorderWidth*2.0f);
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

		glm::mat4 ModelTranform = CameraProjection * CameraView * Transform.GetTransformMatrix();

		Shader->SetMatrix4d("modelViewProjection", ModelTranform);
		Shader->SetVector2d("containerSize", Transform.GetScale());
		Shader->SetVector3d("outlineColor", OutlineColor);
		Shader->SetVector1d("borderWidth", BorderWidth);

		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, Simulation->GetParticlesSSBO());

		Shader->SetInt("particleCount", Simulation->ParticlesCount);
		Shader->SetVector1d("targetDensity", Simulation->TargetDensity);
		Shader->SetVector1d("particleMass", Simulation->ParticleMass);
		Shader->SetVector1d("smoothingRadius", Simulation->SmoothingRadius);
	
		glBindVertexArray(VAO);
			glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
	
	Shader->StopUsage();
}