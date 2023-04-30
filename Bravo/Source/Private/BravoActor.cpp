#include "BravoActor.h"
#include "BravoEngine.h"
#include "BravoMath.h"

glm::mat4 BravoActor::TransformModelMatrix() const
{
	glm::mat4 model;
	model = glm::translate(model, Location);
	model = model * glm::toMat4(BravoMath::EulerToQuat(Rotation));
	model = glm::scale(model, Scale);
	return model;
}