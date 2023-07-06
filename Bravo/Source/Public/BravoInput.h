#pragma once
#include "stdafx.h"
#include "BravoObject.h"

class BravoInputListener
{
	friend class BravoInput;
protected:
	virtual void InputKey(int32 Key, bool bPressed, float DeltaTime) {}
	virtual void InputMouseMove(float DeltaX, float DeltaY, float DeltaTime) {}
	virtual void InputMouseScroll(float DeltaX, float DeltaY, float DeltaTime) {}
};

class BravoInput : public BravoObject
{
	friend class BravoEngine;
public:
	void SetOwnerWindow(struct GLFWwindow* _Window);

	void SetMouseEnabled(bool bNewMouseEnabled) const;

	void SubscribeToKey(int32 Key, std::shared_ptr<BravoInputListener> Subscriber);
	void SubscribeToMouseMove(std::shared_ptr<BravoInputListener> Subscriber);
	void SubscribeToMouseScroll(std::shared_ptr<BravoInputListener> Subscriber);

	void UnsubscribeFromKey(std::shared_ptr<BravoInputListener> Subscriber);
	void UnsubscribeFromMouseMove(std::shared_ptr<BravoInputListener> Subscriber);
	void UnsubscribeFromMouseScroll(std::shared_ptr<BravoInputListener> Subscriber);

protected:
	void ProcessInput(struct GLFWwindow *window, float DeltaTime);
	void OnMouseScroll(struct GLFWwindow* window, float xoffset, float yoffset, float DeltaTime);
	void OnMouseMove(struct GLFWwindow* window, float xpos, float ypos, float DeltaTime);

private:
	std::map<int32, std::weak_ptr<BravoInputListener>> KeyListeners;
	std::vector<std::weak_ptr<BravoInputListener>> MouseMoveListeners;
	std::vector<std::weak_ptr<BravoInputListener>> MouseScrollListeners;

	float OldMouseX = 0.0f;
	float OldMouseY = 0.0f;
	

	struct GLFWwindow* Window = nullptr;

	bool bInitialized = false;

};
