#include "BravoCamera.h"
#include "BravoEngine.h"
#include "BravoMath.h"

void BravoCamera::AttachTo(const std::shared_ptr<BravoActor> InActor)
{
	Owner = InActor;
	bAttachedToActor = true;
}
void BravoCamera::Detach()
{
	bAttachedToActor = false;
}

bool BravoCamera::Initialize_Internal()
{
	UpdateCamera();
	return true;
}

void BravoCamera::UpdateCamera()
{
	if ( bAttachedToActor )
	{
		if ( std::shared_ptr<BravoActor> O = GetOwner() )
		{
			SetLocation(O->GetLocation());
			SetRotation(O->GetRotation());
		}
	}
	UpdateCamera_Internal();
}

void BravoCamera::UpdateCamera_Internal()
{
	glm::vec3 front;
    front.x = cos(glm::radians(Rotation.y)) * cos(glm::radians(Rotation.z));
    front.y = sin(glm::radians(Rotation.z));
    front.z = sin(glm::radians(Rotation.y)) * cos(glm::radians(Rotation.z));
    front = glm::normalize(front);
    // Also re-calculate the Right and Up vector
    glm::vec3 right = glm::normalize(glm::cross(BravoMath::upV, front));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    glm::vec3 up    = glm::normalize(glm::cross(front, right));
	
	ViewMatrix = glm::lookAt(Location, Location + front, up);

	const glm::ivec2 ViewportSize = Engine->GetViewportSize();
	const float AspectRatio = float(ViewportSize.x) / float(ViewportSize.y);

	ProjectionMatrix = glm::perspective(glm::radians(FOV), AspectRatio, MinDrawingDistance, MaxDrawingDistance);
}