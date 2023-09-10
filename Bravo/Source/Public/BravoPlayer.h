#pragma once
#include "stdafx.h"
#include "BravoActor.h"
#include "BravoCamera.h"

class BravoPlayer : public BravoActor, public BravoTickable
{
public:

protected:
	virtual bool Initialize_Internal() override;
	virtual void OnDestroy() override;

	virtual void Tick(float DeltaTime) override;

	void OnInput_W(bool ButtonState, float DeltaTime);
	void OnInput_S(bool ButtonState, float DeltaTime);
	void OnInput_A(bool ButtonState, float DeltaTime);
	void OnInput_D(bool ButtonState, float DeltaTime);
	void OnInput_E(bool ButtonState, float DeltaTime);
	void OnInput_Q(bool ButtonState, float DeltaTime);
	void OnInput_ESCAPE(bool ButtonState, float DeltaTime);
	void OnInput_MOUSERIGHT(bool ButtonState, float DeltaTime);

	void OnMouseScroll(const glm::vec2& DeltaScroll, float DeltaTime);
	void OnMouseMove(const glm::vec2& CurrentPosition, const glm::vec2& DeltaMove, float DeltaTime);

	void AdjustVelocity(const glm::vec3& DeltaVelocity);

private:
	glm::vec3 Velocity = glm::vec3(0.0f, 0.0f, 0.0f);
	float MinMoveSpeed = 0.10f;
	float MaxMoveSpeed = 20.00;

	const float InputMoveSensitivity = 2.5f;
	const float InputMouseMoveSensitivity = 0.1f;
	const float InputMouseScrollSensitivity = 0.1f;

	float MoveSpeed = MaxMoveSpeed / 2;

	bool bMouseInput = false;
};
