#pragma once

#include "stdafx.h"


class BravoTransform final
{
	RTTR_ENABLE();
	
public:
	BravoTransform();

	BravoTransform(const glm::vec3& _location, const glm::quat& _rotation, const glm::vec3& _scale);
	BravoTransform(const glm::mat4& _transform);

	const glm::vec3& GetLocation() const;
	const glm::quat& GetRotation() const;
	const glm::vec3& GetRotationEuler() const;
	const glm::vec3& GetScale() const;
	const glm::mat4& GetTransformMatrix() const;

	glm::vec3 GetForwardVector() const;
	glm::vec3 GetRightVector() const;
	glm::vec3 GetUpVector() const;


	void SetTransformMatrix(const glm::mat4& _transform);
	void SetLocation(const glm::vec3& _location);
	void SetRotation(const glm::quat& _rotation);
	void SetRotationEuler(const glm::vec3& _eulerRotation);

	void Rotate(const glm::quat& _rotation);
	void SetDirection(const glm::vec3& _direction);
	void SetScale(const glm::vec3& _scale);

private:
	void UpdateTransformMatrix() const;
	void DecomposeTransformMatrix();

private:
	mutable glm::mat4 TransformMatrix = glm::mat4(1.0f);
	mutable bool bMatrixDirty = true;

	glm::vec3 Location = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::quat Rotation = glm::quat();
	mutable glm::vec3 RotationEuler = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 Scale = glm::vec3(1.0f, 1.0f, 1.0f);
};