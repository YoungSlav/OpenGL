#include "BravoEngine.h"

#include "BravoInput.h"
#include "BravoCamera.h"
#include "BravoLightManager.h"
#include "BravoRenderTarget.h"
#include "openGL.h"


std::weak_ptr<BravoEngine> _Engine;
std::shared_ptr<BravoEngine> Engine()
{
	if( _Engine.expired() )
		return nullptr;
	return _Engine.lock();
}

BravoEngine::BravoEngine()
	: SharedFromThis()
{
	ViewportSize.x = 1024;
	ViewportSize.y = 768;
}

void BravoEngine::Init()
{
	_Engine = Self<BravoEngine>();

	Input = SpawnObject<BravoInput>();

	if ( std::shared_ptr<BravoCamera> cam = SpawnObject<BravoCamera>() )
	{
		cam->SetAspectRatio(float(ViewportSize.x) / float(ViewportSize.y) );

		Camera = cam;
	}

	LightManager = SpawnObject<BravoLightManager>();

	CreateOpenGLWindow();
}


void BravoEngine::GameLoop()
{
	while( !bRequestExit && !glfwWindowShouldClose(Window) )
	{
		float newTime = (float)glfwGetTime();
		float DeltaTime = newTime - GameTime;
		GameTime = newTime;

		Tick(DeltaTime);
		UpdateViewport();
		ProcessInput(Window);
	}


	glfwTerminate();
}

void BravoEngine::StopGame()
{
	while( Objects.size() > 0 )
	{
		DestroyObject(Objects.cbegin()->second);
	}

	glfwSetWindowShouldClose(Window, true);
}

void BravoEngine::Tick(float DeltaTime)
{
	for ( auto& it : TickableObjects )
	{
		if ( !it.second.expired() )
		{
			it.second.lock()->Update(DeltaTime);
		}
	}
}

void BravoEngine::UpdateViewport()
{
	if ( !GetLightManager() || !GetViewportRenderTarget() )
	{
		bRequestExit = true;
		return;
	}
	
	GetLightManager()->UpdateLightsDepthMaps();

	// we want to draw into PP texture first
	{
		GetViewportRenderTarget()->Use();
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);

		for ( auto& it : Actors )
		{
			it.lock()->Draw(Camera.lock()->GetLocation(), Camera.lock()->GetProjectionMatrix(), Camera.lock()->GetViewMatrix());
		}
		GetViewportRenderTarget()->StopUsage();
	}
		
	// now we want to draw PP texture to screen
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, ViewportSize.x, ViewportSize.y);
		glDisable(GL_DEPTH_TEST);
		glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		GetViewportRenderTarget()->Draw();
	}
	
	glfwSwapBuffers(Window);
	glfwPollEvents();
}

void BravoEngine::DrawShadowMap(std::shared_ptr<class BravoShader> Shader, const glm::vec3& LightPosition) const
{
	for ( auto& it : Actors )
	{
		if ( !it.expired() )
			it.lock()->DrawToShadowMap(Shader, LightPosition);
	}
}

void BravoEngine::Resize(const glm::ivec2& InViewportSize)
{
	ViewportSize = InViewportSize;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, ViewportSize.x, ViewportSize.y);
	if ( GetViewportRenderTarget() )
		GetViewportRenderTarget()->Resize(ViewportSize*2);
	if ( GetCamera() )
		GetCamera()->SetAspectRatio( float(ViewportSize.x) / float(ViewportSize.y) );
}

void BravoEngine::SetMouseEnabled(bool bNewMouseEnabled) const
{
	if ( bNewMouseEnabled )
		glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	else
		glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void BravoEngine::CreateOpenGLWindow()
{
	// glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// create window
	Window = glfwCreateWindow(ViewportSize.x, ViewportSize.y, "Bravo Engine", NULL, NULL);
    if ( Window == nullptr )
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


	glfwSetCursorPosCallback(Window, BravoEngine::Mouse_callback);
	glfwSetScrollCallback(Window, BravoEngine::Scroll_callback);
    glfwSetFramebufferSizeCallback(Window, BravoEngine::Framebuffer_size_callback);


	if ( std::shared_ptr<BravoRenderTarget> rt = SpawnObject<BravoRenderTarget>() )
	{
		rt->Setup(ViewportSize*2, BravoAsset::Load<BravoShader>("PostProccess"));
		ViewportRenderTarget = rt;
	}
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
}

void BravoEngine::Framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	if ( Engine() )
		Engine()->Resize(glm::ivec2(width, height));
}

void BravoEngine::ProcessInput(GLFWwindow *window)
{
	static float oldTime = 0.0f;
	float newTime = (float)glfwGetTime();
	float deltaTime = newTime - oldTime;
	oldTime = newTime;

	if ( Engine() && Engine()->GetInput() )
		Engine()->GetInput()->ProcessInput(window, deltaTime);
}

void BravoEngine::Scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	static float oldTime = 0.0f;
	float newTime = (float)glfwGetTime();
	float deltaTime = newTime - oldTime;
	oldTime = newTime;

	if ( Engine() && Engine()->GetInput() )
		Engine()->GetInput()->OnMouseScroll(window, (float)xoffset, (float)yoffset, deltaTime);
}

void BravoEngine::Mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	static float oldTime = 0.0f;
	float newTime = (float)glfwGetTime();
	float deltaTime = newTime - oldTime;
	oldTime = newTime;

	if ( Engine() && Engine()->GetInput() )
		Engine()->GetInput()->OnMouseMove(window, (float)xpos, (float)ypos, deltaTime);
}

void BravoEngine::RegisterObject(std::shared_ptr<BravoObject> newObject)
{
	Objects.insert({newObject->GetHandle(), newObject});
	if ( std::shared_ptr<BravoTickable> asTickable = std::dynamic_pointer_cast<BravoTickable>(newObject) )
	{
		TickableObjects.insert({newObject->GetHandle(), asTickable});
	}
	if ( std::shared_ptr<BravoActor> asActor = std::dynamic_pointer_cast<BravoActor>(newObject) )
	{
		Actors.push_back(asActor);
		sort(Actors.begin(), Actors.end(), 
			[](std::weak_ptr<BravoActor> left, std::weak_ptr<BravoActor> right) -> bool
			{ 
				return left.lock()->RenderPriority < right.lock()->RenderPriority; 
			});

		if ( std::shared_ptr<BravoLightActor> asLight = std::dynamic_pointer_cast<BravoLightActor>(asActor) )
		{
			if ( GetLightManager() )
				GetLightManager()->RegisterLightActor(asLight);
		}
	}
}

void BravoEngine::DestroyObject(std::shared_ptr<BravoObject> Object)
{
	Object->OnDestroy();

	TickableObjects.erase(Object->GetHandle());

	if ( std::shared_ptr<BravoActor> asActor = std::dynamic_pointer_cast<BravoActor>(Object) )
	{
		if ( GetLightManager() )
		{
			GetLightManager()->RemoveLightActor(asActor);
		}
		for ( unsigned int i = 0; i < Actors.size(); ++i )
		{
			if ( Actors[i].lock() == asActor )
			{
				Actors.erase(Actors.begin() + i);
				break;
			}
		}
	}

	Objects.erase(Object->GetHandle());
}

std::shared_ptr<BravoInput> BravoEngine::GetInput() const
{
	if ( Input.expired() )
		return nullptr;
	return Input.lock();
}
std::shared_ptr<BravoLightManager> BravoEngine::GetLightManager() const
{
	if ( LightManager.expired() )
		return nullptr;
	return LightManager.lock();
}
std::shared_ptr<BravoCamera> BravoEngine::GetCamera() const
{
	if ( Camera.expired() )
		return nullptr;
	return Camera.lock();
}
std::shared_ptr<BravoRenderTarget> BravoEngine::GetViewportRenderTarget() const
{
	if ( ViewportRenderTarget.expired() )
		return nullptr;
	return ViewportRenderTarget.lock();
}