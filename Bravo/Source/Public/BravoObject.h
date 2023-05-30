#pragma once

#include "stdafx.h"
#include "SharedFromThis.h"


class BravoEngine;

class BravoTickable
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
public:
	BravoObject() = default;
	bool Initialize(const BravoHandle& _Handle, const std::string& _Name, std::shared_ptr<class BravoEngine> _Engine, std::shared_ptr<BravoObject> _Owner);
	
	void Destroy();

	const BravoHandle& GetHandle() const { return Handle; }
	const std::shared_ptr<BravoObject> GetOwner() const { return Owner.expired() ? nullptr : Owner.lock(); }
	const std::string& GetName() const { return Name; }

protected:
	virtual bool Initialize_Internal() { return true; }
	virtual void OnDestroy() {}

	template <typename Class>
	std::shared_ptr<Class> NewObject(const std::string& _Name = "", std::shared_ptr<BravoObject> _Owner = nullptr)
	{
		static_assert(std::is_base_of_v<BravoObject, Class>);
		
		if ( !Engine )
			return nullptr;

		BravoHandle newHandle = Engine->GenerateNewHandle();

		std::shared_ptr<BravoObject> newObject = std::shared_ptr<BravoObject>(new Class());

		if ( !newObject->Initialize(
						newHandle,
						_Name.empty() ? std::to_string(newHandle): _Name,
						Engine, 
						_Owner ? _Owner : Self<BravoObject>() )
			)
			return nullptr;
		Engine->RegisterObject(newObject);

		return std::dynamic_pointer_cast<Class>(newObject);
	}

private:

protected:
	std::shared_ptr<BravoEngine> Engine;

private:
	std::weak_ptr<BravoObject> Owner;
	std::string Name = "";

	BravoHandle Handle = 0;
};