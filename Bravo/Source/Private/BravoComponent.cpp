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

glm::mat4 BravoComponent::GetTransformMatrix_Local() const
{
	glm::mat4 transform;
	transform = glm::translate(transform, Location);
	transform = transform * glm::toMat4(BravoMath::EulerToQuat(Rotation));
	transform = glm::scale(transform, Scale);
	return transform;
}
glm::mat4 BravoComponent::GetTransformMatrix_World() const
{
	glm::mat4 transform = GetOwningActor()->GetTransformMatrix();
	transform = glm::translate(transform, Location);
	transform = transform * glm::toMat4(BravoMath::EulerToQuat(Rotation));
	transform = glm::scale(transform, Scale);
	return transform;
}