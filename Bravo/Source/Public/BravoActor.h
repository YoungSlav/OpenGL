#pragma once
#include "stdafx.h"
#include "BravoObject.h"
#include "BravoMath.h"

class BravoActor : public BravoObject
{
public:
	friend class BravoEngine;

	BravoActor(std::shared_ptr<class BravoEngine> _Engine, const BravoHandle& _Handle) :
		BravoObject(_Engine, _Handle)
	{}

	const glm::vec3& GetLocation() const { return Location; }
	void SetLocation(const glm::vec3& InLocation) { Location = InLocation; }

	const glm::vec3& GetRotation() const { return Rotation; }
	void SetRotation(const glm::vec3& InRotation) { Rotation = InRotation; }
	
	inline glm::vec3 GetDirection() const { return BravoMath::RotationToDirection(Rotation); }
	inline void SetDirection(const glm::vec3& Direction) { Rotation = BravoMath::DirectionToRotation(Direction); }

	const glm::vec3& GetScale() const { return Scale; }
	void SetScale(const glm::vec3& InScale) { Scale = InScale; }

protected:

	virtual void Render(const glm::vec3& CameraLocation, const glm::mat4& CameraProjection, const glm::mat4& CameraView) const {}
	virtual void RenderDepthMap(std::shared_ptr<class BravoShader> Shader, const glm::vec3& LightPosition) const {}

	glm::mat4 GetTransformMatrix() const;

	glm::vec3 Scale = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec3 Location = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 Rotation = glm::vec3(0.0f, 0.0f, 0.0f);

	int32 RenderPriority = 0;

private:
};
