#include "BravoComponent.h"
#include "BravoActor.h"

bool BravoComponent::Initialize_Internal()
{
	if ( !BravoObject::Initialize_Internal() )
		return false;

	if ( std::shared_ptr<BravoObject> Owner = GetOwner() )
	{
		if ( std::shared_ptr<ITransformable> asTranformable = std::dynamic_pointer_cast<ITransformable>(Owner) )
		{
			return true;
		}
	}
	Log::LogMessage("Owner of the component " + GetName() + " is not an actor! All components must be owned by an actor or another component!", ELog::Error);
	return false;
}

std::shared_ptr<BravoActor> BravoComponent::GetOwningActor() const
{
	if ( std::shared_ptr<BravoObject> Owner = GetOwner() )
	{
		if ( std::shared_ptr<BravoActor> AsActor = std::dynamic_pointer_cast<BravoActor>(Owner) )
		{
			return AsActor;
		}
	}
	return nullptr;
}

BravoTransform BravoComponent::GetTransform_World() const
{
	if ( std::shared_ptr<ITransformable> asTransformable = std::dynamic_pointer_cast<ITransformable>(GetOwner()) )
	{
		BravoTransform ownerTransform = asTransformable->GetTransform_World();
		BravoTransform worldTransform(ownerTransform.GetTransformMatrix() * Transform.GetTransformMatrix());
		return worldTransform;
	}
	else
	{
		return GetTransform();
	}
}


void BravoComponent::SetTransform_World(const BravoTransform& InTransform)
{
	if ( std::shared_ptr<ITransformable> asTransformable = std::dynamic_pointer_cast<ITransformable>(GetOwner()) )
	{
		BravoTransform ownerTransform = asTransformable->GetTransform_World();
		glm::mat4 parentWorldInverse = glm::inverse(ownerTransform.GetTransformMatrix());
		glm::mat4 localTransform = parentWorldInverse * InTransform.GetTransformMatrix();
		Transform.SetTransformMatrix(localTransform);
	}
	else
	{
		SetTransform(InTransform);
	}
}