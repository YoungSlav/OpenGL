#include "stdafx.h"
#include "BravoScreen.h"

class FluidScreen_Simulation : public BravoScreen
{
public:
	template <typename... Args>
	FluidScreen_Simulation(std::shared_ptr<class FluidSimulation> _Simulation, Args&&... args) :
		BravoScreen(std::forward<Args>(args)...),
		Simulation(_Simulation)
	{}



protected:
	virtual bool Initialize_Internal() override;
	virtual void Render_Internal(float DeltaTime) override;

private:
	void OnToggleHUD(bool ButtonState, float DeltaTime);

private:
	std::shared_ptr<class FluidSimulation> Simulation = nullptr;

	bool bShowHUD = true;
};
