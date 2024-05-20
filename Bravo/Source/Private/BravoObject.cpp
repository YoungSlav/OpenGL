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

bool BravoObject::InitializeNewObject(std::shared_ptr<BravoObject> newObject, const std::string& _Name)
{
	BravoHandle newHandle = Engine->GenerateNewHandle();
	if ( !newObject->Initialize(
					newHandle,
					_Name.empty() ? std::to_string(newHandle): _Name,
					Engine, 
					Self<BravoObject>() )
		)
		return false;

	AddChildObject(newObject);
	Engine->RegisterObject(newObject);

	return true;
}


bool BravoObject::Initialize(const BravoHandle& _Handle, const std::string& _Name, std::shared_ptr<class BravoEngine> _Engine, std::shared_ptr<BravoObject> _Owner)
{
	Handle = _Handle;
	Name = _Name;
	Engine = _Engine;
	Owner = _Owner;

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