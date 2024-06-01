#pragma once

#include "stdafx.h"
#include "BravoMath.h"

class BravoTransform2D final
{
public:
	BravoTransform2D()
	{}

	BravoTransform2D(const glm::vec2& _position, float _rotation, const glm::vec2& _scale) :
		Location(_position), Rotation(_rotation), Scale(_scale)
	{
		UpdateTransformMatrix();
	}
	BravoTransform2D(const glm::mat4& _transform) :
		TransformMatrix(_transform)
	{
		DecomposeTransformMatrix();
	}

	inline const glm::vec2& GetLocation() const { return Location; }
	inline float GetRotation() const { return Rotation; }
	inline const glm::vec2& GetScale() const { return Scale; }
	inline const glm::mat4& GetTransformMatrix() const { UpdateTransformMatrix(); return TransformMatrix; }

	glm::vec2 GetForwardVector() const
	{
		return glm::rotate(glm::vec2(1.0f, 0.0f), glm::radians(Rotation));
	}

	glm::vec2 GetRightVector() const
	{
		return glm::rotate(glm::vec2(0.0f, 1.0f), glm::radians(Rotation));
	}

	void SetTransformMatrix(const glm::mat4& _transform)
	{
		TransformMatrix = _transform;
		DecomposeTransformMatrix();
	}
	 
	void SetLocation(const glm::vec2& _location)
	{
		Location = _location;
		bMatrixDirty = true;
	}

	void SetRotation(float _rotation)
	{
		Rotation = _rotation;
		bMatrixDirty = true;
	}

	void SetScale(const glm::vec2& _scale)
	{
		Scale = _scale;
		bMatrixDirty = true;
	}

	void SetDirection(const glm::vec2& _direction)
	{
		glm::vec2 dir(0.0f);
		if (glm::length(_direction) <= FLT_EPSILON)
			dir = glm::vec2(1.0f, 0.0f);
		else
			dir = glm::normalize(_direction);

		Rotation = glm::degrees(atan2(dir.y, dir.x));
		bMatrixDirty = true;
	}

private:
	inline void UpdateTransformMatrix() const
	{
		if (!bMatrixDirty) return;
		glm::vec3 Scale3d(Scale.x, Scale.y, 1.0f);
		glm::quat Rotation3d = glm::normalize(glm::quat(glm::radians(Rotation), glm::vec3(0.0f, 0.0f, 1.0f)));
		glm::vec3 Location3d(Location.x, Location.y, 0.0f);

		TransformMatrix = glm::recompose(Scale3d, Rotation3d, Location3d, glm::vec3(0.0f), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
		
		bMatrixDirty = false;
	}

	inline void DecomposeTransformMatrix()
	{
		glm::vec3 translation, skew;
        glm::quat orientation;
        glm::vec3 scale;
        glm::vec4 perspective;
        glm::decompose(TransformMatrix, scale, orientation, translation, skew, perspective);

        Location = glm::vec2(translation.x, translation.y);
        Scale = glm::vec2(scale.x, scale.y);

        glm::vec3 euler = glm::eulerAngles(glm::normalize(orientation));
		Rotation = glm::degrees(euler.z);

        bMatrixDirty = false;
	}

private:
	mutable glm::mat4 TransformMatrix = glm::mat4(1.0f);
	mutable bool bMatrixDirty = true;

	glm::vec2 Location = glm::vec2(0.0f, 0.0f);
	float Rotation = 0.0f;
	glm::vec2 Scale = glm::vec2(1.0f, 1.0f);
};