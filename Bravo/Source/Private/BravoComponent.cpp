#include "BravoComponent.h"
#include "BravoActor.h"

void BravoComponent::AttachToActor(std::shared_ptr<class BravoActor> _Actor)
{
	if ( !OwnerActor.expired() && _Actor == OwnerActor.lock() )
		return;

	DetachFromActor();

	OwnerActor = _Actor;
	_Actor->AddComponent(Self<BravoComponent>());
}

void BravoComponent::DetachFromActor()
{
	if ( OwnerActor.expired() )
		return;
	OwnerActor.lock()->RemoveComponent(Self<BravoComponent>());
	OwnerActor.reset();
}

glm::mat4 BravoComponent::GetTransform() const
{
	glm::mat4 transform;
	transform = glm::translate(transform, Location);
	transform = transform * glm::toMat4(BravoMath::EulerToQuat(Rotation));
	transform = glm::scale(transform, Scale);
	return transform;
}

void BravoComponent::OnDestroy()
{
	BravoObject::OnDestroy();

	if( OwnerActor.expired() )
		return;

	OwnerActor.lock()->RemoveComponent(Self<BravoComponent>());
}