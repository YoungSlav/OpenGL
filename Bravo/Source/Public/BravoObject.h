#pragma once

#include "stdafx.h"
#include "SharedFromThis.h"


class BravoEngine;

class IBravoTickable
{
	friend class BravoEngine;
public:
	virtual ~IBravoTickable() = default;

protected:
	virtual void Tick(float DeltaTime) = 0;

private:
	void Update(float DeltaTime)
	{
		LifeTime += DeltaTime;
		Tick(DeltaTime);
	}

protected:
	float LifeTime = 0.0f;
};

typedef MulticastDelegate<const std::shared_ptr<class BravoObject>> OnObjectDestroySignature;

class BravoObject : public SharedFromThis
{
	RTTR_ENABLE();

	friend class BravoEngine;
public:
	BravoObject(const BravoHandle& _Handle, const std::string& _Name, const std::shared_ptr<class BravoEngine> _Engine, const std::shared_ptr<BravoObject> _Owner) :
		SharedFromThis(),
		Handle(_Handle),
		Name(_Name),
		Engine(_Engine),
		Owner(_Owner)
	{}

	virtual ~BravoObject();

	
	bool Initialize();
	void Destroy();

	const BravoHandle& GetHandle() const { return Handle; }
	const std::shared_ptr<BravoObject> GetOwner() const { return Owner.expired() ? nullptr : Owner.lock(); }
	const std::list<std::weak_ptr<BravoObject>>& GetChildren() const { return OwnedObjects; }

	void SetName(const std::string& _Name) { Name = _Name; }
	const std::string& GetName() const { return Name; }

	template <typename Class, typename... Args>
	std::shared_ptr<Class> NewObject(const std::string& _Name = "", Args&&... args)
	{
		static_assert(std::is_base_of_v<BravoObject, Class>);
		
		if ( !Engine )
			return nullptr;
		
		BravoHandle newHandle = NewHandle();
		std::shared_ptr<BravoObject> newObject = std::shared_ptr<BravoObject>(new Class(std::forward<Args>(args)... ,
			newHandle,
			_Name.empty() ? std::to_string(newHandle): _Name,
			Engine, 
			Self<BravoObject>())
		);

		if ( !InitializeNewObject(newObject) )
		{
			assert(false);
			return nullptr;
		}

		return std::dynamic_pointer_cast<Class>(newObject);
	}

	OnObjectDestroySignature OnObjectDestroyDelegate;

protected:
	virtual bool Initialize_Internal() { return true; }
	virtual void OnChildObjectAdded(std::weak_ptr<BravoObject> _OwnedObject) {}
	virtual void OnDestroy() {}
	virtual void OnBeginPlay() {}

private:
	
	BravoHandle NewHandle() const;
	bool InitializeNewObject(std::shared_ptr<BravoObject> obj);

	void AddChildObject(std::weak_ptr<BravoObject> _OwnedObject);

protected:
	std::shared_ptr<class BravoEngine> Engine;

private:
	std::string Name = "";
	
	const BravoHandle Handle = 0;

	const std::weak_ptr<BravoObject> Owner;
	std::list<std::weak_ptr<BravoObject>> OwnedObjects;
};