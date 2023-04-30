#include "BravoCamera.h"
#include "BravoEngine.h"
#include "BravoMath.h"

void BravoCamera::SetActive()
{
	if ( Engine->GetViewport() )
		Engine->GetViewport()->SetCamera(this);
}

void BravoCamera::AttachTo(const BravoActor* InActor)
{
	Owner = InActor;
	bAttachedToActor = true;
}
void BravoCamera::Detach()
{
	Owner = nullptr;
	bAttachedToActor = false;
}

void BravoCamera::Init()
{
	CalcCamera();
}

void BravoCamera::Tick(float DeltaTime)
{
	if ( bAttachedToActor )
	{
		SetLocation(Owner->GetLocation());
		SetRotation(Owner->GetRotation());
	}
	CalcCamera();
}

void BravoCamera::CalcCamera()
{
	const glm::vec3 ProjectionLocation = Location;


	glm::vec3 front;
    front.x = cos(glm::radians(Rotation.y)) * cos(glm::radians(Rotation.z));
    front.y = sin(glm::radians(Rotation.z));
    front.z = sin(glm::radians(Rotation.y)) * cos(glm::radians(Rotation.z));
    front = glm::normalize(front);
    // Also re-calculate the Right and Up vector
    glm::vec3 right = glm::normalize(glm::cross(front, BravoMath::upV));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    glm::vec3 up    = glm::normalize(glm::cross(right, front));

	ProjectionMatrix = glm::perspective(glm::radians(FOV), AspectRatio, MinDrawingDistance, MaxDrawingDistance);
	ViewMatrix = glm::lookAt(ProjectionLocation, ProjectionLocation + front, up);
}