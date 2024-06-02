#pragma once
#include "stdafx.h"
#include "BravoPostProcess.h"

class FluidPostProcess_Density : public BravoPostProcess
{
public:
	template <typename... Args>
	FluidPostProcess_Density(std::shared_ptr<class FluidSimulation> _Simulation, std::shared_ptr<class FluidContainer> _Container, Args&&... args) :
		BravoPostProcess(std::forward<Args>(args)...),
		Simulation(_Simulation),
		Container(_Container)
	{}

protected:
	virtual bool Initialize_Internal() override;
	virtual void Render_Internal() override;

private:
	std::shared_ptr<FluidSimulation> Simulation = nullptr;
	std::shared_ptr<class FluidContainer> Container = nullptr;
};

