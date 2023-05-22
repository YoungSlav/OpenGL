#include "BravoActor.h"
#include "BravoEngine.h"
#include "BravoMath.h"

glm::mat4 BravoActor::GetTransformMatrix() const
{
	glm::mat4 transform;
	transform = glm::translate(transform, Location);
	transform = transform * glm::toMat4(BravoMath::EulerToQuat(Rotation));
	transform = glm::scale(transform, Scale);
	return transform;
}