#pragma once
#include "stdafx.h"
#include "BravoTransform.h"

typedef MulticastDelegate<const class ITransformable*> OnTransformUpdatedSignature;

class ITransformable
{
public:
	ITransformable() = default;
	~ITransformable()
	{
		if ( std::shared_ptr<ITransformable> oldParent = GetParent() )
		{
			oldParent->OnTransformUpdated.RemoveObject(this);
		}
	}

	void SetParent(const std::shared_ptr<ITransformable> _Parent)
	{
		if ( std::shared_ptr<ITransformable> oldParent = GetParent() )
		{
			oldParent->OnTransformUpdated.RemoveObject(this);
		}
		Parent = _Parent;
		_Parent->OnTransformUpdated.AddRaw(this, &ITransformable::OnParentTransformUpdated);

		SetTransform_World(WorldTransform);
	}
	std::shared_ptr<ITransformable> GetParent() const { return Parent.expired() ? nullptr : Parent.lock(); }

	const BravoTransform& GetTransform() const { return LocalTransform; }
	const glm::vec3& GetLocation() const { return LocalTransform.GetLocation(); }
	const glm::vec3& GetRotation() const { return LocalTransform.GetRotation(); }
	const glm::vec3& GetDirection() const { return LocalTransform.GetDirection(); }
	const glm::vec3& GetScale() const { return LocalTransform.GetScale(); }

	void SetTransform(const BravoTransform& InTransform)
	{
		LocalTransform = InTransform;
		UpdateWorldTransform();
		OnTransformUpdated.Broadcast(this);
	}
	void SetLocation(const glm::vec3& InLocation)
	{
		LocalTransform.SetLocation(InLocation);
		UpdateWorldTransform();
		OnTransformUpdated.Broadcast(this);
	}
	void SetRotation(const glm::vec3& InRotation)
	{
		LocalTransform.SetRotation(InRotation);
		UpdateWorldTransform();
		OnTransformUpdated.Broadcast(this);
	}
	void SetDirection(const glm::vec3& Direction)
	{
		LocalTransform.SetDirection(Direction);
		UpdateWorldTransform();
		OnTransformUpdated.Broadcast(this);
	}
	void SetScale(const glm::vec3& InScale)
	{
		LocalTransform.SetScale(InScale);
		UpdateWorldTransform();
		OnTransformUpdated.Broadcast(this);
	}

	const BravoTransform& GetTransform_World() const
	{
		return WorldTransform;
		
	}
	const glm::vec3& GetLocation_World() const { return WorldTransform.GetLocation(); }
	const glm::vec3& GetRotation_World() const { return WorldTransform.GetRotation(); }
	const glm::vec3& GetDirection_World() const { return WorldTransform.GetDirection(); }
	const glm::vec3& GetScale_World() const { return WorldTransform.GetScale(); }


	void SetTransform_World(const BravoTransform& InTransform)
	{
		if ( std::shared_ptr<ITransformable> p = GetParent() )
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
		if ( std::shared_ptr<ITransformable> p = GetParent() )
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

	void OnParentTransformUpdated(const ITransformable* _parent)
	{
		UpdateWorldTransform();
	}

private:
	BravoTransform LocalTransform;
	BravoTransform WorldTransform;

	OnTransformUpdatedSignature OnTransformUpdated;


	std::weak_ptr<ITransformable> Parent;
};