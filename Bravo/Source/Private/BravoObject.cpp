#include "BravoObject.h"
#include "BravoEngine.h"

bool BravoObject::Initialize(const BravoHandle& _Handle, const std::string& _Name, std::shared_ptr<class BravoEngine> _Engine, std::shared_ptr<BravoObject> _Owner)
{
	Handle = _Handle;
	Name = _Name;
	Engine = _Engine;
	Owner = _Owner;

	return Initialize_Internal();
}

void BravoObject::Destroy()
{
	if ( Engine )
	{
		OnDestroy();
		Engine->DestroyObject(Self<BravoObject>());
	}
}