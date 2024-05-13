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

bool BravoMath::IsNearlyZero(float Number, double Tolerance)
{
	return std::abs(Number) <= Tolerance;
}
bool BravoMath::IsNearlyZero(double Number, double Tolerance)
{
	return std::abs(Number) <= Tolerance;
}
bool BravoMath::IsNearlyZero(const glm::vec3& vector, double Tolerance)
{
	return std::abs(vector.length()) <= Tolerance;
}

float BravoMath::MaxComponent(const glm::vec3& Vector)
{
	if ( std::abs(Vector.x) > std::abs(Vector.y) && std::abs(Vector.x) > std::abs(Vector.z) )
		return Vector.x;
	if ( std::abs(Vector.y) > std::abs(Vector.x) && std::abs(Vector.y) > std::abs(Vector.z) )
		return Vector.y;
	if ( std::abs(Vector.z) > std::abs(Vector.y) && std::abs(Vector.z) > std::abs(Vector.x) )
		return Vector.z;
	return Vector.z;
}

void BravoMath::GetFrustumCornersWorldSpace(const glm::mat4& frustrum, std::vector<glm::vec4>& OutFrustumCorners)
{
	OutFrustumCorners.clear();
	OutFrustumCorners.reserve(8);

	const auto inv = glm::inverse(frustrum);
	for (int32 x = 0; x < 2; ++x)
	{
		for (int32 y = 0; y < 2; ++y)
		{
			for (int32 z = 0; z < 2; ++z)
			{
				const glm::vec4 pt = 
					inv * glm::vec4(
						2.0f * x - 1.0f,
						2.0f * y - 1.0f,
						2.0f * z - 1.0f,
						1.0f);
				OutFrustumCorners.push_back(pt / pt.w);
			}
		}
	}
}