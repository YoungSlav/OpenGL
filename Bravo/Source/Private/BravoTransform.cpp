#include "BravoTransform.h"
#include "BravoMath.h"

RTTR_REGISTRATION
{
	using namespace rttr;
	registration::class_<BravoTransform>("Transform")
        .property("Location", &BravoTransform::GetLocation, &BravoTransform::SetLocation)
        .property("Rotation", &BravoTransform::GetRotationEuler, &BravoTransform::SetRotationEuler)
        .property("Scale", &BravoTransform::GetScale, &BravoTransform::SetScale)
        ;
}

BravoTransform::BravoTransform()
{
}

BravoTransform::BravoTransform(const glm::vec3& _location, const glm::quat& _rotation, const glm::vec3& _scale) :
	Location(_location), Rotation(_rotation), Scale(_scale)
{
	UpdateTransformMatrix();
}

BravoTransform::BravoTransform(const glm::mat4& _transform) :
	TransformMatrix(_transform)
{
	DecomposeTransformMatrix();
}

glm::vec3 BravoTransform::GetForwardVector() const
{
	return glm::rotate(Rotation, BravoMath::forwardV);
}

glm::vec3 BravoTransform::GetRightVector() const
{
	return glm::rotate(Rotation, BravoMath::rightV);
}

glm::vec3 BravoTransform::GetUpVector() const
{
	return glm::rotate(Rotation, BravoMath::upV);
}


void BravoTransform::SetTransformMatrix(const glm::mat4& _transform)
{
	TransformMatrix = _transform;
	DecomposeTransformMatrix();
}

void BravoTransform::SetLocation(const glm::vec3& _location)
{
	Location = _location;
	bMatrixDirty = true;
}

void BravoTransform::SetRotation(const glm::quat& _rotation)
{
	Rotation = _rotation;
	bMatrixDirty = true;
}

void BravoTransform::SetRotationEuler(const glm::vec3& _eulerRotation)
{
	RotationEuler = _eulerRotation;
	Rotation = BravoMath::EulerToQuat(_eulerRotation);
	bMatrixDirty = true;
}

const glm::vec3& BravoTransform::GetLocation() const
{
	return Location;
}
const glm::quat& BravoTransform::GetRotation() const
{
	return Rotation;
}
const glm::vec3& BravoTransform::GetRotationEuler() const
{
	return RotationEuler;
}
const glm::vec3& BravoTransform::GetScale() const
{
	return Scale;
}
const glm::mat4& BravoTransform::GetTransformMatrix() const
{
	UpdateTransformMatrix();
	return TransformMatrix;
}


void BravoTransform::Rotate(const glm::quat& _rotation)
{
	Rotation = _rotation * Rotation;
	bMatrixDirty = true;
}
	
void BravoTransform::SetDirection(const glm::vec3& _direction)
{
	glm::vec3 dir(0.0f);
	if ( glm::length(_direction) <= FLT_EPS )
		dir = BravoMath::forwardV;
	else
		dir = glm::normalize(_direction);

	Rotation = BravoMath::DirectionToQuaternion(dir);
	bMatrixDirty = true;
}

void BravoTransform::SetScale(const glm::vec3& _scale)
{
	Scale = _scale;
	bMatrixDirty = true;
}

void BravoTransform::UpdateTransformMatrix() const
{
	if ( !bMatrixDirty ) return;
		
	TransformMatrix = glm::recompose(Scale, Rotation, Location, glm::vec3(0.0f), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

	bMatrixDirty = false;
}

void BravoTransform::DecomposeTransformMatrix()
{
	glm::vec3 skew;
	glm::vec4 perspective;
	glm::decompose(TransformMatrix, Scale, Rotation, Location, skew, perspective);

	bMatrixDirty = false;
}