#include "BravoViewport.h"
#include "BravoCamera.h"
#include "BravoEngine.h"
#include "BravoLightManager.h"
#include "BravoRenderTarget.h"
#include "BravoPostProcess_Outline.h"
#include "BravoPostProcess_AntiAliasing.h"
#include "BravoHUD.h"
#include "BravoAssetManager.h"
#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_opengl3.h"


bool BravoViewport::Initialize_Internal()
{
	return true;
}

void BravoViewport::OnDestroy()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui::DestroyContext();
}

void BravoViewport::Setup()
{
	CreateOpenGLWindow();

	// Setup ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
		
	ImGui_ImplGlfw_InitForOpenGL(Window, true);

	ImGuiIO& io = ImGui::GetIO();
	io.DisplaySize = ImVec2(float(ViewportSize.x), float(ViewportSize.y));
	io.IniFilename = NULL;
	io.LogFilename = NULL;

	ImGui_ImplOpenGL3_Init("#version 430");
	
	HUD = NewObject<BravoHUD>("HUD");
	HUD->SetSize(ViewportSize);

	ViewportRenderTarget = NewObject<BravoRenderTarget>("ViewportRenderTarget", ViewportSize*2, GL_RGB16F, GL_RGB, GL_FLOAT, true);

	OutlinePP = NewObject<BravoPostProcess_Outline>("OutlinePostProcessP");

	AntiAliasingPP = NewObject<BravoPostProcess_AntiAliasing>("AntiAliasingPostProcess", ViewportRenderTarget->GetColorTexture());

}

void BravoViewport::CreateOpenGLWindow()
{
	// glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// create window
	Window = glfwCreateWindow(ViewportSize.x, ViewportSize.y, "Bravo Engine", NULL, NULL);
    if ( Window == nullptr )
    {
		Log::LogMessage(ELog::Error, "Failed to create GLFW window");
        glfwTerminate();
		return;
    }


    glfwMakeContextCurrent(Window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
		Log::LogMessage(ELog::Error, "Failed to initialize GLAD");
		return;
    }

	glfwFocusWindow(Window);

    glfwSetFramebufferSizeCallback(Window, BravoViewport::Framebuffer_size_callback);


	glEnable(GL_PROGRAM_POINT_SIZE);
	
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
	
	ImGuiIO& io = ImGui::GetIO();
	io.DisplaySize = ImVec2(float(ViewportSize.x), float(ViewportSize.y));

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
		const glm::ivec2& Size = std::get<1>(FramebufferStack.top());
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
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	if ( auto RenderGroup = GetRenderGroup(ERenderGroup::Main) )
	{
		for ( auto& it : RenderGroup->Renderables )
		{
			if ( it->IsVisisble() )
				it->RenderSelectionID();
		}
	}

	if ( auto RenderGroup = GetRenderGroup(ERenderGroup::Overlay) )
	{
		glClear(GL_DEPTH_BUFFER_BIT);
		for ( auto& it : RenderGroup->Renderables )
		{
			if ( it->IsVisisble() )
				it->RenderSelectionID();
		}
	}
}

void BravoViewport::RenderDepthMap(std::shared_ptr<class BravoShaderAsset> Shader) const
{
	if ( auto RenderGroup = GetRenderGroup(ERenderGroup::Main) )
	{
		for ( auto& it : RenderGroup->Renderables )
		{
			if ( it->IsVisisble() && it->GetCastShadows() )
				it->RenderDepthMap(Shader);
		}
	}
}

void BravoViewport::DeProject(const glm::vec2& ScreenPos, glm::vec3& OutOrigin, glm::vec3& OutDirection)
{
	std::shared_ptr<BravoCamera> camera = Engine->GetCamera();

	double x = ScreenPos.x;
	double y = ViewportSize.y - 1.0f - ScreenPos.y;

	OutOrigin = glm::unProject(
		glm::vec3(x, y, 0.0),
		camera->GetViewMatrix(),
		camera->GetProjectionMatrix(),
		glm::ivec4(0, 0, ViewportSize.x, ViewportSize.y));

	glm::vec3 farPlane = glm::unProject(
		glm::vec3(x, y, 1.0),
		camera->GetViewMatrix(),
		camera->GetProjectionMatrix(),
		glm::ivec4(0, 0, ViewportSize.x, ViewportSize.y));

	OutDirection = glm::normalize(farPlane - OutOrigin);
}

void BravoViewport::AddPostProcess(std::shared_ptr<class BravoPostProcess> PP)
{
	PostProcesses.push_back(PP);
}

void BravoViewport::UpdateViewport(float DeltaTime)
{
	std::shared_ptr<BravoCamera> camera = Engine->GetCamera();
	std::shared_ptr<BravoLightManager> lightManager = Engine->GetLightManager();
	std::shared_ptr<BravoRenderTarget> viewportRT = ViewportRenderTarget;

	if ( !lightManager || !viewportRT || !camera )
	{
		assert(false);
		Engine->ShutDown();
		return;
	}

	
	//glCullFace(GL_FRONT);
	lightManager->UpdateLightsShaderData();
	//glCullFace(GL_BACK);
	
	// we want to draw into PP texture first
	{
		if ( bWireframe )
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		viewportRT->Bind();
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if ( auto RenderGroup = GetRenderGroup(ERenderGroup::Main) )
		{
			for ( auto& it : RenderGroup->Renderables )
			{
				if ( it->IsVisisble() )
					it->Render();
			}
		}

		if ( bWireframe )
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		if ( auto RenderGroup = GetRenderGroup(ERenderGroup::Overlay) )
		{
			glClear(GL_DEPTH_BUFFER_BIT);
			for ( auto& it : RenderGroup->Renderables )
			{
				if ( it->IsVisisble() )
					it->Render();
			}
		}

		for ( auto it : PostProcesses )
		{
			it->Render();
		}

		OutlinePP->Render();

		viewportRT->Unbind();
	}


	glDisable(GL_DEPTH_TEST);
	// render everything on screen
	{
		PushFramebuffer(0, ViewportSize);
		glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		AntiAliasingPP->Render();
		PopFramebuffer();
	}

	// HUD
	glDisable(GL_DEPTH_TEST);

	HUD->Render(DeltaTime);

	glEnable(GL_DEPTH_TEST);

	glfwSwapBuffers(Window);
	glfwPollEvents();
}