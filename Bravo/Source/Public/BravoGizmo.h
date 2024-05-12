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

	void SetGizmoState(EBravoGizmoState NewState);

protected:

	std::list<std::shared_ptr<class BravoStaticMeshComponent>> TransformComponents;
	std::list<std::shared_ptr<class BravoStaticMeshComponent>> RotationComponents;
	std::list<std::shared_ptr<class BravoStaticMeshComponent>> ScaleComponents;

	EBravoGizmoState GizmoState = EBravoGizmoState::Transform;
};