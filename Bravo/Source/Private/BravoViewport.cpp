#include "BravoViewport.h"
#include "BravoCamera.h"
#include "BravoEngine.h"
#include "BravoLightManager.h"
#include "BravoRenderTarget.h"
#include "BravoOutlineManager.h"
#include "BravoHUD.h"
#include "BravoAssetManager.h"

bool BravoViewport::Initialize_Internal()
{
	return true;
}

void BravoViewport::OnDestroy()
{
	ViewportRenderTarget.reset();
	HUD.reset();
	OutlineManager.reset();
}

void BravoViewport::Setup()
{
	CreateOpenGLWindow();

	HUD = NewObject<BravoHUD>("HUD");
	HUD->SetSize(ViewportSize);

	OutlineManager = NewObject<BravoOutlineManager>("OutlineManager");
}

void BravoViewport::CreateOpenGLWindow()
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

    glfwSetFramebufferSizeCallback(Window, BravoViewport::Framebuffer_size_callback);


	if ( ViewportRenderTarget = NewObject<BravoRenderTarget>("ViewportRenderTarget") )
	{
		ViewportRenderTarget->Setup(ViewportSize*2,
			GL_RGB16F, GL_RGB, GL_FLOAT, true,
			Engine->GetAssetManager()->FindOrLoad<BravoShaderAsset>("PostProccesShaderAsset", BravoShaderLoadingParams("Shaders\\PostProccess")));
	}
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
}

void BravoViewport::Framebuffer_size_callback(GLFWwindow* window, int32 width, int32 height)
{
	BravoEngine::GetEngine()->GetViewport()->Resize(glm::ivec2(width, height));
}

void BravoViewport::Resize(const glm::ivec2& InViewportSize)
{
	ViewportSize = InViewportSize;
	PushFramebuffer(0, ViewportSize);
	PopFramebuffer();
	ViewportRenderTarget->Resize(ViewportSize*2);	
	

	OnResizeDelegate.Broadcast(ViewportSize);
	HUD->SetSize(ViewportSize);
}

void BravoViewport::PushFramebuffer(uint32 Framebuffer, const glm::ivec2& Size)
{
	if ( FramebufferStack.empty() || std::get<0>(FramebufferStack.top()) != Framebuffer )
	{
		FramebufferStack.push({Framebuffer, Size});
		glBindFramebuffer(GL_FRAMEBUFFER, Framebuffer);
		glViewport(0, 0, Size.x, Size.y);
	}
}
void BravoViewport::PopFramebuffer()
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

void BravoViewport::RegisterRenderable(std::shared_ptr<IBravoRenderable> Renderable)
{
	auto RenderGroup = AddRenderGroup(Renderable->GetRenderGroup());
		
	RenderGroup->Renderables.push_back(Renderable);
		
	RenderGroup->Renderables.sort(
		[](std::shared_ptr<IBravoRenderable> left, std::shared_ptr<IBravoRenderable> right) -> bool
		{ 
			return left->GetRenderPriority() < right->GetRenderPriority(); 
		});
}

void BravoViewport::RemoveRenderable(std::shared_ptr<IBravoRenderable> Renderable)
{
	for ( auto it : RenderGroups )
	{
		it.second->Renderables.erase(
			std::remove(it.second->Renderables.begin(), it.second->Renderables.end(), Renderable),
			it.second->Renderables.end()
		);
	}
}

void BravoViewport::RenderSelectionIDs() const
{
	auto RenderGroup = GetRenderGroup(ERenderGroup::Main);
	for ( auto& it : RenderGroup->Renderables )
	{
		if ( it->IsVisisble() )
			it->RenderSelectionID();
	}
}

void BravoViewport::RenderDepthMap(std::shared_ptr<class BravoShaderAsset> Shader) const
{
	auto RenderGroup = GetRenderGroup(ERenderGroup::Main);
	for ( auto& it : RenderGroup->Renderables )
	{
		if ( it->IsVisisble() && it->GetCastShadows() )
			it->RenderDepthMap(Shader);
	}
}

bool BravoViewport::DeProject(const glm::vec2& ScreenPos, glm::vec3& OutOrigin, glm::vec3& OutDirection)
{
	std::shared_ptr<BravoCamera> camera = Engine->GetCamera();

	float mouseX = ScreenPos.x / (ViewportSize.x * 0.5f) - 1.0f;
    float mouseY = ScreenPos.y / (ViewportSize.y * 0.5f) - 1.0f;

	glm::mat4 invVP = glm::inverse(camera->GetProjectionMatrix() * camera->GetViewMatrix());

	glm::vec4 screenPos = glm::vec4(mouseX, -mouseY, 1.0f, 1.0f);

	glm::vec4 worldPos = invVP * screenPos;

	OutOrigin = camera->GetLocation();
	OutDirection = glm::normalize(glm::vec3(worldPos));

	return true;
}

void BravoViewport::UpdateViewport()
{
	std::shared_ptr<BravoCamera> camera = Engine->GetCamera();
	std::shared_ptr<BravoLightManager> lightManager = Engine->GetLightManager();
	std::shared_ptr<BravoRenderTarget> viewportRT = ViewportRenderTarget;

	if ( !lightManager || !viewportRT || !camera )
	{
		Engine->ShutDown();
		return;
	}

	
	//glCullFace(GL_FRONT);
	lightManager->UpdateLightsShaderData();
	//glCullFace(GL_BACK);
	
	// we want to draw into PP texture first
	{
		viewportRT->Bind();
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		auto RenderGroup = GetRenderGroup(ERenderGroup::Main);

		for ( auto& it : RenderGroup->Renderables )
		{
			if ( it->IsVisisble() )
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