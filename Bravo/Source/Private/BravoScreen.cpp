#include "BravoScreen.h"
#include "BravoEngine.h"
#include "BravoViewport.h"
#include "BravoWidget.h"
#include "BravoHUD.h"

void BravoScreen::OnDestroy()
{
	BravoObject::OnDestroy();

	if ( !GetHUD() )
		return;

	GetHUD()->RemoveScreen(Self<BravoScreen>());
}

void BravoScreen::Render_Internal(float DeltaTime)
{
	const glm::vec4 Bounds = GetBounds();
	ImGui::SetNextWindowPos(ImVec2(Bounds.x, Bounds.y), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(Bounds.z, Bounds.w), ImGuiCond_Always);
	//ImGui::SetNextWindowBgAlpha(0.0f);
	ImGui::SetNextWindowSizeConstraints(ImVec2(Bounds.z, Bounds.w), ImVec2(Bounds.z, Bounds.w));
	std::string Label = GetName() + "##" + std::to_string(GetHandle());

	FontScaling = bScaleFonts ? GetHUD()->GetTargetScale().y : 1.0f;
}

glm::vec4 BravoScreen::GetBounds() const
{
	const glm::vec2 hudSize = GetHUD()->GetSize();
	const glm::vec2 trueSize = bTrueScaling ?
		Size * GetHUD()->GetSize() :
		Size * GetHUD()->GetTargetSize() * GetHUD()->GetTargetScale().y;

	
	const glm::vec2 pos = Position * hudSize - Origin * trueSize;
	
	return glm::vec4(pos, trueSize);
}
