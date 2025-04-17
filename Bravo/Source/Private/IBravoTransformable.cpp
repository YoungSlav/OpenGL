#include "IBravoTransformable.h"


#include "BravoReflection.h"

RTTR_REGISTRATION
{
	using namespace rttr;
	registration::class_<IBravoTransformable>("ITransformable")
        .property("Transform", &IBravoTransformable::GetTransform, &IBravoTransformable::SetTransform)
        ;
}

IBravoTransformable::IBravoTransformable(const BravoTransform& Transform) : 
	LocalTransform(Transform),
	WorldTransform(Transform)
{
}

IBravoTransformable::IBravoTransformable(const BravoTransform& Transform, const std::shared_ptr<IBravoTransformable> _Parent) :
	LocalTransform(Transform),
	WorldTransform()
{
	SetParent(_Parent);
}
IBravoTransformable::~IBravoTransformable()
{
	if ( std::shared_ptr<IBravoTransformable> parent = GetParent() )
	{
		parent->OnTransformUpdated.RemoveObject(this);
	}
}

void IBravoTransformable::SetParent(const std::shared_ptr<IBravoTransformable> _Parent)
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
std::shared_ptr<IBravoTransformable> IBravoTransformable::GetParent() const { return ParentPtr.expired() ? nullptr : ParentPtr.lock(); }

const BravoTransform& IBravoTransformable::GetTransform() const { return LocalTransform; }
const glm::vec3& IBravoTransformable::GetLocation() const { return LocalTransform.GetLocation(); }
const glm::quat& IBravoTransformable::GetRotation() const { return LocalTransform.GetRotation(); }
const glm::vec3& IBravoTransformable::GetScale() const { return LocalTransform.GetScale(); }
glm::vec3 IBravoTransformable::GetForwardVector() const { return LocalTransform.GetForwardVector(); }
glm::vec3 IBravoTransformable::GetRightVector() const { return LocalTransform.GetRightVector(); }
glm::vec3 IBravoTransformable::GetUpVector() const { return LocalTransform.GetUpVector(); }

void IBravoTransformable::SetTransform(const BravoTransform& InTransform)
{
	LocalTransform = InTransform;
	UpdateWorldTransform();
	OnTransformUpdated.Broadcast(this);
}
void IBravoTransformable::SetLocation(const glm::vec3& InLocation)
{
	LocalTransform.SetLocation(InLocation);
	UpdateWorldTransform();
	OnTransformUpdated.Broadcast(this);
}
void IBravoTransformable::SetRotation(const glm::quat& InRotation)
{
	LocalTransform.SetRotation(InRotation);
	UpdateWorldTransform();
	OnTransformUpdated.Broadcast(this);
}
void IBravoTransformable::SetRotation(const glm::vec3& InRotation)
{
	LocalTransform.SetRotationEuler(InRotation);
	UpdateWorldTransform();
	OnTransformUpdated.Broadcast(this);
}
void IBravoTransformable::SetDirection(const glm::vec3& Direction)
{
	LocalTransform.SetDirection(Direction);
	UpdateWorldTransform();
	OnTransformUpdated.Broadcast(this);
}
void IBravoTransformable::Rotate(const glm::quat& InRotation)
{
	LocalTransform.Rotate(InRotation);
	UpdateWorldTransform();
	OnTransformUpdated.Broadcast(this);
}
void IBravoTransformable::SetScale(const glm::vec3& InScale)
{
	LocalTransform.SetScale(InScale);
	UpdateWorldTransform();
	OnTransformUpdated.Broadcast(this);
}

const BravoTransform& IBravoTransformable::GetTransform_World() const
{
	return WorldTransform;
}

const glm::vec3& IBravoTransformable::GetLocation_World() const { return WorldTransform.GetLocation(); }
const glm::quat& IBravoTransformable::GetRotation_World() const { return WorldTransform.GetRotation(); }
const glm::vec3& IBravoTransformable::GetScale_World() const { return WorldTransform.GetScale(); }
glm::vec3 IBravoTransformable::GetForwardVector_World() const { return WorldTransform.GetForwardVector(); }
glm::vec3 IBravoTransformable::GetRightVector_World() const { return WorldTransform.GetRightVector(); }
glm::vec3 IBravoTransformable::GetUpVector_World() const { return WorldTransform.GetUpVector(); }


void IBravoTransformable::SetTransform_World(const BravoTransform& InTransform)
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
void IBravoTransformable::SetLocation_World(const glm::vec3& InLocation)
{
	BravoTransform worldTransform = GetTransform_World();
	worldTransform.SetLocation(InLocation);
	SetTransform_World(worldTransform);
}
void IBravoTransformable::SetRotation_World(const glm::vec3& InRotation)
{
	BravoTransform worldTransform = GetTransform_World();
	worldTransform.SetRotationEuler(InRotation);
	SetTransform_World(worldTransform);
}
void IBravoTransformable::SetRotation_World(const glm::quat& InRotation)
{
	BravoTransform worldTransform = GetTransform_World();
	worldTransform.SetRotation(InRotation);
	SetTransform_World(worldTransform);
}
void IBravoTransformable::SetDirection_World(const glm::vec3& Direction)
{
	BravoTransform worldTransform = GetTransform_World();
	worldTransform.SetDirection(Direction);
	SetTransform_World(worldTransform);
}
void IBravoTransformable::SetScale_World(const glm::vec3& InScale)
{
	BravoTransform worldTransform = GetTransform_World();
	worldTransform.SetScale(InScale);
	SetTransform_World(worldTransform);
}


void IBravoTransformable::UpdateWorldTransform()
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

void IBravoTransformable::OnParentTransformUpdated(const IBravoTransformable* _parent)
{
	UpdateWorldTransform();
}