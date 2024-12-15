#pragma once
//FluidScreen_PostProcessSettings
#include "stdafx.h"
#include "BravoScreen.h"

class FluidScreen_PostProcessSettings : public BravoScreen
{
public:
	template <typename... Args>
	FluidScreen_PostProcessSettings(std::shared_ptr<class FluidPostProcess> _PostProcess, Args&&... args) :
		BravoScreen(std::forward<Args>(args)...),
		PostProcess(_PostProcess)
	{}



protected:
	virtual bool Initialize_Internal() override;
	virtual void Render_Internal(float DeltaTime) override;

private:
	void OnToggleHUD(bool ButtonState, float DeltaTime);

	bool Slider(const std::string& Label, float* value, float min, float max);
	bool Slider(const std::string& Label, int32* value, int32 min, int32 max);
	void Spacing();

private:
	std::shared_ptr<class FluidPostProcess> PostProcess = nullptr;

	
	bool bShowHUD = true;

	float SliderWidth = 1.0f;
	float BtnWidth = 1.0f;
};
