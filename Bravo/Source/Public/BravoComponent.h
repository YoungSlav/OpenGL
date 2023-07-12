#pragma once
#include "stdafx.h"
#include "BravoObject.h"
#include "BravoMath.h"
#include "BravoTransform.h"

class BravoComponent : public BravoObject
{
public:
	glm::vec3 GetLocation_World() const;
	inline const glm::vec3& GetLocation() const { return Transform.GetLocation(); }
	inline void SetLocation(const glm::vec3& InLocation) { Transform.SetLocation(InLocation); }

	glm::vec3 GetRotation_World() const;
	inline const glm::vec3& GetRotation() const { return Transform.GetRotation(); }
	inline void SetRotation(const glm::vec3& InRotation) { Transform.SetRotation(InRotation); }
	
	glm::vec3 GetDirection_World() const;
	inline glm::vec3 GetDirection() const { return Transform.GetDirection(); }
	inline void SetDirection(const glm::vec3& Direction) { Transform.SetDirection(Direction); }

	glm::vec3 GetScale_World() const;
	inline const glm::vec3& GetScale() const { return Transform.GetScale(); }
	inline void SetScale(const glm::vec3& InScale) { Transform.SetScale(InScale); }

	const BravoTransform& GetTransform_World() const;
	inline const BravoTransform& GetTransform() const { return Transform; }
	inline void SetTransform(const BravoTransform& InTransform) { Transform = InTransform; }

	std::shared_ptr<class BravoActor> GetOwningActor() const;

protected:
	virtual bool Initialize_Internal() override;

protected:
	BravoTransform Transform;
};