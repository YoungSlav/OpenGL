#pragma once
#include "stdafx.h"
#include "BravoObject.h"
#include "BravoActor.h"

enum EBravoCameraType
{
	Perspective,
	Orthographic
};

class BravoCamera : public BravoActor
{
public:
	template <typename... Args>
	BravoCamera(EBravoCameraType _CameraType, Args&&... args) :
		BravoActor(std::forward<Args>(args)...),
		CameraType(_CameraType)
	{}

	const glm::mat4& GetViewMatrix() const { return ViewMatrix; }
	const glm::mat4 GetProjectionMatrix() const { return ProjectionMatrix; }

	// 3d camera
	void SetFOV(float InFOV) { FOV = InFOV; }
	float GetFOV() const { return FOV; }

	// 2d camera
	void SetWorld2DSize(const glm::vec2& _Size) { WorldSize2D = _Size; }

	void SetDrawingDistance(float InMin, float InMax) { MinDrawingDistance = InMin; MaxDrawingDistance = InMax; }
	
	void SetCameraType(EBravoCameraType NewType) { CameraType = NewType; }
	EBravoCameraType GetCameraType() const  { return CameraType; }
	
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
	float MaxDrawingDistance = 100000.0f;

	glm::vec2 WorldSize2D;
	
	glm::mat4 ViewMatrix;
	glm::mat4 ProjectionMatrix;

	EBravoCameraType CameraType = EBravoCameraType::Perspective;
};

