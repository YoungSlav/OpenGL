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
			SetParent(asTranformable);
			return true;
		}
	}
	Log::LogMessage(ELog::Error, "Owner of the component {} is not an actor! All components must be owned by an actor or another component!", GetName());
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