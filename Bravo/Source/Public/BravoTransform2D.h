#pragma once

#include "stdafx.h"
#include "BravoMath.h"

class BravoTransform2D final
{
	RTTR_ENABLE();
	
public:
	BravoTransform2D();

	BravoTransform2D(const glm::vec2& _position, float _rotation, const glm::vec2& _scale);
	BravoTransform2D(const glm::mat4& _transform);

	const glm::vec2& GetLocation() const;
	float GetRotation() const;
	const glm::vec2& GetScale() const;
	const glm::mat4& GetTransformMatrix() const;

	glm::vec2 GetForwardVector() const;
	glm::vec2 GetRightVector() const;

	void SetTransformMatrix(const glm::mat4& _transform);
	void SetLocation(const glm::vec2& _location);
	void SetRotation(float _rotation);
	void SetScale(const glm::vec2& _scale);
	void SetDirection(const glm::vec2& _direction);

private:
	void UpdateTransformMatrix() const;

	void DecomposeTransformMatrix();

private:
	mutable glm::mat4 TransformMatrix = glm::mat4(1.0f);
	mutable bool bMatrixDirty = true;

	glm::vec2 Location = glm::vec2(0.0f, 0.0f);
	float Rotation = 0.0f;
	glm::vec2 Scale = glm::vec2(1.0f, 1.0f);
};