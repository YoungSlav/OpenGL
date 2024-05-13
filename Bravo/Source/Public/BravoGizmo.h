#pragma once

#include "BravoActor.h"

enum EBravoGizmoState : int32
{
	Transform = 0,
	Rotation = 1,
	Scale = 2,
};

class BravoGizmo : public BravoActor
{

protected:
	virtual bool Initialize_Internal() override;
	virtual void OnDestroy() override;
	virtual void OnBeginPlay() override;

	void OnInput_ChangeGizmo(bool ButtonState, float DeltaTime);

	void OnInput_MouseReleased(bool ButtonState, float DeltaTime);
	void OnMouseMove(const glm::vec2& CurrentPosition, const glm::vec2& DeltaMove, float DeltaTime);

	void SetGizmoState(EBravoGizmoState NewState);

	void OnTransformX(const int32&);
	void OnTransformY(const int32&);
	void OnTransformZ(const int32&);

	void ResetInput();

private:
	bool CastRay(glm::vec3& OutIntersection);

protected:

	std::list<std::shared_ptr<class BravoStaticMeshComponent>> TransformComponents;
	std::list<std::shared_ptr<class BravoStaticMeshComponent>> RotationComponents;
	std::list<std::shared_ptr<class BravoStaticMeshComponent>> ScaleComponents;

	EBravoGizmoState GizmoState = EBravoGizmoState::Transform;

	bool bInputActive = false;
	glm::vec3 TransformDiff;
	glm::vec3 TransformInputPlane = glm::vec3(0.0);
	glm::vec3 TransformInputMask = glm::vec3(0.0);
};