#include "BravoTransform2D.h"

RTTR_REGISTRATION
{
	using namespace rttr;
	registration::class_<BravoTransform2D>("Transform2D")
        .property("Location", &BravoTransform2D::GetLocation, &BravoTransform2D::SetLocation)
        .property("Rotation", &BravoTransform2D::GetRotation, &BravoTransform2D::SetRotation)
        .property("Scale", &BravoTransform2D::GetScale, &BravoTransform2D::SetScale)
        ;
}

BravoTransform2D::BravoTransform2D()
	{}

BravoTransform2D::BravoTransform2D(const glm::vec2& _position, float _rotation, const glm::vec2& _scale) :
	Location(_position), Rotation(_rotation), Scale(_scale)
{
	UpdateTransformMatrix();
}
BravoTransform2D::BravoTransform2D(const glm::mat4& _transform) :
	TransformMatrix(_transform)
{
	DecomposeTransformMatrix();
}

const glm::vec2& BravoTransform2D::GetLocation() const { return Location; }
float BravoTransform2D::GetRotation() const { return Rotation; }
const glm::vec2& BravoTransform2D::GetScale() const { return Scale; }
const glm::mat4& BravoTransform2D::GetTransformMatrix() const { UpdateTransformMatrix(); return TransformMatrix; }

glm::vec2 BravoTransform2D::GetForwardVector() const
{
	return glm::rotate(glm::vec2(1.0f, 0.0f), glm::radians(Rotation));
}

glm::vec2 BravoTransform2D::GetRightVector() const
{
	return glm::rotate(glm::vec2(0.0f, 1.0f), glm::radians(Rotation));
}

void BravoTransform2D::SetTransformMatrix(const glm::mat4& _transform)
{
	TransformMatrix = _transform;
	DecomposeTransformMatrix();
}
	 
void BravoTransform2D::SetLocation(const glm::vec2& _location)
{
	Location = _location;
	bMatrixDirty = true;
}

void BravoTransform2D::SetRotation(float _rotation)
{
	Rotation = _rotation;
	bMatrixDirty = true;
}

void BravoTransform2D::SetScale(const glm::vec2& _scale)
{
	Scale = _scale;
	bMatrixDirty = true;
}

void BravoTransform2D::SetDirection(const glm::vec2& _direction)
{
	glm::vec2 dir(0.0f);
	if (glm::length(_direction) <= FLT_EPSILON)
		dir = glm::vec2(1.0f, 0.0f);
	else
		dir = glm::normalize(_direction);

	Rotation = glm::degrees(atan2(dir.y, dir.x));
	bMatrixDirty = true;
}


void BravoTransform2D::UpdateTransformMatrix() const
{
	if (!bMatrixDirty) return;
	glm::vec3 Scale3d(Scale.x, Scale.y, 1.0f);
	glm::quat Rotation3d = glm::normalize(glm::quat(glm::radians(Rotation), glm::vec3(0.0f, 0.0f, 1.0f)));
	glm::vec3 Location3d(Location.x, Location.y, 0.0f);

	TransformMatrix = glm::recompose(Scale3d, Rotation3d, Location3d, glm::vec3(0.0f), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
		
	bMatrixDirty = false;
}

void BravoTransform2D::DecomposeTransformMatrix()
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