#include "BravoMath.h"

const glm::vec3 BravoMath::forwardV = glm::vec3(0, 0, -1.0);
const glm::vec3 BravoMath::upV = glm::vec3(0.0, 1.0, 0.0);
const glm::vec3 BravoMath::rightV = glm::vec3(1.0, 0.0, 0.0);

glm::vec3 BravoMath::NormalizeEuler(const glm::vec3& euler)
{
	// Convert Euler angles to rotation matrix
	glm::mat4 rotationMatrix = glm::eulerAngleYXZ(glm::radians(euler.y), glm::radians(euler.x), glm::radians(euler.z));

	// Extract pitch, yaw, and roll from the rotation matrix
	float newPitch, newYaw, newRoll;
	glm::extractEulerAngleYXZ(rotationMatrix, newYaw, newPitch, newRoll);

	// Return adjusted pitch and yaw with roll set to 0
	return glm::degrees(glm::vec3(newPitch, newYaw, newRoll));
}


glm::quat BravoMath::DirectionToQuaternion(const glm::vec3& Direction)
{
	glm::vec3 dir = Direction;
	if ( glm::length(dir) <= FLT_EPS )
		dir = BravoMath::forwardV;

	if(glm::abs(glm::dot(dir, BravoMath::upV)) > 1.0f - FLT_EPS)
	{
		return glm::quatLookAt(glm::normalize(dir), BravoMath::forwardV);
	}
	else
	{
		return glm::quatLookAt(glm::normalize(dir), BravoMath::upV);
	}
}

glm::vec3 BravoMath::QuaternionToDirection(const glm::quat& Quaternion)
{
	return glm::rotate(Quaternion, BravoMath::forwardV);
}

glm::vec3 BravoMath::QuatToEuler(const glm::quat& QuatRotation)
{
	glm::vec3 euler = glm::degrees(glm::eulerAngles(QuatRotation));


	return euler;
}
glm::quat BravoMath::EulerToQuat(const glm::vec3& EulerRotation)
{
	return glm::quat(glm::radians(EulerRotation));
}

glm::vec3 BravoMath::RandVector(float MaxDimension)
{
	int32 iMaxDimension = (int32)MaxDimension;
	glm::vec3 Result(
		BravoMath::Rand(-MaxDimension, MaxDimension),
		BravoMath::Rand(-MaxDimension, MaxDimension),
		BravoMath::Rand(-MaxDimension, MaxDimension)
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