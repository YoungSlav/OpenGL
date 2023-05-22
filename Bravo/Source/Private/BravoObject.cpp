#include "BravoObject.h"
#include "BravoEngine.h"

bool BravoObject::Initialize()
{
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