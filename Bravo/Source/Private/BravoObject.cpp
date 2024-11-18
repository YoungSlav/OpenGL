#include "BravoObject.h"
#include "BravoEngine.h"

#if TEST_MEMORY_LEAKS
int32 ObjectsSpawned = 0;
#endif

BravoObject::~BravoObject()
{
#if TEST_MEMORY_LEAKS
	ObjectsSpawned--;
	Log::LogMessage(ELog::Warning, "Object {} destroyed! Total object count: {}", Name, ObjectsSpawned);
#endif
}

bool BravoObject::InitializeNewObject(std::shared_ptr<BravoObject> newObject)
{
	if ( !newObject->Initialize() )
		return false;

	AddChildObject(newObject);
	Engine->RegisterObject(newObject);

	return true;
}

BravoHandle BravoObject::NewHandle() const
{
	return Engine->GenerateNewHandle();
}


bool BravoObject::Initialize()
{

#if	TEST_MEMORY_LEAKS
	ObjectsSpawned++;
	Log::LogMessage(ELog::Warning, "Object {} created! Total object count: {}", Name, ObjectsSpawned );
#endif

	return Initialize_Internal();
}

void BravoObject::AddChildObject(std::weak_ptr<BravoObject> _OwnedObject)
{
	OwnedObjects.push_back(_OwnedObject);
	OnChildObjectAdded(_OwnedObject);
}

void BravoObject::Destroy()
{
	OnObjectDestroyDelegate.Broadcast(Self<BravoObject>());

	for( auto it : OwnedObjects )
	{
		if ( it.expired() )
			continue;
		it.lock()->Destroy();
	}
	OwnedObjects.clear();

	if ( Engine )
	{
		OnDestroy();
		Engine->DestroyObject(Self<BravoObject>());
		Engine.reset();
	}
}