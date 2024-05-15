#pragma once

#include "stdafx.h"
#include "SharedFromThis.h"


class BravoEngine;

class IBravoTickable
{
	friend class BravoEngine;
protected:
	virtual void Tick(float DeltaTime) {};

private:
	void Update(float DeltaTime)
	{
		LifeTime += DeltaTime;
		Tick(DeltaTime);
	}

protected:
	float LifeTime = 0.0f;
};

class BravoObject : public SharedFromThis
{
	friend class BravoEngine;
public:
	BravoObject() = default;
	~BravoObject();

	bool Initialize(const BravoHandle& _Handle, const std::string& _Name, std::shared_ptr<class BravoEngine> _Engine, std::shared_ptr<BravoObject> _Owner);
	
	void Destroy();

	const BravoHandle& GetHandle() const { return Handle; }
	const std::shared_ptr<BravoObject> GetOwner() const { return Owner.expired() ? nullptr : Owner.lock(); }
	const std::string& GetName() const { return Name; }

	template <typename Class, typename... Args>
	std::shared_ptr<Class> NewObject(const std::string& _Name = "", Args&&... args)
	{
		static_assert(std::is_base_of_v<BravoObject, Class>);
		
		if ( !Engine )
			return nullptr;

		std::shared_ptr<BravoObject> newObject = std::shared_ptr<BravoObject>(new Class(std::forward<Args>(args)...));

		if ( !InitializeNewObject(newObject, _Name) )
			return nullptr;

		return std::dynamic_pointer_cast<Class>(newObject);
	}

protected:
	virtual bool Initialize_Internal() { return true; }
	virtual void OnChildObjectAdded(std::weak_ptr<BravoObject> _OwnedObject) {}
	virtual void OnDestroy() {}
	virtual void OnBeginPlay() {}

private:
	
	bool InitializeNewObject(std::shared_ptr<BravoObject> obj, const std::string& _Name);

	void AddChildObject(std::weak_ptr<BravoObject> _OwnedObject);

protected:
	std::shared_ptr<class BravoEngine> Engine;

private:
	std::weak_ptr<BravoObject> Owner;
	std::list<std::weak_ptr<BravoObject>> OwnedObjects;
	std::string Name = "";

	BravoHandle Handle = 0;
};