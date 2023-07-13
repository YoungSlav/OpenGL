#pragma once
#include "stdafx.h"
#include "BravoObject.h"
#include "BravoMath.h"
#include "BravoTransform.h"
#include "BravoComponent.h"

class BravoActor : public BravoObject
{
public:

	const glm::vec3& GetLocation() const { return Transform.GetLocation(); }
	void SetLocation(const glm::vec3& InLocation) { Transform.SetLocation(InLocation); }

	const glm::vec3& GetRotation() const { return Transform.GetRotation(); }
	void SetRotation(const glm::vec3& InRotation) { Transform.SetRotation(InRotation); }
	
	glm::vec3 GetDirection() const { return Transform.GetDirection(); }
	void SetDirection(const glm::vec3& Direction) { Transform.SetDirection(Direction); }

	const glm::vec3& GetScale() const { return Transform.GetScale(); }
	void SetScale(const glm::vec3& InScale) { Transform.SetScale(InScale); }

	void SetTransform(const BravoTransform& InTransform) { Transform = InTransform; }
	const BravoTransform& GetTransform() const { return Transform; }

	const std::vector<std::shared_ptr<BravoComponent>>& GetComponents() const { return Components; }

protected:
	virtual void OnChildObjectAdded(std::weak_ptr<BravoObject> _OwnedObject) override;
	virtual void OnDestroy() override;

protected:
	BravoTransform Transform;

	std::vector<std::shared_ptr<BravoComponent>> Components;
};
