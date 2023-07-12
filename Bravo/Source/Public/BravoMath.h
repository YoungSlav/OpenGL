#pragma once

#include "stdafx.h"

class BravoMath
{
public:
	static glm::vec3 DirectionToRotation(const glm::vec3& Direction);
	static glm::quat DirectionToQuaternion(const glm::vec3& Direction);

	static glm::vec3 RotationToDirection(const glm::vec3& Rotation);
	static glm::vec3 QuaternionToDirection(const glm::quat& Quaternion);

	static glm::vec3 QuatToEuler(const glm::quat& QuatRotation);
	static glm::quat EulerToQuat(const glm::vec3& EulerRotation);

	static glm::vec3 RandVector(float MaxDimension);

	static const glm::vec3 upV;
	static const glm::vec3 forwardV;
	static const glm::vec3 rightV;
};
