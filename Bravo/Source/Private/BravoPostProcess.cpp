#include "BravoPostProcess.h"
#include "BravoShaderAsset.h"
#include "BravoViewport.h"
#include "BravoEngine.h"
#include "openGL.h"

bool BravoPostProcess::Initialize_Internal()
{
	// vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
	static float planeVertices[] = {
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };

	glGenVertexArrays(1, &PlaneVAO);
    glGenBuffers(1, &PlaneVBO);
    glBindVertexArray(PlaneVAO);
    glBindBuffer(GL_ARRAY_BUFFER, PlaneVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    return true;
}

void BravoPostProcess::OnDestroy()
{
    glDeleteBuffers(1, &PlaneVBO);
	glDeleteVertexArrays(1, &PlaneVAO);
}

bool BravoPostProcess::EnsureReady()
{
	if ( !Shader )
		return false;
	if ( !Shader->EnsureReady() )
		return false;

	return true;
}

bool BravoPostProcess::Render()
{
	if ( !EnsureReady() )
		return false;

	Render_Internal();

	return true;
}

void BravoPostProcess::Draw()
{
    glBindVertexArray(PlaneVAO);
	    glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}