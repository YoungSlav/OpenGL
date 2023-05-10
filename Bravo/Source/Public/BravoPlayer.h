#pragma once
#include "stdafx.h"
#include "BravoActor.h"
#include "BravoInput.h"
#include "BravoCamera.h"

class BravoPlayer : public BravoActor, public BravoTickable, public BravoInputListener
{
public:
	BravoPlayer(std::shared_ptr<class BravoEngine> _Engine, const BravoHandle& _Handle) :
		BravoActor(_Engine, _Handle),
		BravoTickable(),
		BravoInputListener()
	{}

protected:
	virtual void Initialize_Internal() override;
	virtual void OnDestroy() override;

	virtual void Tick(float DeltaTime) override;

	virtual void InputKey(int32 Key, bool bPressed, float DeltaTime) override;
	virtual void InputMouseMove(float DeltaX, float DeltaY, float DeltaTime) override;
	virtual void InputMouseScroll(float DeltaX, float DeltaY, float DeltaTime) override;

private:
	glm::vec3 Velocity = glm::vec3(0.0f, 0.0f, 0.0f);
	float MinMoveSpeed = 0.10f;
	float MaxMoveSpeed = 5.00;
	float MoveSpeed = MaxMoveSpeed;

	bool bMouseInput = false;
};
