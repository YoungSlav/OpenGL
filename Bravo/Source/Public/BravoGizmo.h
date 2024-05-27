#pragma once

#include "BravoActor.h"

enum EBravoGizmoState : int32
{
	Transform = 0,
	Rotation = 1,
	Scale = 2,
};

class BravoGizmo : public BravoActor, public IBravoTickable
{
public:
	template <typename... Args>
	BravoGizmo(Args&&... args) :
		BravoActor(std::forward<Args>(args)...),
		IBravoTickable()
	{}


	void UpdateGizmoAttachments(std::list<std::weak_ptr<class IBravoTransformable>> NewAttachments);

protected:
	virtual bool Initialize_Internal() override;
	virtual void OnDestroy() override;
	virtual void OnBeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	void OnInput_ChangeGizmo(bool ButtonState, float DeltaTime);

	void OnInput_MouseReleased(bool ButtonState, float DeltaTime);
	void OnMouseMove(const glm::vec2& CurrentPosition, const glm::vec2& DeltaMove, float DeltaTime);

	void SetGizmoState(EBravoGizmoState NewState);

	void OnTransformX(const int32&);
	void OnTransformY(const int32&);
	void OnTransformZ(const int32&);

	void OnScaleX(const int32&);
	void OnScaleY(const int32&);
	void OnScaleZ(const int32&);

	void OnRotationX(const int32&);
	void OnRotationY(const int32&);
	void OnRotationZ(const int32&);

	void ResetInput();

private:
	bool CastRay(glm::vec3& OutIntersection);

	const glm::vec3& SelectPlane(const glm::vec3& NormalA, const glm::vec3& NormalB)const;

protected:

	std::list<std::weak_ptr<class IBravoTransformable>> Attachments;

	std::list<std::shared_ptr<class BravoStaticMeshComponent>> TransformComponents;
	std::list<std::shared_ptr<class BravoStaticMeshComponent>> RotationComponents;
	std::list<std::shared_ptr<class BravoStaticMeshComponent>> ScaleComponents;

	EBravoGizmoState GizmoState = EBravoGizmoState::Transform;

	bool bInputActive = false;
	glm::vec3 InputPlane = glm::vec3(0.0);
	glm::vec3 InputMask = glm::vec3(0.0);
	glm::vec3 OldIntersection = glm::vec3(0.0);


	const float DesiredScreenSize = 100.0f;
};