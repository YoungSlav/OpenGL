#include "BravoInput.h"
#include "BravoEngine.h"
#include "openGL.h"

void BravoInput::SubscribeToKey(int Key, BravoInputListener* Subscriber)
{
	KeyListeners.insert( std::pair<int, BravoInputListener*>(Key, Subscriber) );
}
void BravoInput::SubscribeToMouseMove(BravoInputListener* Subscriber)
{
	if ( std::find(MouseMoveListeners.begin(), MouseMoveListeners.end(), Subscriber) == MouseMoveListeners.end() )
		MouseMoveListeners.push_back(Subscriber);
}
void BravoInput::SubscribeToMouseScroll(BravoInputListener* Subscriber)
{
	if ( std::find(MouseScrollListeners.begin(), MouseScrollListeners.end(), Subscriber) == MouseScrollListeners.end() )
		MouseScrollListeners.push_back(Subscriber);
}

void BravoInput::UnsubscribeToKey(BravoInputListener* Subscriber)
{
	for(std::map<int, BravoInputListener*>::iterator it = KeyListeners.begin(); it != KeyListeners.end();)
	{
		if( (it->second) == Subscriber )
			it = KeyListeners.erase(it);
		else
			it++;
	}
}

void BravoInput::UnsubscribeToMouseMove(BravoInputListener* Subscriber)
{
	for ( int i = MouseMoveListeners.size()-1; i >= 0; --i )
	{
		if ( MouseMoveListeners[i] == Subscriber )
			MouseMoveListeners.erase(MouseMoveListeners.begin() + i);
	}
}
void BravoInput::UnsubscribeToMouseScroll(BravoInputListener* Subscriber)
{
	for ( int i = MouseScrollListeners.size()-1; i >= 0; --i )
	{
		if ( MouseScrollListeners[i] == Subscriber )
			MouseScrollListeners.erase(MouseScrollListeners.begin() + i);
	}
}

void BravoInput::ProcessInput(GLFWwindow *window, float DeltaTime)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		Engine->StopGame();
	}
	else
	{
		for ( auto it : KeyListeners )
		{
			bool bPressed = glfwGetKey(window, it.first) == GLFW_PRESS || glfwGetMouseButton(window, it.first) == GLFW_PRESS;
			it.second->InputKey(it.first, bPressed, DeltaTime);
		}
	}

}

void BravoInput::OnMouseScroll(GLFWwindow* window, float xoffset, float yoffset, float DeltaTime)
{
	for ( auto it : MouseScrollListeners )
		it->InputMouseScroll(xoffset, yoffset, DeltaTime);
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
		it->InputMouseMove(deltaX, deltaY, DeltaTime);
}