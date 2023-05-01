#include "BravoObject.h"
#include "BravoEngine.h"


void BravoObject::Destroy()
{
	GetEngine()->DestroyObject(Self<BravoObject>());
}