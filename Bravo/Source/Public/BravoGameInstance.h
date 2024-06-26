#pragma once

#include "stdafx.h"
#include "BravoObject.h"

class BravoGameInstance : public BravoObject, public IBravoTickable
{
public:
	template <typename... Args>
	BravoGameInstance(Args&&... args) :
		BravoObject(std::forward<Args>(args)...),
		IBravoTickable()
	{}

protected:
	virtual bool Initialize_Internal() override;
	virtual void Tick(float DeltaTime) override;

	void TestTransforms();

	void TestCompTransform(float DeltaTime);

	void SpawnDirLights();
	void SpawnSpotLights();
	void SpawnPointLights();
	void SpawnSpheres();
	void SpawnTestInstances();

	std::vector<std::shared_ptr<class BravoSpotLightActor>> spotLights;
	std::vector<std::shared_ptr<class BravoPointLightActor>> pointLights;
	std::vector<std::weak_ptr<class BravoActor>> Cubes;
	std::shared_ptr<class BravoStaticMeshComponent> Mesh;

	std::shared_ptr<class BravoPointLightActor> pbrLight;

	std::shared_ptr<class BravoCamera> Camera;
	std::shared_ptr<class BravoPlayer> Player;

	std::shared_ptr<class BravoActor> TestActor;
	std::shared_ptr<class BravoStaticMeshComponent> TestComponent;
};