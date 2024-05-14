#pragma once

#include "stdafx.h"
#include "BravoMath.h"

class BravoTransform final
{
public:
	BravoTransform()
	{
		UpdateTransformMatrix();
	}
	BravoTransform(const glm::vec3& _location, const glm::vec3& _rotation, const glm::vec3& _scale) :
		Location(_location), Rotation(_rotation), Scale(_scale)
	{
		UpdateTransformMatrix();
	}
	BravoTransform(const glm::mat4& _transform) :
		TransformMatrix(_transform)
	{
		DecomposeTransformMatrix();
	}

	inline const glm::vec3& GetLocation() const { return Location; }
	inline const glm::vec3& GetRotation() const { return Rotation; }
	inline const glm::vec3& GetDirection() const { return Direction; }
	inline const glm::vec3& GetScale() const { return Scale; }
	inline const glm::mat4& GetTransformMatrix() const { UpdateTransformMatrix(); return TransformMatrix; }


	inline void SetTransformMatrix(const glm::mat4& _transform)
	{
		TransformMatrix = _transform;
		DecomposeTransformMatrix();
	}

	inline void SetLocation(const glm::vec3& _location)
	{
		Location = _location;
		bMatrixDirty = true;
	}

	inline void SetRotation(const glm::vec3& _rotation)
	{
		Rotation = _rotation;
		Direction = BravoMath::RotationToDirection(Rotation);
		bMatrixDirty = true;
	}
	
	inline void SetDirection(const glm::vec3& _direction)
	{
		Direction = _direction;
		Rotation = BravoMath::DirectionToRotation(_direction);
		bMatrixDirty = true;
	}

	inline void SetScale(const glm::vec3& _scale)
	{
		Scale = _scale;
		bMatrixDirty = true;
	}

	inline bool IsNearlyEqual(const BravoTransform& Other, float Eps = FLT_EPS)
	{
		UpdateTransformMatrix();
		// "Code from hell" material :)
		return (
			abs(TransformMatrix[0][0] - Other.TransformMatrix[0][0]) <= Eps &&
			abs(TransformMatrix[0][1] - Other.TransformMatrix[0][1]) <= Eps &&
			abs(TransformMatrix[0][2] - Other.TransformMatrix[0][2]) <= Eps &&
			abs(TransformMatrix[0][3] - Other.TransformMatrix[0][3]) <= Eps &&
			
			abs(TransformMatrix[1][0] - Other.TransformMatrix[1][0]) <= Eps &&
			abs(TransformMatrix[1][1] - Other.TransformMatrix[1][1]) <= Eps &&
			abs(TransformMatrix[1][2] - Other.TransformMatrix[1][2]) <= Eps &&
			abs(TransformMatrix[1][3] - Other.TransformMatrix[1][3]) <= Eps &&
			
			abs(TransformMatrix[2][0] - Other.TransformMatrix[2][0]) <= Eps &&
			abs(TransformMatrix[2][1] - Other.TransformMatrix[2][1]) <= Eps &&
			abs(TransformMatrix[2][2] - Other.TransformMatrix[2][2]) <= Eps &&
			abs(TransformMatrix[2][3] - Other.TransformMatrix[2][3]) <= Eps &&
			
			abs(TransformMatrix[3][0] - Other.TransformMatrix[3][0]) <= Eps &&
			abs(TransformMatrix[3][1] - Other.TransformMatrix[3][1]) <= Eps &&
			abs(TransformMatrix[3][2] - Other.TransformMatrix[3][2]) <= Eps &&
			abs(TransformMatrix[3][3] - Other.TransformMatrix[3][3]) <= Eps
			);
	}

private:
	inline void UpdateTransformMatrix() const
	{
		if ( !bMatrixDirty ) return;
		glm::mat4 transform;
		ApplyOnMatrix(transform);
		TransformMatrix = transform;
		bMatrixDirty = false;
	}

	inline void ApplyOnMatrix(glm::mat4& OutMatrix) const
	{
		OutMatrix = glm::translate(OutMatrix, Location);
		OutMatrix = OutMatrix * glm::toMat4(BravoMath::EulerToQuat(Rotation));
		OutMatrix = glm::scale(OutMatrix, Scale);
	}

	inline void DecomposeTransformMatrix()
	{
		Location = glm::vec3(
			TransformMatrix[3].x,
			TransformMatrix[3].y,
			TransformMatrix[3].z
		);
		
		Scale = glm::vec3(
			glm::length(glm::vec3(TransformMatrix[0][0], TransformMatrix[0][1], TransformMatrix[0][2])),
			glm::length(glm::vec3(TransformMatrix[1][0], TransformMatrix[1][1], TransformMatrix[1][2])),
			glm::length(glm::vec3(TransformMatrix[2][0], TransformMatrix[2][1], TransformMatrix[2][2]))
		);

		Rotation = glm::degrees(glm::eulerAngles(glm::quat_cast(TransformMatrix)));
		Direction = BravoMath::RotationToDirection(Rotation);

		bMatrixDirty = false;
	}

private:
	mutable glm::mat4 TransformMatrix;
	mutable bool bMatrixDirty = true;


	glm::vec3 Location = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 Rotation = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 Direction = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 Scale = glm::vec3(1.0f, 1.0f, 1.0f);
};