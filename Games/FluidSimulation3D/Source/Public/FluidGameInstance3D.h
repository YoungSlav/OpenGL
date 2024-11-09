#pragma once
#include "stdafx.h"
#include "BravoObject.h"

class FluidGameInstance3D : public BravoObject
{
public:
	template <typename... Args>
	FluidGameInstance3D(Args&&... args) :
		BravoObject(std::forward<Args>(args)...)
	{}

	

protected:
	virtual bool Initialize_Internal() override;

private:

private:
	
	std::shared_ptr<class BravoPlayer> Player = nullptr;
	std::shared_ptr<class BravoCamera> Camera = nullptr;
	std::shared_ptr<class FluidSimulation3D> Simulation = nullptr;

	const float WorldHeight = 10.0f;
};
