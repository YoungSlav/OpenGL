#include "BravoInput.h"
#include "openGL.h"


void BravoInput::SetOwnerWindow(GLFWwindow* _Window)
{
	Window = _Window;
}

void BravoInput::SetMouseEnabled(bool bNewMouseEnabled) const
{
	if ( bNewMouseEnabled )
		glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	else
		glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void BravoInput::SubscribeToKey(int32 Key, std::shared_ptr<BravoInputListener> Subscriber)
{
	KeyListeners.insert( {Key, Subscriber} );
}
void BravoInput::SubscribeToMouseMove(std::shared_ptr<BravoInputListener> Subscriber)
{
	MouseMoveListeners.push_back(Subscriber);
}
void BravoInput::SubscribeToMouseScroll(std::shared_ptr<BravoInputListener> Subscriber)
{
	MouseScrollListeners.push_back(Subscriber);
}

void BravoInput::UnsubscribeFromKey(std::shared_ptr<BravoInputListener> Subscriber)
{

	for (auto it = KeyListeners.cbegin(); it != KeyListeners.cend(); )
	{
		if ( it->second.expired() || it->second.lock() == Subscriber)
			it = KeyListeners.erase(it);
		else
			++it;
	}
}

void BravoInput::UnsubscribeFromMouseMove(std::shared_ptr<BravoInputListener> Subscriber)
{
	for ( size_t i = MouseMoveListeners.size()-1; i >= 0; --i )
	{
		if ( MouseMoveListeners[i].expired() || MouseMoveListeners[i].lock() == Subscriber )
			MouseMoveListeners.erase(MouseMoveListeners.begin() + i);
	}
}
void BravoInput::UnsubscribeFromMouseScroll(std::shared_ptr<BravoInputListener> Subscriber)
{
	for ( size_t i = MouseScrollListeners.size()-1; i >= 0; --i )
	{
		if ( MouseScrollListeners[i].expired() || MouseScrollListeners[i].lock() == Subscriber )
			MouseScrollListeners.erase(MouseScrollListeners.begin() + i);
	}
}

void BravoInput::ProcessInput(GLFWwindow *window, float DeltaTime)
{
	for (auto it = KeyListeners.cbegin(); it != KeyListeners.cend(); )
	{
		if ( !it->second.expired() )
		{
			bool bPressed = glfwGetKey(window, it->first) == GLFW_PRESS || glfwGetMouseButton(window, it->first) == GLFW_PRESS;
			it->second.lock()->InputKey(it->first, bPressed, DeltaTime);
			++it;
		}
		else
		{
			it = KeyListeners.erase(it);
		}
	}
}

void BravoInput::OnMouseScroll(GLFWwindow* window, float xoffset, float yoffset, float DeltaTime)
{
	for ( auto it : MouseScrollListeners )
	{
		if ( !it.expired() )
			it.lock()->InputMouseScroll(xoffset, yoffset, DeltaTime);
	}
}

void BravoInput::OnMouseMove(GLFWwindow* window, float xpos, float ypos, float DeltaTime)
{
	if ( !bInitialized )
	{
		OldMouseX = xpos;
		OldMouseY = ypos;
		bInitialized = true;
		return;
	}
	
	float deltaX = xpos - OldMouseX;
	float deltaY = OldMouseY - ypos;

	OldMouseX = xpos;
	OldMouseY = ypos;

	for ( auto it : MouseMoveListeners )
	{
		if ( !it.expired() )
			it.lock()->InputMouseMove(deltaX, deltaY, DeltaTime);
	}
}