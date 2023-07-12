#include "BravoPlayer.h"
#include "BravoEngine.h"
#include "BravoInput.h"
#include "openGL.h"

bool BravoPlayer::Initialize_Internal()
{
	if ( std::shared_ptr<BravoInput> Input = Engine->GetInput() )
	{
		Input->SubscribeToKey(GLFW_KEY_W, Self<BravoPlayer>());
		Input->SubscribeToKey(GLFW_KEY_S, Self<BravoPlayer>());
		Input->SubscribeToKey(GLFW_KEY_A, Self<BravoPlayer>());
		Input->SubscribeToKey(GLFW_KEY_D, Self<BravoPlayer>());
		Input->SubscribeToKey(GLFW_KEY_E, Self<BravoPlayer>());
		Input->SubscribeToKey(GLFW_KEY_Q, Self<BravoPlayer>());
		Input->SubscribeToKey(GLFW_MOUSE_BUTTON_LEFT, Self<BravoPlayer>());
		Input->SubscribeToKey(GLFW_MOUSE_BUTTON_RIGHT, Self<BravoPlayer>());

		Input->SubscribeToKey(GLFW_KEY_ESCAPE, Self<BravoPlayer>());

		Input->SubscribeToMouseMove(Self<BravoPlayer>());
		Input->SubscribeToMouseScroll(Self<BravoPlayer>());
	}
	return true;
}

void BravoPlayer::OnDestroy()
{
	if ( std::shared_ptr<BravoInput> Input = Engine->GetInput() )
	{
		Input->UnsubscribeFromKey(Self<BravoPlayer>());
		Input->UnsubscribeFromMouseMove(Self<BravoPlayer>());
		Input->UnsubscribeFromMouseScroll(Self<BravoPlayer>());
	}

	if ( Engine && Engine->GetCamera() )
		Engine->GetCamera()->Detach();
}

void BravoPlayer::Tick(float DeltaTime)
{
	SetLocation(GetLocation() + Velocity * MoveSpeed * DeltaTime);
	Velocity = glm::vec3(0.0f, 0.0f, 0.0f);
}

void BravoPlayer::InputKey(int32 Key, bool bPressed, float DeltaTime)
{
	static const float sensitivity = 2.5f;
	glm::vec3 front;
    front.x = cos(glm::radians(GetRotation().y)) * cos(glm::radians(GetRotation().z));
    front.y = sin(glm::radians(GetRotation().z));
    front.z = sin(glm::radians(GetRotation().y)) * cos(glm::radians(GetRotation().z));
    front = glm::normalize(front);

	glm::vec3 right = glm::normalize(glm::cross(BravoMath::upV, front));
	glm::vec3 up = BravoMath::upV;
	
	if( Key == GLFW_KEY_W && bPressed )
		Velocity += sensitivity * front;
	if( Key == GLFW_KEY_S && bPressed )
		Velocity -= sensitivity * front;
	if( Key == GLFW_KEY_A && bPressed )
		Velocity += sensitivity * right;
	if( Key == GLFW_KEY_D && bPressed )
		Velocity -= sensitivity * right;
	if( Key == GLFW_KEY_E && bPressed )
		Velocity += sensitivity * up;
	if( Key == GLFW_KEY_Q && bPressed )
		Velocity -= sensitivity * up;

	if ( Key == GLFW_KEY_ESCAPE && bPressed )
	{
		if( Engine )
			Engine->StopGame();
	}

	if ( Key == GLFW_MOUSE_BUTTON_RIGHT )
	{
		bMouseInput = bPressed;
		//BravoEngine::GetInstance()->GetViewport()->SetMouseEnabled(!bPressed);
	}

}

void BravoPlayer::InputMouseMove(float DeltaX, float DeltaY, float DeltaTime)
{
	if ( !bMouseInput ) return;
	static const float sensitivity = 0.1f;
	SetRotation(glm::vec3(
		GetRotation().x,
		GetRotation().y + DeltaX * sensitivity,
		glm::clamp((GetRotation().z + DeltaY * sensitivity), -89.0f, 89.0f))
	);
}

void BravoPlayer::InputMouseScroll(float DeltaX, float DeltaY, float DeltaTime)
{
	static const float sensitivity = 1.0f;
	int32 delta = (int32)(DeltaY / glm::abs(DeltaY));
	MoveSpeed = glm::clamp(MoveSpeed + sensitivity * delta, MinMoveSpeed, MaxMoveSpeed);
	Log::LogMessage(std::to_string(MoveSpeed));
}