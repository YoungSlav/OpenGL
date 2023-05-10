#include "BravoObject.h"
#include "BravoEngine.h"

void BravoObject::Initialize()
{
	Initialize_Internal();
}

void BravoObject::Destroy()
{
	GetEngine()->DestroyObject(Self<BravoObject>());
}