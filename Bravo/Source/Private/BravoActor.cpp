#include "BravoActor.h"
#include "BravoEngine.h"
#include "BravoMath.h"

#include "BravoComponent.h"

glm::mat4 BravoActor::GetTransform() const
{
	glm::mat4 transform;
	transform = glm::translate(transform, Location);
	transform = transform * glm::toMat4(BravoMath::EulerToQuat(Rotation));
	transform = glm::scale(transform, Scale);
	return transform;
}

void BravoActor::AddComponent(std::shared_ptr<BravoComponent> _Component)
{
	for ( auto it : Components )
	{
		if ( it == _Component)
			return;
	}
	Components.push_back(_Component);
}

void BravoActor::RemoveComponent(std::shared_ptr<BravoComponent> _Component)
{
	Components.erase(std::remove(Components.begin(), Components.end(), _Component), Components.end());
}


void BravoActor::OnDestroy()
{
	BravoObject::OnDestroy();

	for ( int32 i = (int32)Components.size()-1; i >= 0; --i )
	{
		Components[i]->Destroy();
	}
}