#pragma once
#include "stdafx.h"
#include "BravoActor.h"
#include "BravoInput.h"
#include "BravoCamera.h"

class BravoPlayer : public BravoActor, public BravoTickable, public BravoInputListener
{
public:

protected:
	virtual bool Initialize_Internal() override;
	virtual void OnDestroy() override;

	virtual void Tick(float DeltaTime) override;

	virtual void InputKey(int32 Key, bool bPressed, float DeltaTime) override;
	virtual void InputMouseMove(float DeltaX, float DeltaY, float DeltaTime) override;
	virtual void InputMouseScroll(float DeltaX, float DeltaY, float DeltaTime) override;

private:
	glm::vec3 Velocity = glm::vec3(0.0f, 0.0f, 0.0f);
	float MinMoveSpeed = 0.10f;
	float MaxMoveSpeed = 20.00;
	float MoveSpeed = MaxMoveSpeed / 2;

	bool bMouseInput = false;
};
