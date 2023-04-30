#pragma once
#include "stdafx.h"
#include "BravoObject.h"
#include "BravoActor.h"


class BravoCamera : public BravoActor, public BravoTickable
{
public:
	const glm::mat4& GetViewMatrix() const { return ViewMatrix; }
	const glm::mat4 GetProjectionMatrix() const { return ProjectionMatrix; }

	void SetFOV(float InFOV) { FOV = InFOV; }
	void SetAspectRatio(float InAspectRatio) { AspectRatio = InAspectRatio; }
	void SetDrawingDistance(float InMin, float InMax) { MinDrawingDistance = InMin; MaxDrawingDistance = InMax; }

	void SetActive();

	void AttachTo(const BravoActor* InActor);
	void Detach();

protected:
	virtual void Init() override;
	virtual void Tick(float DeltaTime) override;

	virtual void CalcCamera();

protected:
	float FOV = 90.0f;
	float AspectRatio;
	float MinDrawingDistance;
	float MaxDrawingDistance;

	bool bAttachedToActor = false;
	const BravoActor* Owner = nullptr;
	

private:
	glm::mat4 ViewMatrix;
	glm::mat4 ProjectionMatrix;
};

