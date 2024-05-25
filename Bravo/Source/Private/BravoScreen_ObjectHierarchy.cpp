#include "BravoScreen_ObjectHierarchy.h"
#include "BravoActor.h"
#include "BravoComponent.h"

bool BravoScreen_ObjectHierarchy::Initialize_Internal()
{
	if ( !BravoScreen::Initialize_Internal() || RootObject == nullptr )
		return false;

	SetTrueScaling(false);
	SetSize(glm::vec2(0.2f, 1.0f));
	SetMaxSize(glm::vec2(1.0f, 1.0f));
	SetOrigin(glm::vec2(0.0f, 0.0f));
	SetPosition(glm::vec2(0.0f, 0.0f));

	
	return true;
}

void BravoScreen_ObjectHierarchy::Render_Internal(float DeltaTime)
{
	BravoScreen::Render_Internal(DeltaTime);

	ImGui::SetNextWindowBgAlpha(1.0f);
	ImGui::Begin(GetLabel().c_str(), nullptr,
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoCollapse );
		
		ImGui::BeginChild("ScrollingRegion", ImVec2(0, 0), true, ImGuiWindowFlags_HorizontalScrollbar);
			RenderNode_Recursive(RootObject, 0);
		ImGui::EndChild();

	ImGui::End();
}

void BravoScreen_ObjectHierarchy::RenderNode_Recursive(const std::shared_ptr<class BravoObject> obj, int32 Depth)
{
	std::string lb = obj->GetName() + "##" + std::to_string(GetHandle());
	
	if ( ImGui::TreeNode(lb.c_str()) )
	{
		if (ImGui::IsItemClicked())
		{
			Log::LogMessage(ELog::Log, "Node clicked: {}", obj->GetName());
		}
		const std::list<std::weak_ptr<BravoObject>> Children = obj->GetChildren();
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