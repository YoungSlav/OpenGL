#include "BravoScreen_ObjectHierarchy.h"
#include "BravoActor.h"
#include "BravoComponent.h"
#include "BravoInput.h"
#include "BravoEngine.h"

#include "BravoScreen_ObjectProperties.h"
#include "BravoHUD.h"
#include "BravoViewport.h"

bool BravoScreen_ObjectHierarchy::Initialize_Internal()
{
	if ( !BravoScreen::Initialize_Internal() || RootObject == nullptr )
		return false;

	SetTrueScaling(false);
	SetSize(glm::vec2(0.4f, 1.0f));
	SetMaxSize(glm::vec2(1.0f, 1.0f));
	SetOrigin(glm::vec2(0.0f, 0.0f));
	SetPosition(glm::vec2(0.0f, 0.0f));

	if ( Engine->GetInput() )
	{
		BravoKeySubscription sub;
		sub.Key = GLFW_KEY_GRAVE_ACCENT;
		sub.SubscribedType = EKeySubscriptionType::Released;
		sub.Callback.BindSP(Self<BravoScreen_ObjectHierarchy>(), &BravoScreen_ObjectHierarchy::OnToggleHUD);
		Engine->GetInput()->SubscribeKey(sub);
	}
	
	return true;
}


void BravoScreen_ObjectHierarchy::OnToggleHUD(bool ButtonState, float DeltaTime)
{
	bShowHUD = !bShowHUD;
}

void BravoScreen_ObjectHierarchy::Render_Internal(float DeltaTime)
{
	if ( !bShowHUD )
		return;

	BravoScreen::Render_Internal(DeltaTime);
	ImGui::SetNextWindowBgAlpha(1.0f);
	ImGui::Begin(GetLabel().c_str(), nullptr,
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoCollapse);
		
		ImGui::BeginChild("ScrollingRegion", ImVec2(0, 0), true, ImGuiWindowFlags_HorizontalScrollbar);
			RenderNode_Recursive(RootObject, 0);
		ImGui::EndChild();

	ImGui::End();
}

void BravoScreen_ObjectHierarchy::RenderNode_Recursive(const std::shared_ptr<class BravoObject> obj, int32 Depth)
{
	std::string lb = obj->GetName() + "##" + std::to_string(GetHandle());
	
	const std::list<std::weak_ptr<BravoObject>>& Children = obj->GetChildren();
	
	ImGuiTreeNodeFlags NodeFlags = ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_OpenOnArrow;
	
	if (Depth == 0) NodeFlags |= ImGuiTreeNodeFlags_DefaultOpen;
	if ( Children.size() == 0) NodeFlags |= ImGuiTreeNodeFlags_Leaf;

	bool isOpen = ImGui::TreeNodeEx(lb.c_str(), NodeFlags);
	bool isClicked = ImGui::IsItemClicked();

	
	if (isClicked)
	{
		Log::LogMessage(ELog::Log, "Node clicked: {}", obj->GetName());
		if ( Engine->GetViewport()->GetHUD() )
		{
			auto propertiesScreen = NewObject<BravoScreen_ObjectProperties>("Properties Screen", obj);
			Engine->GetViewport()->GetHUD()->AddScreen(propertiesScreen);
		}
	}
	if ( isOpen )
	{
		for ( const std::weak_ptr<BravoObject>& childIt : Children )
		{
			const std::shared_ptr<BravoObject> child = childIt.expired() ? nullptr : childIt.lock();
			if ( !child )
				continue;

			bool bSkip = true;
			if ( const std::shared_ptr<BravoActor> asActor = std::dynamic_pointer_cast<BravoActor>(child) )
				bSkip = false;
			else if ( const std::shared_ptr<BravoComponent> asComp = std::dynamic_pointer_cast<BravoComponent>(child) )
				bSkip = false;
			if ( bSkip )
				continue;

			RenderNode_Recursive(child, ++Depth);
		}

		ImGui::TreePop();
	}
}