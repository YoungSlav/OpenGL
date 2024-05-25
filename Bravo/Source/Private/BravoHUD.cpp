#include "BravoHUD.h"

#include "openGL.h"
#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_opengl3.h"

#include <algorithm>
#include "BravoScreen.h"

bool BravoHUD::Initialize_Internal()
{
	SetSize(TargetSize);

	return true;
}

void BravoHUD::AddScreen(std::shared_ptr<BravoScreen> _Screen)
{
	Screens.push_back(_Screen);
	sort(Screens.begin(), Screens.end(), 
			[](std::shared_ptr<BravoScreen> left, std::shared_ptr<BravoScreen> right) -> bool
			{ 
				return left->GetRenderPriority() < right->GetRenderPriority(); 
			});
}

void BravoHUD::RemoveScreen(std::shared_ptr<class BravoScreen> _Screen)
{
	Screens.erase(std::remove(Screens.begin(), Screens.end(), _Screen), Screens.end());
}


void BravoHUD::SetSize(const glm::vec2& _Size)
{
	Size = _Size;
	ModelMatrix = glm::ortho(0.0f, Size.x, Size.y, 0.0f);

	OnHUDResized.Broadcast(Size);
}

void BravoHUD::Render(float DeltaTime)
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	ImGui_ImplOpenGL3_NewFrame();
	ImGui::NewFrame();

	for ( auto screen : Screens )
	{
		screen->Render(DeltaTime);
	}

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}