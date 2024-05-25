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
	ImGui::SetNextWindowSize(ImVec2(Bounds.z, Bounds.w), ImGuiCond_Once);
	//ImGui::SetNextWindowBgAlpha(0.0f);

	if ( !BravoMath::IsNearlyZero(MaxSize) )
	{
		const glm::vec2 maxSize = bTrueScaling ?
			MaxSize * GetHUD()->GetSize() :
			MaxSize * GetHUD()->GetTargetSize() * GetHUD()->GetTargetScale().y;
		ImGui::SetNextWindowSizeConstraints(ImVec2(Bounds.z, Bounds.w), ImVec2(maxSize.x, maxSize.y));
	}
	
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
