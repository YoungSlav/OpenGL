#pragma once
#include "stdafx.h"
#include "BravoObject.h"
#include "BravoMath.h"

class BravoComponent : public BravoObject
{
public:
	const glm::vec3& GetLocation() const { return Location; }
	void SetLocation(const glm::vec3& InLocation) { Location = InLocation; }

	const glm::vec3& GetRotation() const { return Rotation; }
	void SetRotation(const glm::vec3& InRotation) { Rotation = InRotation; }
	
	glm::vec3 GetDirection() const { return BravoMath::RotationToDirection(Rotation); }
	void SetDirection(const glm::vec3& Direction) { Rotation = BravoMath::DirectionToRotation(Direction); }

	const glm::vec3& GetScale() const { return Scale; }
	void SetScale(const glm::vec3& InScale) { Scale = InScale; }

	glm::mat4 GetTransformMatrix_Local() const;
	glm::mat4 GetTransformMatrix_World() const;

	std::shared_ptr<class BravoActor> GetOwningActor() const;

protected:
	virtual bool Initialize_Internal() override;

protected:
	glm::vec3 Scale = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec3 Location = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 Rotation = glm::vec3(0.0f, 0.0f, 0.0f);
};