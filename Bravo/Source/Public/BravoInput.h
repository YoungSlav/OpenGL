#pragma once
#include "stdafx.h"
#include "BravoObject.h"

enum EKeySubscriptionType : int32
{
	Pressed = 0x01,
	Hold = 0x02,
	Released = 0x04,

	Any = 0x07
};

typedef Delegate<void, bool, float> OnKeyInputSignature;
typedef MulticastDelegate<const glm::vec2&, float> OnMouseScrollSignature;
typedef MulticastDelegate<const glm::vec2&, const glm::vec2&, float> OnMouseMoveSignature;

struct BravoKeySubscription
{
	int32 Key = 0;
	int32 SubscribedType = EKeySubscriptionType::Any;
	OnKeyInputSignature Callback;

	bool CheckType(EKeySubscriptionType Type)
	{
		return SubscribedType & Type;
	}
	bool CheckType(int32 Types)
	{
		return SubscribedType & Types;
	}
};

class BravoInput : public BravoObject
{
public:
	void SetOwnerWindow(struct GLFWwindow* _Window);
	void SetMouseEnabled(bool bNewMouseEnabled) const;
	void ProcessInput(float DeltaTime);

	bool GetKeyState(int32 Key) const;
	const glm::vec2& GetMousePosition() const { return MousePos; }

	void SubscribeKey(const BravoKeySubscription& NewSubscription);
	OnMouseScrollSignature OnMouseScrollDelegate;
	OnMouseMoveSignature OnMouseMoveDelegate;

	void UnSubscribeAll(std::shared_ptr<BravoObject> Owner);
	void UnSubscribeKey(int32 Key, std::shared_ptr<BravoObject> Owner);
	void UnSubscribeMouseScroll(std::shared_ptr<BravoObject> Owner);
	void UnSubscribeMousePosition(std::shared_ptr<BravoObject> Owner);

private:
	static void SCallbackScroll(struct GLFWwindow* window, double xoffset, double yoffset);
	static void SCallbackMouse(struct GLFWwindow* window, double xpos, double ypos);

private:
	std::vector<BravoKeySubscription> KeysSubscribers;

	std::map<int32, bool> KeyStates;

	struct GLFWwindow* Window = nullptr;
	glm::vec2 MousePos = glm::vec2(0.0);
	glm::vec2 DeltaMouseMove = glm::vec2(0.0);
	glm::vec2 DeltaMouseScroll = glm::vec2(0.0);
};