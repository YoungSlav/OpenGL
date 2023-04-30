#include "BravoPlayer.h"
#include "BravoEngine.h"
#include "BravoInput.h"
#include "openGL.h"

void BravoPlayer::Init()
{
	if ( BravoInput* Input = BravoEngine::GetInstance()->GetInput() )
	{
		Input->SubscribeToKey(GLFW_KEY_W, this);
		Input->SubscribeToKey(GLFW_KEY_S, this);
		Input->SubscribeToKey(GLFW_KEY_A, this);
		Input->SubscribeToKey(GLFW_KEY_D, this);
		Input->SubscribeToKey(GLFW_KEY_E, this);
		Input->SubscribeToKey(GLFW_KEY_Q, this);
		Input->SubscribeToKey(GLFW_MOUSE_BUTTON_LEFT, this);
		Input->SubscribeToKey(GLFW_MOUSE_BUTTON_RIGHT, this);

		Input->SubscribeToMouseMove(this);
		Input->SubscribeToMouseScroll(this);
	}
	Engine->GetViewport()->GetCamera()->AttachTo(this);

	
}

void BravoPlayer::OnDestroy()
{
	if ( BravoInput* Input = BravoEngine::GetInstance()->GetInput() )
	{
		Input->UnsubscribeToKey(this);
		Input->UnsubscribeToMouseMove(this);
		Input->UnsubscribeToMouseScroll(this);
	}

	Engine->GetViewport()->GetCamera()->Detach();
}

void BravoPlayer::Tick(float DeltaTime)
{
	Location += Velocity * MoveSpeed * DeltaTime;
	Velocity = glm::vec3(0.0f, 0.0f, 0.0f);
}

void BravoPlayer::InputKey(int Key, bool bPressed, float DeltaTime)
{
	static const float sensitivity = 2.5f;
	glm::vec3 front;
    front.x = cos(glm::radians(Rotation.y)) * cos(glm::radians(Rotation.z));
    front.y = sin(glm::radians(Rotation.z));
    front.z = sin(glm::radians(Rotation.y)) * cos(glm::radians(Rotation.z));
    front = glm::normalize(front);

	glm::vec3 right = glm::normalize(glm::cross(front, glm::vec3(0.0f, 1.0f, 0.0f)));
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
	
	if( Key == GLFW_KEY_W && bPressed )
		Velocity += sensitivity * front;
	if( Key == GLFW_KEY_S && bPressed )
		Velocity -= sensitivity * front;
	if( Key == GLFW_KEY_A && bPressed )
		Velocity -= sensitivity * right;
	if( Key == GLFW_KEY_D && bPressed )
		Velocity += sensitivity * right;
	if( Key == GLFW_KEY_E && bPressed )
		Velocity += sensitivity * up;
	if( Key == GLFW_KEY_Q && bPressed )
		Velocity -= sensitivity * up;

	if ( Key == GLFW_MOUSE_BUTTON_RIGHT )
	{
		bMouseInput = bPressed;
		BravoEngine::GetInstance()->GetViewport()->SetMouseEnabled(!bPressed);
	}

}

void BravoPlayer::InputMouseMove(float DeltaX, float DeltaY, float DeltaTime)
{
	if ( !bMouseInput ) return;
	static const float sensitivity = 0.1f;
	Rotation.y += DeltaX * sensitivity;
	Rotation.z += DeltaY * sensitivity;

	Rotation.z = glm::clamp(Rotation.z, -89.0f, 89.0f);
}

void BravoPlayer::InputMouseScroll(float DeltaX, float DeltaY, float DeltaTime)
{
	static const float sensitivity = 1.0f;
	int delta = (int)(DeltaY / glm::abs(DeltaY));
	MoveSpeed = glm::clamp(MoveSpeed + sensitivity * delta, MinMoveSpeed, MaxMoveSpeed);
	Log::LogMessage(std::to_string(MoveSpeed));
}