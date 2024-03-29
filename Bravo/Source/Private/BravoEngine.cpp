#include "BravoEngine.h"
#include "BravoInput.h"
#include "BravoCamera.h"
#include "BravoLightManager.h"
#include "BravoRenderTarget.h"
#include "BravoAssetManager.h"
#include "openGL.h"
#include "BravoHUD.h"
#include "IBravoRenderable.h"
#include "BravoSelectionManager.h"
#include "BravoOutlineManager.h"

namespace GlobalEngine
{
	std::weak_ptr<BravoEngine> _Engine;
	std::shared_ptr<BravoEngine> Engine()
	{
		if( _Engine.expired() )
			return nullptr;
		return _Engine.lock();
	}
};

void BravoEngine::PushFramebuffer(uint32 Framebuffer, const glm::ivec2& Size)
{
	if ( FramebufferStack.empty() || std::get<0>(FramebufferStack.top()) != Framebuffer )
	{
		FramebufferStack.push({Framebuffer, Size});
		glBindFramebuffer(GL_FRAMEBUFFER, Framebuffer);
		glViewport(0, 0, Size.x, Size.y);
	}
}
void BravoEngine::PopFramebuffer()
{
	if ( FramebufferStack.empty() )
	{
		return;
	}
	
	FramebufferStack.pop();
	if ( !FramebufferStack.empty() )
	{
		uint32 Framebuffer = std::get<0>(FramebufferStack.top());
		const glm::vec2& Size = std::get<1>(FramebufferStack.top());
		glBindFramebuffer(GL_FRAMEBUFFER, Framebuffer);
		glViewport(0, 0, Size.x, Size.y);
	}
}

std::shared_ptr<BravoEngine> BravoEngine::GetEngine()
{
	return GlobalEngine::Engine();
}

bool BravoEngine::Initialize_Internal()
{
	GlobalEngine::_Engine = Self<BravoEngine>();
	Engine = Self<BravoEngine>();
	LastUsedHandle = GetHandle();

	AssetManager = NewObject<BravoAssetManager>("AssetManager");

	CreateOpenGLWindow();

	Input = NewObject<BravoInput>("Input");
	Input->SetOwnerWindow(Window);

	LightManager = NewObject<BravoLightManager>("LightManager");

	HUD = NewObject<BravoHUD>("HUD");
	HUD->SetSize(ViewportSize);


	SelectionManager = NewObject<BravoSelectionManager>("SelectionManager");

	OutlineManager = NewObject<BravoOutlineManager>("OutlineManager");

	return true;
}

void BravoEngine::SetCamera(std::shared_ptr<class BravoCamera> _Camera)
{
	Camera = _Camera;
}


void BravoEngine::GameLoop()
{
	while( !bRequestExit && !glfwWindowShouldClose(Window) )
	{
		float newTime = (float)glfwGetTime();
		float DeltaTime = newTime - GameTime;
		GameTime = newTime;

		Tick(DeltaTime);
		
		if ( GetCamera() )
			GetCamera()->UpdateCamera();

		UpdateViewport();
		if ( Input )
			Input->ProcessInput(DeltaTime);

		while ((glfwGetTime()  - newTime) < (1.0 / FPSLimit)) { }
	}

	glfwTerminate();
}

void BravoEngine::StopGame()
{
	while( Objects.size() > 0 )
	{
		DestroyObject(*Objects.cbegin());
	}

	glfwSetWindowShouldClose(Window, true);
}

void BravoEngine::Tick(float DeltaTime)
{
	for ( auto it : TickableObjects )
		it->Update(DeltaTime);
}

void BravoEngine::UpdateViewport()
{
	std::shared_ptr<BravoCamera> camera = GetCamera();
	std::shared_ptr<BravoLightManager> lightManager = GetLightManager();
	std::shared_ptr<BravoRenderTarget> viewportRT = ViewportRenderTarget;

	if ( !lightManager || !viewportRT || !camera )
	{
		bRequestExit = true;
		return;
	}

	AssetManager->CheckPendingAssets();
	
	//glCullFace(GL_FRONT);
	lightManager->UpdateLightsShaderData();
	//glCullFace(GL_BACK);
	
	// we want to draw into PP texture first
	{
		viewportRT->Bind();
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		for ( auto& it : RenderableObjects )
		{
			it->Render();
		}

		OutlineManager->RenderSelections();

		viewportRT->Unbind();
	}


	glDisable(GL_DEPTH_TEST);
	// render everything on screen
	{
		PushFramebuffer(0, ViewportSize);
		glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		viewportRT->Render();
		PopFramebuffer();
	}
	HUD->Render();
	glEnable(GL_DEPTH_TEST);

	glfwSwapBuffers(Window);
	glfwPollEvents();
}

void BravoEngine::RenderSelectionIDs() const
{
	for ( auto& it : RenderableObjects )
	{
		it->RenderSelectionID();
	}
}

void BravoEngine::RenderDepthMap(std::shared_ptr<class BravoShaderAsset> Shader) const
{
	for ( auto& it : RenderableObjects )
	{
		if ( it->GetCastShadows() )
			it->RenderDepthMap(Shader);
	}
}

void BravoEngine::Resize(const glm::ivec2& InViewportSize)
{
	ViewportSize = InViewportSize;
	PushFramebuffer(0, ViewportSize);
	PopFramebuffer();
	ViewportRenderTarget->Resize(ViewportSize*2);	
	

	OnResizeDelegate.Broadcast(ViewportSize);
	HUD->SetSize(ViewportSize);
}

void BravoEngine::CreateOpenGLWindow()
{
	// glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
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

    glfwSetFramebufferSizeCallback(Window, BravoEngine::Framebuffer_size_callback);


	if ( ViewportRenderTarget = NewObject<BravoRenderTarget>("ViewportRenderTarget") )
	{
		ViewportRenderTarget->Setup(ViewportSize*2,
			GL_RGB16F, GL_RGB, GL_FLOAT, true,
			AssetManager->FindOrLoad<BravoShaderAsset>("PostProccesShaderAsset", BravoShaderLoadingParams("Shaders\\PostProccess")));
	}
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
}

void BravoEngine::Framebuffer_size_callback(GLFWwindow* window, int32 width, int32 height)
{
	if ( GlobalEngine::Engine() )
		GlobalEngine::Engine()->Resize(glm::ivec2(width, height));
}

void BravoEngine::RegisterObject(std::shared_ptr<BravoObject> newObject)
{
	Objects.push_back(newObject);

	HandleToObject.insert({newObject->GetHandle(), newObject});
	
	if ( std::shared_ptr<IBravoTickable> asTickable = std::dynamic_pointer_cast<IBravoTickable>(newObject) )
	{
		TickableObjects.push_back(asTickable);
	}
	if ( std::shared_ptr<BravoActor> asActor = std::dynamic_pointer_cast<BravoActor>(newObject) )
	{
		Actors.push_back(asActor);

		if ( std::shared_ptr<BravoLightManager> lightManager = GetLightManager() )
		{
			if ( std::shared_ptr<BravoLightActor> asLight = std::dynamic_pointer_cast<BravoLightActor>(asActor) )
			{
				lightManager->RegisterLightActor(asLight);
			}
		}
	}
	if ( std::shared_ptr<IBravoRenderable> asRenderable = std::dynamic_pointer_cast<IBravoRenderable>(newObject) )
	{
		RenderableObjects.push_back(asRenderable);
		
		RenderableObjects.sort(
			[](std::shared_ptr<IBravoRenderable> left, std::shared_ptr<IBravoRenderable> right) -> bool
			{ 
				return left->GetRenderPriority() < right->GetRenderPriority(); 
			});
	}
}

std::shared_ptr<BravoObject> BravoEngine::FindObjectByHandle(const BravoHandle& Handle)
{
	auto it = HandleToObject.find(Handle);
	if ( it != HandleToObject.end() )
		return it->second;
	return nullptr;
}

void BravoEngine::DestroyObject(std::shared_ptr<BravoObject> Object)
{
	if ( Input )
		Input->UnSubscribeAll(Object);

	HandleToObject.erase(Object->GetHandle());

	if ( std::shared_ptr<BravoActor> asActor = std::dynamic_pointer_cast<BravoActor>(Object) )
	{
		if ( std::shared_ptr<BravoLightManager> lightManager = GetLightManager() )
		{
			if ( std::shared_ptr<BravoLightActor> asLight = std::dynamic_pointer_cast<BravoLightActor>(asActor) )
				lightManager->RemoveLightActor(asLight);
		}
		Actors.erase(std::remove(Actors.begin(), Actors.end(), asActor), Actors.end());
	}

	if ( std::shared_ptr<IBravoTickable> asTiackble = std::dynamic_pointer_cast<IBravoTickable>(Object) )
		TickableObjects.erase(std::remove(TickableObjects.begin(), TickableObjects.end(), asTiackble), TickableObjects.end());

	if ( std::shared_ptr<IBravoRenderable> asRenderable = std::dynamic_pointer_cast<IBravoRenderable>(Object) )
		RenderableObjects.erase(std::remove(RenderableObjects.begin(), RenderableObjects.end(), asRenderable), RenderableObjects.end());

	Objects.erase(std::remove(Objects.begin(), Objects.end(), Object), Objects.end());
}

void BravoEngine::OnDestroy()
{
	BravoObject::OnDestroy();

	Input.reset();
	LightManager.reset();
	ViewportRenderTarget.reset();
	HUD.reset();
	SelectionManager.reset();
	OutlineManager.reset();
}