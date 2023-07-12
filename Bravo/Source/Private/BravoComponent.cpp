#include "BravoComponent.h"
#include "BravoActor.h"

bool BravoComponent::Initialize_Internal()
{
	if ( !BravoObject::Initialize_Internal() )
		return false;

	if ( std::shared_ptr<BravoObject> Owner = GetOwner() )
	{
		if ( std::shared_ptr<BravoActor> AsActor = std::dynamic_pointer_cast<BravoActor>(Owner) )
		{
			return true;
		}
	}
	Log::LogMessage("Owner of the component " + GetName() + " is not an actor! All components must be owned by an actor!", ELog::Error);
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

glm::vec3 BravoComponent::GetLocation_World() const
{
	return Transform.LocalToWorld(GetOwningActor()->GetTransform()).GetLocation();
}

glm::vec3 BravoComponent::GetRotation_World() const
{
	return Transform.LocalToWorld(GetOwningActor()->GetTransform()).GetRotation();
}
	
glm::vec3 BravoComponent::GetDirection_World() const
{
	return Transform.LocalToWorld(GetOwningActor()->GetTransform()).GetDirection();
}

glm::vec3 BravoComponent::GetScale_World() const
{
	return Transform.LocalToWorld(GetOwningActor()->GetTransform()).GetScale();
}

const BravoTransform& BravoComponent::GetTransform_World() const
{
	return Transform.LocalToWorld(GetOwningActor()->GetTransform());
}