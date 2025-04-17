#pragma once
#include "stdafx.h"
#include "BravoTransform.h"

typedef MulticastDelegate<const class IBravoTransformable*> OnTransformUpdatedSignature;

class IBravoTransformable
{
	RTTR_ENABLE();
public:
	IBravoTransformable() = default;
	IBravoTransformable(const BravoTransform& Transform);

	IBravoTransformable(const BravoTransform& Transform, const std::shared_ptr<IBravoTransformable> _Parent);

	virtual ~IBravoTransformable();

	void SetParent(const std::shared_ptr<IBravoTransformable> _Parent);
	std::shared_ptr<IBravoTransformable> GetParent() const;

	const BravoTransform& GetTransform() const;
	const glm::vec3& GetLocation() const;
	const glm::quat& GetRotation() const;
	const glm::vec3& GetScale() const;
	glm::vec3 GetForwardVector() const;
	glm::vec3 GetRightVector() const;
	glm::vec3 GetUpVector() const;

	void SetTransform(const BravoTransform& InTransform);
	void SetLocation(const glm::vec3& InLocation);
	void SetRotation(const glm::quat& InRotation);
	void SetRotation(const glm::vec3& InRotation);
	void SetDirection(const glm::vec3& Direction);
	void Rotate(const glm::quat& InRotation);
	void SetScale(const glm::vec3& InScale);

	const BravoTransform& GetTransform_World() const;
	const glm::vec3& GetLocation_World() const;
	const glm::quat& GetRotation_World() const;
	const glm::vec3& GetScale_World() const;
	glm::vec3 GetForwardVector_World() const;
	glm::vec3 GetRightVector_World() const;
	glm::vec3 GetUpVector_World() const;


	void SetTransform_World(const BravoTransform& InTransform);
	void SetLocation_World(const glm::vec3& InLocation);
	void SetRotation_World(const glm::vec3& InRotation);
	void SetRotation_World(const glm::quat& InRotation);
	void SetDirection_World(const glm::vec3& Direction);
	void SetScale_World(const glm::vec3& InScale);

private:

	void UpdateWorldTransform();

	void OnParentTransformUpdated(const IBravoTransformable* _parent);

	

public:
	OnTransformUpdatedSignature OnTransformUpdated;

private:
	BravoTransform LocalTransform;
	BravoTransform WorldTransform;

	std::weak_ptr<IBravoTransformable> ParentPtr;
};