#pragma once
#include "stdafx.h"
#include "BravoObject.h"

class BravoInputListener
{
	friend class BravoInput;
protected:
	virtual void InputKey(int Key, bool bPressed, float DeltaTime) {}
	virtual void InputMouseMove(float DeltaX, float DeltaY, float DeltaTime) {}
	virtual void InputMouseScroll(float DeltaX, float DeltaY, float DeltaTime) {}
};



class BravoViewport;
class BravoInput : public BravoObject
{
	friend class BravoViewport;

public:
	void SubscribeToKey(int Key, BravoInputListener* Subscriber);
	void SubscribeToMouseMove(BravoInputListener* Subscriber);
	void SubscribeToMouseScroll(BravoInputListener* Subscriber);

	void UnsubscribeToKey(BravoInputListener* Subscriber);
	void UnsubscribeToMouseMove(BravoInputListener* Subscriber);
	void UnsubscribeToMouseScroll(BravoInputListener* Subscriber);

protected:
	void ProcessInput(struct GLFWwindow *window, float DeltaTime);
	void OnMouseScroll(struct GLFWwindow* window, float xoffset, float yoffset, float DeltaTime);
	void OnMouseMove(struct GLFWwindow* window, float xpos, float ypos, float DeltaTime);

private:
	std::map<int, BravoInputListener*> KeyListeners;
	std::vector<BravoInputListener*> MouseMoveListeners;
	std::vector<BravoInputListener*> MouseScrollListeners;

	float OldMouseX = 0.0f;
	float OldMouseY = 0.0f;
	
	bool bInitialized = false;

};
