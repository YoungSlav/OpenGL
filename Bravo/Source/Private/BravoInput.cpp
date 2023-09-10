#include "BravoInput.h"
#include "openGL.h"
#include "BravoEngine.h"

void BravoInput::SetOwnerWindow(GLFWwindow* _Window)
{
	Window = _Window;
	glfwSetCursorPosCallback(Window, BravoInput::SCallbackMouse);
	glfwSetScrollCallback(Window, BravoInput::SCallbackScroll);
}

void BravoInput::SetMouseEnabled(bool bNewMouseEnabled) const
{
	if (bNewMouseEnabled)
		glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	else
		glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void BravoInput::SCallbackScroll(GLFWwindow* window, double xoffset, double yoffset)
{
	std::shared_ptr<BravoEngine> Engine = BravoEngine::GetEngine();
	std::shared_ptr<BravoInput> Input = Engine ? Engine->GetInput() : nullptr;
	if ( Input )
		Input->DeltaMouseScroll += glm::vec2(xoffset, yoffset);
}

void BravoInput::SCallbackMouse(GLFWwindow* window, double xpos, double ypos)
{
	std::shared_ptr<BravoEngine> Engine = BravoEngine::GetEngine();
	std::shared_ptr<BravoInput> Input = Engine ? Engine->GetInput() : nullptr;
	if ( Input )
	{
		Input->DeltaMouseMove += glm::vec2(xpos, ypos) - Input->MousePos;
		Input->MousePos = glm::vec2(xpos, ypos);
	}
}

void BravoInput::SubscribeKey(const BravoKeySubscription& NewSubscription)
{
	bool bPressed = glfwGetKey(Window, NewSubscription.Key) == GLFW_PRESS || glfwGetMouseButton(Window, NewSubscription.Key) == GLFW_PRESS;
	KeysSubscribers.push_back(NewSubscription);
	KeyStates.insert({NewSubscription.Key, bPressed});	
}

void BravoInput::SubscribeMouseScroll(const BravoMouseScrollSubscription& Callback)
{
	MouseScrollSubscribers.push_back(Callback);
}
void BravoInput::SubscribeMousePosition(const BravoMouseMoveSubscription& Callback)
{
	MousePositionSubscribers.push_back(Callback);
}

void BravoInput::UnSubscribeAll(std::shared_ptr<BravoObject> Owner)
{
	for ( int32 i = (int32)KeysSubscribers.size()-1; i >= 0; --i )
	{
		if ( KeysSubscribers[i].Callback.IsBoundTo(Owner.get()) )
			KeysSubscribers.erase(KeysSubscribers.begin()+i);
	}

	UnSubscribeMouseScroll(Owner);
	UnSubscribeMousePosition(Owner);
}
void BravoInput::UnSubscribeKey(int32 Key, std::shared_ptr<BravoObject> Owner)
{
	for ( int32 i = (int32)KeysSubscribers.size()-1; i >= 0; --i )
	{
		if ( KeysSubscribers[i].Key != Key )
			continue;
		if ( KeysSubscribers[i].Callback.IsBoundTo(Owner.get()) )
			KeysSubscribers.erase(KeysSubscribers.begin()+i);
	}
}
void BravoInput::UnSubscribeMouseScroll(std::shared_ptr<BravoObject> Owner)
{
	for ( int32 i = (int32)MouseScrollSubscribers.size()-1; i >= 0; --i )
	{
		if ( MouseScrollSubscribers[i].Callback.IsBoundTo(Owner.get()) )
			MouseScrollSubscribers.erase(MouseScrollSubscribers.begin()+i);
	}
}
void BravoInput::UnSubscribeMousePosition(std::shared_ptr<BravoObject> Owner)
{
	for ( int32 i = (int32)MousePositionSubscribers.size()-1; i >= 0; --i )
	{
		if ( MousePositionSubscribers[i].Callback.IsBoundTo(Owner.get()) )
			MousePositionSubscribers.erase(MousePositionSubscribers.begin()+i);
	}
}

bool BravoInput::GetKeyState(int32 Key) const
{
	return glfwGetKey(Window, Key) == GLFW_PRESS || glfwGetMouseButton(Window, Key) == GLFW_PRESS;
}

void BravoInput::ProcessInput(float DeltaTime)
{
	for ( BravoKeySubscription& listener : KeysSubscribers )
	{
		bool bPressedNow = GetKeyState(listener.Key);
		auto stateIt = KeyStates.find(listener.Key);
		bool bPressedOld = false;
		if ( stateIt != KeyStates.end() )
		{
			bPressedOld = stateIt->second;
			stateIt->second = bPressedNow;
		}

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

	for ( auto it : MousePositionSubscribers )
	{
		it.Callback.Execute(MousePos, DeltaMouseMove, DeltaTime);
	}

	for ( auto it : MouseScrollSubscribers )
	{
		it.Callback.Execute(DeltaMouseScroll, DeltaTime);
	}

	DeltaMouseScroll = glm::vec2(0.0);
	DeltaMouseMove = glm::vec2(0.0);
}

