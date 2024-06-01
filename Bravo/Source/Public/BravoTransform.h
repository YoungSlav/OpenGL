#pragma once

#include "stdafx.h"
#include "BravoMath.h"

class BravoTransform final
{
public:
	BravoTransform()
	{}

	BravoTransform(const glm::vec3& _location, const glm::quat& _rotation, const glm::vec3& _scale) :
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
	inline const glm::quat& GetRotation() const { return Rotation; }
	inline const glm::vec3& GetScale() const { return Scale; }
	inline const glm::mat4& GetTransformMatrix() const { UpdateTransformMatrix(); return TransformMatrix; }

	glm::vec3 GetForwardVector() const
	{
		return glm::rotate(Rotation, BravoMath::forwardV);
	}

	glm::vec3 GetRightVector() const
	{
		return glm::rotate(Rotation, BravoMath::rightV);
	}

	glm::vec3 GetUpVector() const
	{
		return glm::rotate(Rotation, BravoMath::upV);
	}


	void SetTransformMatrix(const glm::mat4& _transform)
	{
		TransformMatrix = _transform;
		DecomposeTransformMatrix();
	}

	void SetLocation(const glm::vec3& _location)
	{
		Location = _location;
		bMatrixDirty = true;
	}

	void SetRotation(const glm::quat& _rotation)
	{
		Rotation = _rotation;
		bMatrixDirty = true;
	}

	void SetRotation(const glm::vec3& _eulerRotation)
	{
		Rotation = BravoMath::EulerToQuat(_eulerRotation);
		bMatrixDirty = true;
	}


	void Rotate(const glm::quat& _rotation)
	{
		Rotation = _rotation * Rotation;
		bMatrixDirty = true;
	}
	
	void SetDirection(const glm::vec3& _direction)
	{
		glm::vec3 dir(0.0f);
		if ( glm::length(_direction) <= FLT_EPS )
			dir = BravoMath::forwardV;
		else
			dir = glm::normalize(_direction);

		Rotation = BravoMath::DirectionToQuaternion(dir);
		bMatrixDirty = true;
	}

	void SetScale(const glm::vec3& _scale)
	{
		Scale = _scale;
		bMatrixDirty = true;
	}

private:
	inline void UpdateTransformMatrix() const
	{
		if ( !bMatrixDirty ) return;
		
		TransformMatrix = glm::recompose(Scale, Rotation, Location, glm::vec3(0.0f), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

		bMatrixDirty = false;
	}

	inline void DecomposeTransformMatrix()
	{
		glm::vec3 skew;
		glm::vec4 perspective;
		glm::decompose(TransformMatrix, Scale, Rotation, Location, skew, perspective);

		bMatrixDirty = false;
	}

private:
	mutable glm::mat4 TransformMatrix = glm::mat4(1.0f);
	mutable bool bMatrixDirty = true;


	glm::vec3 Location = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::quat Rotation = glm::quat();
	glm::vec3 Scale = glm::vec3(1.0f, 1.0f, 1.0f);
};