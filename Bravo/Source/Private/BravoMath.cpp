#include "BravoMath.h"

const glm::vec3 BravoMath::forwardV = glm::vec3(1.0, 0.0, 0.0);
const glm::vec3 BravoMath::upV = glm::vec3(0.0, 1.0, 0.0);
const glm::vec3 BravoMath::rightV = glm::vec3(0.0, 0.0, 1.0);

glm::vec3 BravoMath::DirectionToRotation(const glm::vec3& Direction)
{
	return QuatToEuler(glm::rotation(BravoMath::forwardV, glm::normalize(Direction)));
}

glm::quat BravoMath::DirectionToQuaternion(const glm::vec3& Direction)
{
	return glm::rotation(BravoMath::forwardV, glm::normalize(Direction));
}

glm::vec3 BravoMath::RotationToDirection(const glm::vec3& Rotation)
{
	glm::mat4 model = glm::toMat4(EulerToQuat(Rotation));
	glm::vec3 rotation = glm::normalize(glm::vec3(model * glm::vec4(BravoMath::forwardV, 1.0f)));
	return rotation;
}


glm::vec3 BravoMath::QuaternionToDirection(const glm::quat& Quaternion)
{
	glm::mat4 model = glm::toMat4(Quaternion);
	glm::vec3 rotation = glm::normalize(glm::vec3(model * glm::vec4(BravoMath::forwardV, 1.0f)));
	return rotation;
}

glm::vec3 BravoMath::QuatToEuler(const glm::quat& QuatRotation)
{
	return glm::degrees(glm::eulerAngles(QuatRotation));
}

glm::quat BravoMath::EulerToQuat(const glm::vec3& EulerRotation)
{
	return glm::quat(glm::radians(EulerRotation));
}

glm::vec3 BravoMath::RandVector(float MaxDimension)
{
	int32 iMaxDimension = (int32)MaxDimension;
	glm::vec3 Result(
		((rand() % iMaxDimension*2) - iMaxDimension),
		((rand() % iMaxDimension*2) - iMaxDimension),
		((rand() % iMaxDimension*2) - iMaxDimension)
	);
	return Result;
}