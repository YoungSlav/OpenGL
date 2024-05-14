#pragma once
#include "stdafx.h"
#include "BravoTransform.h"

class ITransformable
{
public:
	inline const BravoTransform& GetTransform() const { return Transform; }
	inline const glm::vec3& GetLocation() const { return Transform.GetLocation(); }
	inline const glm::vec3& GetRotation() const { return Transform.GetRotation(); }
	inline const glm::vec3& GetDirection() const { return Transform.GetDirection(); }
	inline const glm::vec3& GetScale() const { return Transform.GetScale(); }

	inline void SetTransform(const BravoTransform& InTransform) { Transform = InTransform; }
	inline void SetLocation(const glm::vec3& InLocation) { Transform.SetLocation(InLocation); }
	inline void SetRotation(const glm::vec3& InRotation) { Transform.SetRotation(InRotation); }
	inline void SetDirection(const glm::vec3& Direction) { Transform.SetDirection(Direction); }
	inline void SetScale(const glm::vec3& InScale) { Transform.SetScale(InScale); }

	virtual BravoTransform GetTransform_World() const { return Transform; }
	glm::vec3 GetLocation_World() const { return GetTransform_World().GetLocation(); }
	glm::vec3 GetRotation_World() const { return GetTransform_World().GetRotation(); }
	glm::vec3 GetDirection_World() const { return GetTransform_World().GetDirection(); }
	glm::vec3 GetScale_World() const { return GetTransform_World().GetScale(); }


	virtual void SetTransform_World(const BravoTransform& InTransform) { Transform = InTransform; }
	void SetLocation_World(const glm::vec3& InLocation)
	{
		BravoTransform worldTransform = GetTransform_World();
		worldTransform.SetLocation(InLocation);
		SetTransform_World(worldTransform);
	}
	void SetRotation_World(const glm::vec3& InRotation)
	{
		BravoTransform worldTransform = GetTransform_World();
		worldTransform.SetRotation(InRotation);
		SetTransform_World(worldTransform);
	}
	void SetDirection_World(const glm::vec3& Direction)
	{
		BravoTransform worldTransform = GetTransform_World();
		worldTransform.SetDirection(Direction);
		SetTransform_World(worldTransform);
	}
	void SetScale_World(const glm::vec3& InScale)
	{
		BravoTransform worldTransform = GetTransform_World();
		worldTransform.SetScale(InScale);
		SetTransform_World(worldTransform);
	}

protected:
	BravoTransform Transform;
};