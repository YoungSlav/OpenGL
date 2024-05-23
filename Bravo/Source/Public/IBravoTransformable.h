#pragma once
#include "stdafx.h"
#include "BravoTransform.h"

typedef MulticastDelegate<const class IBravoTransformable*> OnTransformUpdatedSignature;

class IBravoTransformable
{
public:
	IBravoTransformable() = default;
	IBravoTransformable(const BravoTransform& Transform) : 
		LocalTransform(Transform),
		WorldTransform()
	{
	}

	IBravoTransformable(const BravoTransform& Transform, const std::shared_ptr<IBravoTransformable> _Parent) :
		LocalTransform(Transform),
		WorldTransform()
	{
		SetParent(_Parent);
	}
	virtual ~IBravoTransformable()
	{
		if ( std::shared_ptr<IBravoTransformable> parent = GetParent() )
		{
			parent->OnTransformUpdated.RemoveObject(this);
		}
	}

	void SetParent(const std::shared_ptr<IBravoTransformable> _Parent)
	{
		if ( std::shared_ptr<IBravoTransformable> oldParent = GetParent() )
		{
			oldParent->OnTransformUpdated.RemoveObject(this);
		}
		ParentPtr = _Parent;
		if ( _Parent != nullptr )
		{
			_Parent->OnTransformUpdated.AddRaw(this, &IBravoTransformable::OnParentTransformUpdated);
			OnParentTransformUpdated(_Parent.get());
		}
	}
	std::shared_ptr<IBravoTransformable> GetParent() const { return ParentPtr.expired() ? nullptr : ParentPtr.lock(); }

	const BravoTransform& GetTransform() const { return LocalTransform; }
	const glm::vec3& GetLocation() const { return LocalTransform.GetLocation(); }
	const glm::quat& GetRotation() const { return LocalTransform.GetRotation(); }
	const glm::vec3& GetScale() const { return LocalTransform.GetScale(); }
	glm::vec3 GetForwardVector() const { return LocalTransform.GetForwardVector(); }
	glm::vec3 GetRightVector() const { return LocalTransform.GetRightVector(); }
	glm::vec3 GetUpVector() const { return LocalTransform.GetUpVector(); }

	void SetTransform(const BravoTransform& InTransform)
	{
		if ( BravoMath::IsNearlyEqual(LocalTransform.GetTransformMatrix(), InTransform.GetTransformMatrix()) )
			return;
		LocalTransform = InTransform;
		UpdateWorldTransform();
		OnTransformUpdated.Broadcast(this);
	}
	void SetLocation(const glm::vec3& InLocation)
	{
		if ( BravoMath::IsNearlyEqual(LocalTransform.GetLocation(), InLocation) )
			return;
		LocalTransform.SetLocation(InLocation);
		UpdateWorldTransform();
		OnTransformUpdated.Broadcast(this);
	}
	void SetRotation(const glm::quat& InRotation)
	{
		if ( BravoMath::IsNearlyEqual(LocalTransform.GetRotation(), InRotation) )
			return;
		LocalTransform.SetRotation(InRotation);
		UpdateWorldTransform();
		OnTransformUpdated.Broadcast(this);
	}
	void SetRotation(const glm::vec3& InRotation)
	{
		if ( BravoMath::IsNearlyEqual(LocalTransform.GetRotation(), BravoMath::EulerToQuat(InRotation)) )
			return;
		LocalTransform.SetRotation(InRotation);
		UpdateWorldTransform();
		OnTransformUpdated.Broadcast(this);
	}
	void SetDirection(const glm::vec3& Direction)
	{
		if ( BravoMath::IsNearlyEqual(LocalTransform.GetRotation(), BravoMath::DirectionToQuaternion(Direction)) )
			return;
		LocalTransform.SetDirection(Direction);
		UpdateWorldTransform();
		OnTransformUpdated.Broadcast(this);
	}
	void Rotate(const glm::quat& InRotation)
	{
		if ( BravoMath::IsNearlyZero(InRotation.w) )
			return;
		LocalTransform.Rotate(InRotation);
		UpdateWorldTransform();
		OnTransformUpdated.Broadcast(this);
	}
	void SetScale(const glm::vec3& InScale)
	{
		if ( BravoMath::IsNearlyEqual(LocalTransform.GetScale(), InScale) )
			return;
		LocalTransform.SetScale(InScale);
		UpdateWorldTransform();
		OnTransformUpdated.Broadcast(this);
	}

	const BravoTransform& GetTransform_World() const
	{
		return WorldTransform;
		
	}
	const glm::vec3& GetLocation_World() const { return WorldTransform.GetLocation(); }
	const glm::quat& GetRotation_World() const { return WorldTransform.GetRotation(); }
	const glm::vec3& GetScale_World() const { return WorldTransform.GetScale(); }
	glm::vec3 GetForwardVector_World() const { return WorldTransform.GetForwardVector(); }
	glm::vec3 GetRightVector_World() const { return WorldTransform.GetRightVector(); }
	glm::vec3 GetUpVector_World() const { return WorldTransform.GetUpVector(); }


	void SetTransform_World(const BravoTransform& InTransform)
	{
		if ( std::shared_ptr<IBravoTransformable> p = GetParent() )
		{
			BravoTransform panretTransform = p->GetTransform_World();
			glm::mat4 parentWorldInverse = glm::inverse(panretTransform.GetTransformMatrix());
			glm::mat4 localTransform = parentWorldInverse * InTransform.GetTransformMatrix();
			SetTransform(BravoTransform(localTransform));
		}
		else
		{
			SetTransform(InTransform);
		}
	}
	void SetLocation_World(const glm::vec3& InLocation)
	{
		BravoTransform worldTransform = GetTransform_World();
		worldTransform.SetLocation(InLocation);
		SetTransform_World(worldTransform);
	}
	void SetRotation_World(const glm::vec3& InRotation)
	{
		BravoTransform worldTransform = GetTransform_World();
		worldTransform.SetRotation(InRotation);
		SetTransform_World(worldTransform);
	}
	void SetRotation_World(const glm::quat& InRotation)
	{
		BravoTransform worldTransform = GetTransform_World();
		worldTransform.SetRotation(InRotation);
		SetTransform_World(worldTransform);
	}
	void SetDirection_World(const glm::vec3& Direction)
	{
		BravoTransform worldTransform = GetTransform_World();
		worldTransform.SetDirection(Direction);
		SetTransform_World(worldTransform);
	}
	void SetScale_World(const glm::vec3& InScale)
	{
		BravoTransform worldTransform = GetTransform_World();
		worldTransform.SetScale(InScale);
		SetTransform_World(worldTransform);
	}

private:

	void UpdateWorldTransform()
	{
		if ( std::shared_ptr<IBravoTransformable> p = GetParent() )
		{
			const BravoTransform& parentTransform = p->GetTransform_World();
			glm::mat4 parentMat = parentTransform.GetTransformMatrix();
			glm::mat4 myMat = LocalTransform.GetTransformMatrix();
			glm::mat4 myWorld = parentMat * myMat;
			WorldTransform.SetTransformMatrix(myWorld);
		}
		else
		{
			WorldTransform = LocalTransform;
		}
	}

	void OnParentTransformUpdated(const IBravoTransformable* _parent)
	{
		UpdateWorldTransform();
	}

	

public:
	OnTransformUpdatedSignature OnTransformUpdated;

private:
	BravoTransform LocalTransform;
	BravoTransform WorldTransform;

	std::weak_ptr<IBravoTransformable> ParentPtr;
};