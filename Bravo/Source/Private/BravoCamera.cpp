#include "BravoCamera.h"
#include "BravoEngine.h"
#include "BravoViewport.h"
#include "BravoMath.h"

void BravoCamera::AttachTo(const std::shared_ptr<BravoActor> InActor)
{
	SetParent(std::dynamic_pointer_cast<IBravoTransformable>(InActor));
}
void BravoCamera::Detach()
{
	SetParent(nullptr);
}

bool BravoCamera::Initialize_Internal()
{
	UpdateCamera();
	return true;
}

void BravoCamera::UpdateCamera()
{
	UpdateCamera_Internal();
}

void BravoCamera::UpdateCamera_Internal()
{
	ViewMatrix = glm::lookAt(GetLocation_World(), GetLocation_World() + GetForwardVector_World(), BravoMath::upV);

	const glm::ivec2 ViewportSize = Engine->GetViewport()->GetViewportSize();
	const float AspectRatio = ViewportSize.y > 0.0f ? float(ViewportSize.x) / float(ViewportSize.y) : 0.0f;

	PerspectiveMatrix = glm::perspective(glm::radians(FOV), AspectRatio, MinDrawingDistance, MaxDrawingDistance);

	float orthoLeft = -ViewportSize.x / 2.0f;
	float orthoRight = ViewportSize.x / 2.0f;
	float orthoBottom = -ViewportSize.y / 2.0f;
	float orthoTop = ViewportSize.y / 2.0f;
}