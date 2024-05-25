#pragma once

#include "stdafx.h"

#define FLT_EPS FLT_EPSILON

class BravoMath
{
public:
	static glm::quat DirectionToQuaternion(const glm::vec3& Direction);
	static glm::vec3 QuaternionToDirection(const glm::quat& Quaternion);


	static glm::vec3 NormalizeEuler(const glm::vec3& angles);
	static glm::vec3 QuatToEuler(const glm::quat& QuatRotation);
	static glm::quat EulerToQuat(const glm::vec3& EulerRotation);

	template<typename T>
	static T Rand(T Min, T Max)
	{
		double r = (double)(rand()) / (double)(RAND_MAX);

		return Min + (T)((Max - Min)*r);
	}
	static glm::vec3 RandVector(float MaxDimension);

	static bool IsNearlyZero(float Number);
	static bool IsNearlyZero(double Number);
	static bool IsNearlyZero(const glm::vec2& vector);
	static bool IsNearlyZero(const glm::vec3& vector);

	static bool IsNearlyEqual(const float& A, const float& B);
	static bool IsNearlyEqual(const double& A, const double& B);
	static bool IsNearlyEqual(const glm::vec3& A, const glm::vec3& B);
	static bool IsNearlyEqual(const glm::quat& A, const glm::quat& B);
	static bool IsNearlyEqual(const glm::mat4& A, const glm::mat4& B);
	static bool IsNearlyEqual(const class BravoTransform& A, const BravoTransform& B);



	static float MaxComponent(const glm::vec3& Vector);

	static void GetFrustumCornersWorldSpace(const glm::mat4& frustrum, std::vector<glm::vec4>& OutFrustumCorners);

	static const glm::vec3 upV;
	static const glm::vec3 forwardV;
	static const glm::vec3 rightV;
};
