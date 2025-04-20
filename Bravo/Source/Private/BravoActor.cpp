#include "BravoActor.h"
#include "BravoEngine.h"
#include "BravoMath.h"
#include "BravoComponent.h"



RTTR_REGISTRATION
{
	using namespace rttr;
	registration::class_<BravoActor>("Actor")
		.property("Components", &BravoActor::Components)
		//.property("tmp", &BravoActor::tmp)
		;
}

void BravoActor::OnChildObjectAdded(std::weak_ptr<BravoObject> _OwnedObject)
{
	BravoObject::OnChildObjectAdded(_OwnedObject);

	if ( _OwnedObject.expired() )
		return;
	std::shared_ptr<BravoObject> o = _OwnedObject.lock();
	if ( std::shared_ptr<BravoComponent> oc = std::dynamic_pointer_cast<BravoComponent>(o) )
		Components.push_back(oc);
}

void BravoActor::OnDestroy()
{
	Components.clear();
	BravoObject::OnDestroy();
}