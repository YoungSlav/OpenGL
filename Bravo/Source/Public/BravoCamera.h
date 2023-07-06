#pragma once
#include "stdafx.h"
#include "BravoObject.h"
#include "BravoActor.h"


class BravoCamera : public BravoActor
{
public:

	const glm::mat4& GetViewMatrix() const { return ViewMatrix; }
	const glm::mat4 GetProjectionMatrix() const { return ProjectionMatrix; }

	void SetFOV(float InFOV) { FOV = InFOV; }
	void SetDrawingDistance(float InMin, float InMax) { MinDrawingDistance = InMin; MaxDrawingDistance = InMax; }
	
	
	float GetMinDrawingDistance() const { return MinDrawingDistance; }
	float GetMaxDrawingDistance() const { return MaxDrawingDistance; }

	void AttachTo(const std::shared_ptr<BravoActor> InActor);
	void Detach();

	void UpdateCamera();

	std::shared_ptr<BravoActor> GetOwner() const { return Owner.expired() ? nullptr : Owner.lock(); }

protected:
	virtual bool Initialize_Internal() override;
	virtual void UpdateCamera_Internal();

private:
	float FOV = 45.0f;
	float MinDrawingDistance = 0.1f;
	float MaxDrawingDistance = 100.0f;

	bool bAttachedToActor = false;
	std::weak_ptr<BravoActor> Owner;
	
	glm::mat4 ViewMatrix;
	glm::mat4 ProjectionMatrix;
};

