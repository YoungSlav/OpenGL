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

void BravoActor::OnChildObjectAdded(std::shared_ptr<BravoObject> _OwnedObject)
{
	BravoObject::OnChildObjectAdded(_OwnedObject);

	if ( std::shared_ptr<BravoComponent> oc = std::dynamic_pointer_cast<BravoComponent>(_OwnedObject) )
	{
		Components.push_back(oc);
	}
}

void BravoActor::OnChildObjectRemoved(std::shared_ptr<BravoObject> _OwnedObject)
{
	BravoObject::OnChildObjectRemoved(_OwnedObject);

	if ( std::shared_ptr<BravoComponent> oc = std::dynamic_pointer_cast<BravoComponent>(_OwnedObject) )
	{
		std::erase(Components, oc);
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