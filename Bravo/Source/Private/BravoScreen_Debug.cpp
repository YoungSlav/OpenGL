#include "BravoScreen_Debug.h"
#include "BravoAssetManager.h"

bool BravoScreen_Debug::Initialize_Internal()
{
	if ( !BravoScreen::Initialize_Internal() )
		return false;

	SetTrueScaling(false);
	SetSize(glm::vec2(0.1f, 0.08f));
	SetOrigin(glm::vec2(1.0f, 0.0f));
	SetPosition(glm::vec2(1.0f, 0.00f));


	Font = ImGui::GetIO().Fonts->AddFontFromFileTTF((GetAssetManager()->GetResourceFolder() + "Fonts\\arial.ttf").c_str(), 30);
	
	
	return true;
}

void BravoScreen_Debug::Render_Internal(float DeltaTime)
{
	BravoScreen::Render_Internal(DeltaTime);

	const int32 FPS = (int32)(1.0f / DeltaTime);
	const std::string FPSStr = "FPS: " + std::to_string(FPS);

	ImVec4 color;
	if ( FPS >= 59 )
		color = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
	else if ( FPS >= 29 )
		color = ImVec4(1.0f, 1.0f, 0.0f, 1.0f);
	else
		color = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);

	ImGui::Begin(GetLabel().c_str(), nullptr,
		ImGuiWindowFlags_NoBackground |
		ImGuiWindowFlags_NoDecoration |
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoCollapse );

		ImGui::SetWindowFontScale(FontScaling);

		ImGui::PushFont(Font);
		ImGui::PushStyleColor(ImGuiCol_Text, color);
			ImGui::Text(FPSStr.c_str());
		ImGui::PopStyleColor();
		ImGui::PopFont();

	ImGui::End();
}
