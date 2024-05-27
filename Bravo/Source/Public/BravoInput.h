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

	bool CheckType(EKeySubscriptionType Type) const
	{
		return SubscribedType & Type;
	}
	bool CheckType(int32 Types) const
	{
		return SubscribedType & Types;
	}
};



class BravoInput : public BravoObject
{
	struct KeyEvent
	{
		KeyEvent(int32 _Key, EKeySubscriptionType _Event, int32 _Mods) :
			Key(_Key), EventType(_Event), Mods(_Mods),
			bPressedNow(_Event == EKeySubscriptionType::Pressed || _Event == EKeySubscriptionType::Hold)
		{}

		int32 Key = 0;
		int32 Mods = 0;
		EKeySubscriptionType EventType;
		bool bPressedNow;
	};

public:
	template <typename... Args>
	BravoInput(Args&&... args) :
		BravoObject(std::forward<Args>(args)...)
	{}


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

private:

	static void SCallbackMouseScroll(struct GLFWwindow* window, double xoffset, double yoffset);
	static void SCallbackMousePosition(struct GLFWwindow* window, double xpos, double ypos);
	static void SCallbackMouseButton(GLFWwindow* window, int button, int action, int mods);
	static void SCallbackKeyboard(GLFWwindow* window, int key, int scancode, int action, int mods);

private:
	std::vector<BravoKeySubscription> KeysSubscribers;
		
	std::set<int32> PressedKeys;
	std::set<int32> OldPressedStates;


	struct GLFWwindow* Window = nullptr;
	glm::vec2 MousePos = glm::vec2(0.0);
	glm::vec2 DeltaMouseMove = glm::vec2(0.0);
	glm::vec2 DeltaMouseScroll = glm::vec2(0.0);
};