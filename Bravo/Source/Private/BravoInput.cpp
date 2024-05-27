#include "BravoInput.h"
#include "openGL.h"
#include "BravoEngine.h"
#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_opengl3.h"

void BravoInput::SetOwnerWindow(GLFWwindow* _Window)
{
	Window = _Window;
	glfwSetMouseButtonCallback(Window, BravoInput::SCallbackMouseButton);
	glfwSetCursorPosCallback(Window, BravoInput::SCallbackMousePosition);
	glfwSetScrollCallback(Window, BravoInput::SCallbackMouseScroll);
	glfwSetKeyCallback(Window, BravoInput::SCallbackKeyboard);
}

void BravoInput::SetMouseEnabled(bool bNewMouseEnabled) const
{
	if (bNewMouseEnabled)
		glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	else
		glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void BravoInput::SCallbackMouseScroll(GLFWwindow* window, double xoffset, double yoffset)
{
	ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);
	if ( ImGui::GetIO().WantCaptureMouse )
		return;

	std::shared_ptr<BravoEngine> Engine = BravoEngine::GetEngine();
	std::shared_ptr<BravoInput> Input = Engine ? Engine->GetInput() : nullptr;
	if ( Input )
	{
		Input->DeltaMouseScroll = glm::vec2(xoffset, yoffset);
	}
}

void BravoInput::SCallbackMousePosition(GLFWwindow* window, double xpos, double ypos)
{
	ImGui_ImplGlfw_CursorPosCallback(window, xpos, ypos);
	if ( ImGui::GetIO().WantCaptureMouse )
		return;

	std::shared_ptr<BravoEngine> Engine = BravoEngine::GetEngine();
	std::shared_ptr<BravoInput> Input = Engine ? Engine->GetInput() : nullptr;
	if ( Input )
	{
		Input->DeltaMouseMove += glm::vec2(xpos, ypos) - Input->MousePos;
		Input->MousePos = glm::vec2(xpos, ypos);
	}
}

void BravoInput::SCallbackMouseButton(GLFWwindow* window, int32 key, int32 action, int32 mods)
{
	ImGui_ImplGlfw_MouseButtonCallback(window, key, action, mods);
	if ( ImGui::GetIO().WantCaptureMouse )
		action = GLFW_RELEASE;

	std::shared_ptr<BravoEngine> Engine = BravoEngine::GetEngine();
	std::shared_ptr<BravoInput> Input = Engine ? Engine->GetInput() : nullptr;
	
	if ( Input )
	{
		if ( action == GLFW_PRESS || action == GLFW_REPEAT )
			Input->PressedKeys.insert(key);
		else
			Input->PressedKeys.erase(key);
	}
}



void BravoInput::SCallbackKeyboard(GLFWwindow* window, int32 key, int32 scancode, int32 action, int32 mods)
{
	ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
	if ( ImGui::GetIO().WantCaptureKeyboard )
		action = GLFW_RELEASE;

	std::shared_ptr<BravoEngine> Engine = BravoEngine::GetEngine();
	std::shared_ptr<BravoInput> Input = Engine ? Engine->GetInput() : nullptr;
	if ( Input )
	{
		if ( action == GLFW_PRESS || action == GLFW_REPEAT )
			Input->PressedKeys.insert(key);
		else
			Input->PressedKeys.erase(key);

		//Log::LogMessage(ELog::Log, "{} {}", key, action);
	}
}

void BravoInput::SubscribeKey(const BravoKeySubscription& NewSubscription)
{
	KeysSubscribers.push_back(NewSubscription);
}

void BravoInput::UnSubscribeAll(std::shared_ptr<BravoObject> Owner)
{
	for (auto it = KeysSubscribers.begin(); it != KeysSubscribers.end();) 
	{
		if ( it->Callback.IsBoundTo(Owner.get()) )
			it = KeysSubscribers.erase(it);
		else 
			++it;
	}
	OnMouseScrollDelegate.RemoveObject(Owner.get());
	OnMouseMoveDelegate.RemoveObject(Owner.get());

}
void BravoInput::UnSubscribeKey(int32 Key, std::shared_ptr<BravoObject> Owner)
{
	for (auto it = KeysSubscribers.begin(); it != KeysSubscribers.end();) 
	{
		if ( Key == it->Key && it->Callback.IsBoundTo(Owner.get()) ) 
			it = KeysSubscribers.erase(it);
		else 
			++it;
	}
}

bool BravoInput::GetKeyState(int32 Key) const
{
	return PressedKeys.find(Key) != PressedKeys.end();
}

void BravoInput::ProcessInput(float DeltaTime)
{
	for ( BravoKeySubscription& listener : KeysSubscribers )
	{
		bool bPressedNow = PressedKeys.find(listener.Key) != PressedKeys.end();
		bool bPressedOld = OldPressedStates.find(listener.Key) != OldPressedStates.end();

		if ( !bPressedNow && bPressedOld )
		{
			// button released
			if ( listener.CheckType(EKeySubscriptionType::Released) )
			{
				// user subscribed to button being released
				listener.Callback.Execute(bPressedNow, DeltaTime);
			}
		}
		else if ( bPressedNow && !bPressedOld )
		{
			// button pressed
			if ( listener.CheckType(EKeySubscriptionType::Pressed | EKeySubscriptionType::Hold) )
			{
				// user subscribed to button being pressed or hold
				listener.Callback.Execute(bPressedNow, DeltaTime);
			}
		}
		else if( bPressedNow && bPressedOld )
		{
			// button hold
			if ( listener.CheckType(EKeySubscriptionType::Hold) )
			{
				// user subscribed to button being hold
				listener.Callback.Execute(bPressedNow, DeltaTime);
			}
		}
	}
	OldPressedStates = PressedKeys;

	
	if ( !BravoMath::IsNearlyZero(DeltaMouseMove) )
	{
		OnMouseMoveDelegate.Broadcast(MousePos, DeltaMouseMove, DeltaTime);
	}

	if ( !BravoMath::IsNearlyZero(DeltaMouseScroll) )
	{
		OnMouseScrollDelegate.Broadcast(DeltaMouseScroll, DeltaTime);
	}

	DeltaMouseScroll = glm::vec2(0.0);
	DeltaMouseMove = glm::vec2(0.0);
}

