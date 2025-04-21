#include "BravoActor.h"
#include "BravoEngine.h"
#include "BravoMath.h"
#include "BravoComponent.h"
#include "IBravoRenderable.h"


RTTR_REGISTRATION
{
	using namespace rttr;
	registration::class_<BravoActor>("Actor")
		.property("Components", &BravoActor::Components)
		;
}

void BravoActor::OnChildObjectAdded(std::weak_ptr<BravoObject> _OwnedObject)
{
	BravoObject::OnChildObjectAdded(_OwnedObject);

	if ( _OwnedObject.expired() )
		return;
	std::shared_ptr<BravoObject> o = _OwnedObject.lock();
	if ( std::shared_ptr<BravoComponent> oc = std::dynamic_pointer_cast<BravoComponent>(o) )
	{
		Components.push_back(oc);
	}
}

void BravoActor::OnDestroy()
{
	Components.clear();
	BravoObject::OnDestroy();
}

void BravoActor::SetVisisble(bool bNewVisible, bool bPropagateToChildren)
{
	bVisible = bNewVisible;
	if ( bPropagateToChildren )
	{
		for ( auto it : Components )
		{
			if ( std::shared_ptr<IBravoRenderable> asRenderable = std::dynamic_pointer_cast<IBravoRenderable>(it) )
			{
				asRenderable->SetVisisble(bNewVisible);
			}
		}
	}
}