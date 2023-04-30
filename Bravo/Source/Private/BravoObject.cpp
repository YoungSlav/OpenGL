#include "BravoObject.h"
#include "BravoEngine.h"

void BravoObject::PreInit(class BravoEngine* InEngine, BravoObjectHandle InHandle)
{
	Handle = InHandle;
	Engine = InEngine;
}

void BravoObject::Destroy()
{
	if ( Engine )
		Engine->DestroyObject(Handle);
}
