#include "BravoRenderTarget.h"
#include "stdafx.h"
#include "openGL.h"
#include "BravoShader.h"
#include "BravoTexture.h"
#include "BravoTextureUnitManager.h"

void BravoRenderTarget::Setup(const glm::ivec2& InSize, std::shared_ptr<class BravoShader> InShader)
{
	Size = InSize;
	Shader = InShader;

	static float planeVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
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

	// framebuffer configuration
	glGenFramebuffers(1, &FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	// create a color attachment texture
	glGenTextures(1, &TextureColorBuffer);
    glBindTexture(GL_TEXTURE_2D, TextureColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Size.x, Size.y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, TextureColorBuffer, 0);

	// create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
	glGenRenderbuffers(1, &RBO);
    glBindRenderbuffer(GL_RENDERBUFFER, RBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, Size.x, Size.y); // use a single renderbuffer object for both a depth AND stencil buffer.
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO); // now actually attach it
    // now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        Log::LogMessage("Framebuffer is not complete!", ELog::Error);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void BravoRenderTarget::Resize(const glm::ivec2& InSize)
{
	Clean();
	Setup(InSize, Shader);
}

void BravoRenderTarget::Clean()
{
	StopUsage();
	glDeleteBuffers(1, &PlaneVBO);
	glDeleteVertexArrays(1, &PlaneVAO);
	glDeleteTextures(1, &TextureColorBuffer);
	glDeleteRenderbuffers(1, &RBO);
	glDeleteFramebuffers(1, &FBO);
}

void BravoRenderTarget::Use()
{
	glViewport(0, 0, Size.x, Size.y);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
}

void BravoRenderTarget::StopUsage()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	if ( Shader )
		Shader->StopUsage();
}

void BravoRenderTarget::OnDestroy()
{
	Clean();
}

void BravoRenderTarget::Render()
{
	if ( Shader )
	{
		Shader->Use();
			int32 TextureUnit = BravoTextureUnitManager::BindTexture();
			glActiveTexture(GL_TEXTURE0 + TextureUnit);
			glBindTexture(GL_TEXTURE_2D, TextureColorBuffer);
			Shader->SetInt("screenTexture", 0);
	
			glBindVertexArray(PlaneVAO);
    
			glDrawArrays(GL_TRIANGLES, 0, 6);
	
			BravoTextureUnitManager::UnbindTexture(TextureUnit);
			glBindVertexArray(0);
			glActiveTexture(0);
		Shader->StopUsage();
	}
}