#include "BravoViewport.h"
#include "BravoCamera.h"
#include "BravoInput.h"
#include "BravoActor.h"
#include "BravoEngine.h"
#include "BravoRenderTarget.h"
#include "BravoAsset.h"
#include "BravoShader.h"
#include "BravoLightActor.h"
#include "BravoLightManager.h"
#include "openGL.h"

void BravoViewport::Init()
{
	ViewportSize.x = 1024;
	ViewportSize.y = 768;

	CreateOpenGLWindow();
	CreateDefaultCamera();

	LightManager = Engine->SpawnObject<BravoLightManager>();
}

void BravoViewport::CreateOpenGLWindow()
{
	// glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// create window
	Window = glfwCreateWindow(ViewportSize.x, ViewportSize.y, "Bravo Engine", NULL, NULL);
    if (Window == NULL)
    {
		Log::LogMessage("Failed to create GLFW window", ELog::Error);
        glfwTerminate();
		return;
    }
    glfwMakeContextCurrent(Window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
		Log::LogMessage("Failed to initialize GLAD", ELog::Error);
		return;
    }

	glfwFocusWindow(Window);

	glfwSetCursorPosCallback(Window, BravoViewport::Mouse_callback);
	glfwSetScrollCallback(Window, BravoViewport::Scroll_callback);
    glfwSetFramebufferSizeCallback(Window, BravoViewport::Framebuffer_size_callback);


	PostProcessRenderTarget = Engine->SpawnObject<BravoRenderTarget>();
	PostProcessRenderTarget->Setup(ViewportSize*2, BravoAsset::Load<BravoShader>("PostProccess"));
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
}

void BravoViewport::SetMouseEnabled(bool bNewMouseEnabled) const
{
	if ( bNewMouseEnabled )
		glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	else
		glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void BravoViewport::CreateDefaultCamera()
{
	Camera = Engine->SpawnObject<BravoCamera>();
	Camera->SetFOV(45.0f);
	Camera->SetAspectRatio(float(ViewportSize.x) / float(ViewportSize.y) );
	Camera->SetDrawingDistance(MinDrawDistance, MaxDrawDistance);
}

void BravoViewport::OnDestroy()
{
	// glfw: terminate, clearing all previously allocated GLFW resources.
	glfwTerminate();

	if ( Camera )
		Camera->Destroy();
}

void BravoViewport::GameLoop()
{
	while( !bShutDown && !glfwWindowShouldClose(Window) )
	{
		float newTime = (float)glfwGetTime();
		float DeltaTime = newTime - OldGameTime;
		OldGameTime = newTime;

		Engine->Tick(DeltaTime);
		UpdateViewport();
		ProcessInput(Window);
	}
}

void BravoViewport::ShutDown()
{
	bShutDown = true;
}

void BravoViewport::SetCamera(BravoCamera* InCamera)
{
	Camera = InCamera;
}

void BravoViewport::AddActor(BravoActor* InActor)
{
	Actors.push_back(InActor);
	sort(Actors.begin(), Actors.end(), 
		[](BravoActor* left, BravoActor* right) -> bool
		{ 
			return left->RenderPriority < right->RenderPriority; 
		});

	if ( BravoLightActor* asLight = dynamic_cast<BravoLightActor*>(InActor) )
	{
		LightManager->RegisterLightActor(asLight);
	}
}

void BravoViewport::RemoveActor(BravoObjectHandle InHandle)
{
	for ( unsigned int i = 0; i < Actors.size(); ++i )
	{
		if ( Actors[i]->GetHandle() == InHandle )
		{
			Actors.erase(Actors.begin() + i);
			break;
		}
	}
	LightManager->RemoveLightActor(InHandle);
}

void BravoViewport::UpdateViewport()
{
	LightManager->UpdateLightsDepthMaps();

	// we want to draw into PP texture first
	{
		PostProcessRenderTarget->Use();
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);

		for ( auto& it : Actors )
		{
			it->Draw(Camera->GetLocation(), Camera->GetProjectionMatrix(), Camera->GetViewMatrix());
		}
		PostProcessRenderTarget->StopUsage();
	}
	
		
	// now we want to draw PP texture to screen
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, ViewportSize.x, ViewportSize.y);
		glDisable(GL_DEPTH_TEST);
		glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		PostProcessRenderTarget->Draw();
	}


	
	glfwSwapBuffers(Window);
	glfwPollEvents();
}



void BravoViewport::DrawDepthMap(std::shared_ptr<class BravoShader> Shader, const glm::vec3& Position) const
{
	for ( auto& it : Actors )
		it->DrawToShadowMap(Shader, Position);
}

void BravoViewport::Framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	BravoEngine::GetInstance()->GetViewport()->Resize(glm::ivec2(width, height));
}

void BravoViewport::Resize(const glm::ivec2& InViewportSize)
{
	ViewportSize = InViewportSize;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, ViewportSize.x, ViewportSize.y);
	PostProcessRenderTarget->Resize(ViewportSize*2);
	if ( Camera )
		Camera->SetAspectRatio( float(ViewportSize.x) / float(ViewportSize.y) );
}

void BravoViewport::ProcessInput(GLFWwindow *window)
{
	static float oldTime = 0.0f;
	float newTime = (float)glfwGetTime();
	float deltaTime = newTime - oldTime;
	oldTime = newTime;

	if ( BravoEngine::GetInstance()->GetInput() )
		BravoEngine::GetInstance()->GetInput()->ProcessInput(window, deltaTime);
}

void BravoViewport::Scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	static float oldTime = 0.0f;
	float newTime = (float)glfwGetTime();
	float deltaTime = newTime - oldTime;
	oldTime = newTime;

	if ( BravoEngine::GetInstance()->GetInput() )
		BravoEngine::GetInstance()->GetInput()->OnMouseScroll(window, (float)xoffset, (float)yoffset, deltaTime);
}

void BravoViewport::Mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	static float oldTime = 0.0f;
	float newTime = (float)glfwGetTime();
	float deltaTime = newTime - oldTime;
	oldTime = newTime;

	if ( BravoEngine::GetInstance()->GetInput() )
		BravoEngine::GetInstance()->GetInput()->OnMouseMove(window, (float)xpos, (float)ypos, deltaTime);
}
