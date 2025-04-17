#include "BravoMath.h"
#include "BravoTransform.h"

const glm::vec3 BravoMath::forwardV = glm::vec3(0, 0, -1.0);
const glm::vec3 BravoMath::upV = glm::vec3(0.0, 1.0, 0.0);
const glm::vec3 BravoMath::rightV = glm::vec3(1.0, 0.0, 0.0);

RTTR_REGISTRATION
{
    using namespace rttr;

    registration::class_<glm::vec2>("vec2")
        .property("x", &glm::vec2::x)
        .property("y", &glm::vec2::y);

    registration::class_<glm::vec3>("vec3")
        .property("x", &glm::vec3::x)
        .property("y", &glm::vec3::y)
        .property("z", &glm::vec3::z);

    registration::class_<glm::vec4>("vec4")
        .property("x", &glm::vec4::x)
        .property("y", &glm::vec4::y)
        .property("z", &glm::vec4::z)
        .property("w", &glm::vec4::w);


    registration::class_<glm::ivec2>("ivec2")
        .property("x", &glm::ivec2::x)
        .property("y", &glm::ivec2::y);

    registration::class_<glm::ivec3>("ivec3")
        .property("x", &glm::ivec3::x)
        .property("y", &glm::ivec3::y)
        .property("z", &glm::ivec3::z);

    registration::class_<glm::ivec4>("ivec4")
        .property("x", &glm::ivec4::x)
        .property("y", &glm::ivec4::y)
        .property("z", &glm::ivec4::z)
        .property("w", &glm::ivec4::w);


    registration::class_<glm::quat>("quat")
        .property("x", &glm::quat::x)
        .property("y", &glm::quat::y)
        .property("z", &glm::quat::z)
        .property("w", &glm::quat::w);
}

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

bool BravoMath::IsNearlyZero(float Number)
{
	return std::abs(Number) <= FLT_EPS;
}
bool BravoMath::IsNearlyZero(double Number)
{
	return std::abs(Number) <= FLT_EPS;
}
bool BravoMath::IsNearlyZero(const glm::vec2& vector)
{
	return IsNearlyZero(vector.x) && IsNearlyZero(vector.y);
}
bool BravoMath::IsNearlyZero(const glm::vec3& vector)
{
	return IsNearlyZero(vector.x) && IsNearlyZero(vector.y) && IsNearlyZero(vector.z);
}

bool BravoMath::IsNearlyEqual(const float& A, const float& B)
{
	return glm::abs(A-B) <= FLT_EPS;
}
bool BravoMath::IsNearlyEqual(const double& A, const double& B)
{
	return glm::abs(A-B) <= FLT_EPS;
}
bool BravoMath::IsNearlyEqual(const glm::vec3& A, const glm::vec3& B)
{
	return glm::all(glm::epsilonEqual(A,B, glm::vec3(FLT_EPS)));
}
bool BravoMath::IsNearlyEqual(const glm::quat& A, const glm::quat& B)
{
	float dotProduct = glm::dot(A, B);
	return glm::abs(dotProduct) > 1.0f - FLT_EPS;
}
bool BravoMath::IsNearlyEqual(const glm::mat4& A, const glm::mat4& B)
{
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			if (!glm::epsilonEqual(A[i][j], B[i][j], FLT_EPS))
			{
				return false;
			}
		}
	}
	return true; 
}

bool BravoMath::IsNearlyEqual(const class BravoTransform& A, const BravoTransform& B)
{
	return IsNearlyEqual(A.GetTransformMatrix(), B.GetTransformMatrix());
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