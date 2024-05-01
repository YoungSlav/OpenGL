#pragma once

#include "stdafx.h"
#include "BravoObject.h"

class BravoGameInstance : public BravoObject, public IBravoTickable
{
public:

protected:
	virtual bool Initialize_Internal() override;
	virtual void Tick(float DeltaTime) override;

	void SpawnDirLights();
	void SpawnSpotLights();
	void SpawnPointLights();
	void SpawnCubes();
	void SpawnTestInstances();
	void SpawnTriangles();
	void SpawnTriangles_Inst();

	std::vector<std::shared_ptr<class BravoSpotLightActor>> spotLights;
	std::vector<std::shared_ptr<class BravoPointLightActor>> pointLights;
	std::vector<std::weak_ptr<class BravoActor>> Cubes;
	std::shared_ptr<class BravoStaticMeshComponent> Mesh;

	std::shared_ptr<class BravoPointLightActor> pbrLight;
};