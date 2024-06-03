#pragma once
#include "stdafx.h"
#include "BravoObject.h"

class FluidGameInstance : public BravoObject, public IBravoTickable
{
public:
	template <typename... Args>
	FluidGameInstance(Args&&... args) :
		BravoObject(std::forward<Args>(args)...),
		IBravoTickable()
	{}

	

protected:
	virtual bool Initialize_Internal() override;
	virtual void Tick(float DeltaTime) override;

private:
	void OnViewportResize(const glm::ivec2& NewSize);

private:
	std::shared_ptr<class BravoCamera> Camera = nullptr;

	std::shared_ptr<class FluidContainer> Container = nullptr;
	std::shared_ptr<class FluidSimulation> Simulation = nullptr;

	const float WorldHeight = 10.0f;
};
