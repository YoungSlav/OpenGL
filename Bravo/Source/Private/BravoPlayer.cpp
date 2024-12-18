#include "BravoPlayer.h"
#include "BravoEngine.h"
#include "BravoInput.h"
#include "openGL.h"
#include "BravoViewport.h"

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
		BravoKeySubscription subscription;
		subscription.Key = GLFW_KEY_F2;
		subscription.SubscribedType = EKeySubscriptionType::Released;
		subscription.Callback.BindSP(Self<BravoPlayer>(), &BravoPlayer::ToggleWireframe);
		Input->SubscribeKey(subscription);
		}
		{
		BravoKeySubscription subscription;
		subscription.Key = GLFW_KEY_F3;
		subscription.SubscribedType = EKeySubscriptionType::Released;
		subscription.Callback.BindSP(Self<BravoPlayer>(), &BravoPlayer::ToggleCameraMode);
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

void BravoPlayer::ToggleWireframe(bool ButtonState, float DeltaTime)
{
	Engine->GetViewport()->ToggleWireframe();
}

void BravoPlayer::ToggleCameraMode(bool ButtonState, float DeltaTime)
{
	EBravoCameraType NewCameraType = Engine->GetCamera()->GetCameraType() == EBravoCameraType::Orthographic ? 
		EBravoCameraType::Perspective : 
		EBravoCameraType::Orthographic;
	Engine->GetCamera()->SetCameraType(NewCameraType);
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

void BravoPlayer::SetMoveSpeed(float NewSpeed)
{
	MoveSpeed = glm::clamp(NewSpeed, MinMoveSpeed, MaxMoveSpeed);
}
void BravoPlayer::OnMouseMove(const glm::vec2& CurrentPosition, const glm::vec2& DeltaMove, float DeltaTime)
{
	if ( !bMouseInput || glm::all(glm::epsilonEqual(DeltaMove, glm::vec2(0.0f), FLT_EPS)) ) return;

	glm::quat currentRotation = GetRotation_World();
		
	float yawDelta = glm::radians(DeltaMove.x * -InputMouseMoveSensitivity);
	glm::quat yawRotation = glm::angleAxis(yawDelta, BravoMath::upV);
	
	float pitchDelta = glm::radians(DeltaMove.y * -InputMouseMoveSensitivity);
	glm::quat pitchRotation = glm::normalize(glm::angleAxis(pitchDelta, GetRightVector_World()));

	glm::quat newRotation = glm::normalize(pitchRotation * yawRotation * currentRotation);

	const glm::vec3 newCameraDirection = BravoMath::QuaternionToDirection(newRotation);
	const float MaxPitch = 89.0f;
	const float maxDot = glm::abs(glm::cos(glm::radians(MaxPitch)));
	const float newDot = 1.0f - glm::abs(glm::dot(newCameraDirection, BravoMath::upV));

	if ( newDot <= maxDot )
	{
		const glm::vec3 currentCameraDirection = BravoMath::QuaternionToDirection(currentRotation);
		const float currentDot = 1.0f - glm::abs(glm::dot(currentCameraDirection, BravoMath::upV));

		if ( newDot <= currentDot )
		{
			//Log::LogMessage(ELog::Error, "SHOULD RESTRICT, {} > {}", glm::degrees(glm::acos(newDot)), glm::degrees(glm::acos(currentDot)));
			newRotation = glm::normalize(yawRotation * currentRotation);

			// TODO: clamp instead of restriction (need to project direction vector onto restriction "cone")
		}
	}
	else
	{
		//Log::LogMessage(ELog::Success, "No restriction, {} <= {}", glm::degrees(glm::acos(newDot)), glm::degrees(glm::acos(maxDot)));
	}
	SetRotation(newRotation);
}