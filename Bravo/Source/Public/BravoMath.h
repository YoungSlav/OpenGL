#pragma once

#include "stdafx.h"

#define FLT_EPS 0.001f

class BravoMath
{
public:
	static glm::vec3 DirectionToRotation(const glm::vec3& Direction);
	static glm::quat DirectionToQuaternion(const glm::vec3& Direction);

	static glm::vec3 RotationToDirection(const glm::vec3& Rotation);
	static glm::vec3 QuaternionToDirection(const glm::quat& Quaternion);


	static glm::vec3 NormalizeEulerAngles(const glm::vec3& angles);
	static glm::vec3 QuatToEuler(const glm::quat& QuatRotation);
	static glm::quat EulerToQuat(const glm::vec3& EulerRotation);

	template<typename T>
	T BravoMath::Rand(T Min, T Max)
	{
		double r = (double)(rand()) / (double)(RAND_MAX);

		return Min + (T)(((Max - Min)*r);
	}
	static glm::vec3 RandVector(float MaxDimension);

	static bool IsNearlyZero(float Number, double Tolerance = 1e-5);
	static bool IsNearlyZero(double Number, double Tolerance = 1e-5);
	static bool IsNearlyZero(const glm::vec3& vector, double Tolerance = 1e-5);

	static float MaxComponent(const glm::vec3& Vector);

	static void GetFrustumCornersWorldSpace(const glm::mat4& frustrum, std::vector<glm::vec4>& OutFrustumCorners);

	static const glm::vec3 upV;
	static const glm::vec3 forwardV;
	static const glm::vec3 rightV;
};
