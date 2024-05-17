#include "BravoPlayer.h"
#include "BravoEngine.h"
#include "BravoInput.h"
#include "openGL.h"

bool BravoPlayer::Initialize_Internal()
{
	if ( std::shared_ptr<BravoInput> Input = Engine->GetInput() )
	{
		{
		BravoKeySubscription subscription;
		subscription.Key = GLFW_KEY_W;
		subscription.SubscribedType = EKeySubscriptionType::Hold;
		subscription.Callback.BindSP(Self<BravoPlayer>(), &BravoPlayer::OnInput_W);
		Input->SubscribeKey(subscription);
		}
		{
		BravoKeySubscription subscription;
		subscription.Key = GLFW_KEY_S;
		subscription.SubscribedType = EKeySubscriptionType::Hold;
		subscription.Callback.BindSP(Self<BravoPlayer>(), &BravoPlayer::OnInput_S);
		Input->SubscribeKey(subscription);
		}
		{
		BravoKeySubscription subscription;
		subscription.Key = GLFW_KEY_A;
		subscription.SubscribedType = EKeySubscriptionType::Hold;
		subscription.Callback.BindSP(Self<BravoPlayer>(), &BravoPlayer::OnInput_A);
		Input->SubscribeKey(subscription);
		}
		{
		BravoKeySubscription subscription;
		subscription.Key = GLFW_KEY_D;
		subscription.SubscribedType = EKeySubscriptionType::Hold;
		subscription.Callback.BindSP(Self<BravoPlayer>(), &BravoPlayer::OnInput_D);
		Input->SubscribeKey(subscription);
		}
		{
		BravoKeySubscription subscription;
		subscription.Key = GLFW_KEY_E;
		subscription.SubscribedType = EKeySubscriptionType::Hold;
		subscription.Callback.BindSP(Self<BravoPlayer>(), &BravoPlayer::OnInput_E);
		Input->SubscribeKey(subscription);
		}
		{
		BravoKeySubscription subscription;
		subscription.Key = GLFW_KEY_Q;
		subscription.SubscribedType = EKeySubscriptionType::Hold;
		subscription.Callback.BindSP(Self<BravoPlayer>(), &BravoPlayer::OnInput_Q);
		Input->SubscribeKey(subscription);
		}
		{
		BravoKeySubscription subscription;
		subscription.Key = GLFW_MOUSE_BUTTON_RIGHT;
		subscription.SubscribedType = EKeySubscriptionType::Hold | EKeySubscriptionType::Released;
		subscription.Callback.BindSP(Self<BravoPlayer>(), &BravoPlayer::OnInput_MOUSERIGHT);
		Input->SubscribeKey(subscription);
		}
		{
		BravoKeySubscription subscription;
		subscription.Key = GLFW_KEY_ESCAPE;
		subscription.SubscribedType = EKeySubscriptionType::Released;
		subscription.Callback.BindSP(Self<BravoPlayer>(), &BravoPlayer::OnInput_ESCAPE);
		Input->SubscribeKey(subscription);
		}
		{
			Input->OnMouseMoveDelegate.AddSP(Self<BravoPlayer>(), &BravoPlayer::OnMouseMove);
		}
		{
			Input->OnMouseScrollDelegate.AddSP(Self<BravoPlayer>(), &BravoPlayer::OnMouseScroll);
		}
	}
	return true;
}

void BravoPlayer::OnDestroy()
{
	if ( Engine && Engine->GetCamera() )
		Engine->GetCamera()->Detach();
}

void BravoPlayer::Tick(float DeltaTime)
{
	SetLocation(GetLocation() + Velocity * MoveSpeed * DeltaTime);
	Velocity = glm::vec3(0.0f, 0.0f, 0.0f);
}

void BravoPlayer::OnInput_W(bool ButtonState, float DeltaTime)
{
	AdjustVelocity(GetForwardVector_World());
}
void BravoPlayer::OnInput_S(bool ButtonState, float DeltaTime)
{
	AdjustVelocity(-GetForwardVector_World());
}
void BravoPlayer::OnInput_A(bool ButtonState, float DeltaTime)
{
	glm::vec3 right = glm::normalize(glm::cross(BravoMath::upV, GetForwardVector_World()));
	AdjustVelocity(right);
}
void BravoPlayer::OnInput_D(bool ButtonState, float DeltaTime)
{
	glm::vec3 right = glm::normalize(glm::cross(BravoMath::upV, GetForwardVector_World()));
	AdjustVelocity(-right);
}
void BravoPlayer::OnInput_E(bool ButtonState, float DeltaTime)
{
	glm::vec3 up = BravoMath::upV;
	AdjustVelocity(up);
}
void BravoPlayer::OnInput_Q(bool ButtonState, float DeltaTime)
{
	glm::vec3 up = BravoMath::upV;
	AdjustVelocity(-up);
}
void BravoPlayer::AdjustVelocity(const glm::vec3& DeltaVelocity)
{
	Velocity += (DeltaVelocity * InputMoveSensitivity);
}
void BravoPlayer::OnInput_ESCAPE(bool ButtonState, float DeltaTime)
{
	if( Engine )
		Engine->StopGame();
}
void BravoPlayer::OnInput_MOUSERIGHT(bool ButtonState, float DeltaTime)
{
	bMouseInput = ButtonState;
}

void BravoPlayer::OnMouseScroll(const glm::vec2& DeltaScroll, float DeltaTime)
{
	MoveSpeed = glm::clamp(MoveSpeed + InputMouseScrollSensitivity * DeltaScroll.y, MinMoveSpeed, MaxMoveSpeed);
}
void BravoPlayer::OnMouseMove(const glm::vec2& CurrentPosition, const glm::vec2& DeltaMove, float DeltaTime)
{
	if ( !bMouseInput ) return;

	glm::quat currentRotation = GetRotation_World();
	glm::vec3 currentRotationEuler = glm::eulerAngles(currentRotation);
	
	float currentPitch = glm::degrees(currentRotationEuler.x);
	float currentYaw = glm::degrees(currentRotationEuler.y);

	float pitchDelta = DeltaMove.y * InputMouseMoveSensitivity;
	float newPitch = currentPitch + pitchDelta;
	
	const float MaxPitch = 89.0f;
    newPitch = glm::clamp(newPitch, -MaxPitch, MaxPitch);

	glm::quat pitchRotation = glm::angleAxis(glm::radians(newPitch - currentPitch), GetRightVector_World());

	
	float yawDelta = DeltaMove.x * InputMouseMoveSensitivity;
	float newYaw = currentYaw + yawDelta;
	Log::LogMessage(Log::to_string(newYaw));

	glm::quat yawRotation = glm::angleAxis(glm::radians(yawDelta), BravoMath::upV);

	glm::quat newRotation = yawRotation * pitchRotation * currentRotation;

	newRotation = glm::normalize(newRotation);

	SetRotation(newRotation);

	////glm::quat curentRotation = GetRotation();
	////glm::vec3 eulerRotation
	//glm::quat addPitch = glm::quat(glm::radians(DeltaMove.y * InputMouseMoveSensitivity), GetForwardVector());
	//Rotate(addPitch);
	//
	//glm::quat addYaw = glm::quat(glm::radians(DeltaMove.x * InputMouseMoveSensitivity), GetUpVector());
	//Rotate(addYaw);

	//SetRotation(glm::vec3(
	//	GetRotation().x,
	//	GetRotation().y + DeltaMove.x * InputMouseMoveSensitivity,
	//	glm::clamp((GetRotation().z - DeltaMove.y * InputMouseMoveSensitivity), -89.0f, 89.0f))
	//);
}