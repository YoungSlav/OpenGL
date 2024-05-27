#include "BravoRenderTarget.h"
#include "stdafx.h"
#include "openGL.h"
#include "BravoEngine.h"
#include "BravoViewport.h"

void BravoRenderTarget::Setup()
{
	// framebuffer configuration
	glGenFramebuffers(1, &FBO);
    Engine->GetViewport()->PushFramebuffer(FBO, Size);
	
	// create a color attachment texture
	glGenTextures(1, &TextureColorBuffer);
    glBindTexture(GL_TEXTURE_2D, TextureColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, InternalFormat, Size.x, Size.y, 0, Format, Type, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, TextureColorBuffer, 0);

	if ( DepthComponent )
	{
		glGenTextures(1, &TextureDepthBuffer);
		glBindTexture(GL_TEXTURE_2D, TextureDepthBuffer);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, Size.x, Size.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
			constexpr float bordercolor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
			glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, bordercolor);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, TextureDepthBuffer, 0);
	}
	// now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        Log::LogMessage(ELog::Error, "Framebuffer is not complete!");
    Engine->GetViewport()->PopFramebuffer();
}

void BravoRenderTarget::Resize(const glm::ivec2& InSize)
{
	Clean();
	Size = InSize;
}

void BravoRenderTarget::Clean()
{	
	glDeleteTextures(1, &TextureColorBuffer);
	glDeleteTextures(1, &TextureDepthBuffer);
	glDeleteFramebuffers(1, &FBO);
}

void BravoRenderTarget::Bind()
{
	Engine->GetViewport()->PushFramebuffer(FBO, Size);
}

void BravoRenderTarget::Unbind()
{
	Engine->GetViewport()->PopFramebuffer();
}

void BravoRenderTarget::OnDestroy()
{
	Clean();
}