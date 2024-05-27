#pragma once
#include "stdafx.h"
#include "BravoObject.h"
#include "BravoActor.h"

class BravoCamera : public BravoActor
{
public:
	template <typename... Args>
	BravoCamera(Args&&... args) :
		BravoActor(std::forward<Args>(args)...)
	{}


	const glm::mat4& GetViewMatrix() const { return ViewMatrix; }
	const glm::mat4 GetProjectionMatrix() const { return PerspectiveMatrix; }

	void SetFOV(float InFOV) { FOV = InFOV; }
	float GetFOV() const { return FOV; }
	void SetDrawingDistance(float InMin, float InMax) { MinDrawingDistance = InMin; MaxDrawingDistance = InMax; }
	
	
	float GetMinDrawingDistance() const { return MinDrawingDistance; }
	float GetMaxDrawingDistance() const { return MaxDrawingDistance; }

	void AttachTo(const std::shared_ptr<BravoActor> InActor);
	void Detach();

	void UpdateCamera();

protected:
	virtual bool Initialize_Internal() override;
	virtual void UpdateCamera_Internal();

private:
	float FOV = 45.0f;
	float MinDrawingDistance = 0.1f;
	float MaxDrawingDistance = 1000.0f;
	
	glm::mat4 ViewMatrix;
	glm::mat4 PerspectiveMatrix;
	glm::mat4 OrthographicMatrix;
};

