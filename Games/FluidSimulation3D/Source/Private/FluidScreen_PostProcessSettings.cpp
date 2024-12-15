#include "FluidScreen_PostProcessSettings.h"
#include "BravoInput.h"
#include "BravoEngine.h"
#include "FluidSimulation3D.h"
#include "FluidPostProcess.h"


bool FluidScreen_PostProcessSettings::Initialize_Internal()
{
	if ( !BravoScreen::Initialize_Internal() )
		return false;

	SetTrueScaling(true);
	SetSize(glm::vec2(0.3f, 2.0f));
	SetOrigin(glm::vec2(1.0f, 0.0f));
	SetPosition(glm::vec2(0.99f, 0.0f));

	if ( Engine->GetInput() )
	{
		BravoKeySubscription sub;
		sub.Key = GLFW_KEY_GRAVE_ACCENT;
		sub.SubscribedType = EKeySubscriptionType::Released;
		sub.Callback.BindSP(Self<FluidScreen_PostProcessSettings>(), &FluidScreen_PostProcessSettings::OnToggleHUD);
		Engine->GetInput()->SubscribeKey(sub);
	}


	glm::vec2 trueSize = GetTrueSize();
	SliderWidth = trueSize.x;
	BtnWidth = trueSize.x;
	
	return true;
}

void FluidScreen_PostProcessSettings::Spacing()
{
	ImGui::Dummy(ImVec2(0.0f, 10.0f));
}

bool FluidScreen_PostProcessSettings::Slider(const std::string& Label, int32* value, int32 min, int32 max)
{
	ImGui::SetNextItemWidth(SliderWidth);
	ImGui::Text(Label.c_str());

	ImGui::SetNextItemWidth(SliderWidth * 0.65f);
	std::string sliderName = "##slider" + Label;
	bool retSlider = ImGui::SliderInt(sliderName.c_str(), value, min, max);
	ImGui::SameLine();
	std::string inputName = "##input" + Label;
	bool retInput = ImGui::InputInt(inputName.c_str(), value, 1, 10);

	return retSlider || retInput;
}

bool FluidScreen_PostProcessSettings::Slider(const std::string& Label, float* value, float min, float max)
{
	ImGui::SetNextItemWidth(SliderWidth);
	ImGui::Text(Label.c_str());
	
	ImGui::SetNextItemWidth(SliderWidth * 0.65f);
	std::string sliderName = "##slider" + Label;
	bool retSlider = ImGui::SliderFloat(sliderName.c_str(), value, min, max);
	ImGui::SameLine();
	std::string inputName = "##input" + Label;
	bool retInput = ImGui::InputFloat(inputName.c_str(), value, 0.01f, 100.0f);

	return retSlider || retInput;
}

void FluidScreen_PostProcessSettings::Render_Internal(float DeltaTime)
{
	if ( !bShowHUD )
		return;

	assert(PostProcess != nullptr );

	

	ImGui::GetStyle().ScaleAllSizes(0.5f);
	BravoScreen::Render_Internal(DeltaTime);
	ImGui::Begin("Post process", nullptr,
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_AlwaysVerticalScrollbar );

		Spacing();
		Slider("DensitiesResolution", &PostProcess->DensitiesResolution, 0.001, 1.0);

		Spacing();
		Slider("Density offset", &PostProcess->DensityOffset, 0, 1000);

		Spacing();
		Slider("Density multiplier", &PostProcess->DensityMultiplier, 0, 1.0);

		Spacing();
		Slider("Marching ray step", &PostProcess->MarchingRayStep, 0.001, 1.0);

		Spacing();
		Slider("Light marching step", &PostProcess->LightMarchingRayStep, 0.1, 1.0);

		Spacing();
		Slider("Scattering Red", &PostProcess->ScatteringCoefficients.x, 0.001, 10.0);

		Spacing();
		Slider("Scattering Greed", &PostProcess->ScatteringCoefficients.y, 0.001, 10.0);

		Spacing();
		Slider("Scattering Blue", &PostProcess->ScatteringCoefficients.z, 0.001, 10.0);

		Spacing();
		Slider("SurfaceStepsTreashold", &PostProcess->SurfaceStepsTreashold, 1.0f, 1000000.0f);

	ImGui::End();
	ImGui::GetStyle().ScaleAllSizes(2.0f);
}

void FluidScreen_PostProcessSettings::OnToggleHUD(bool ButtonState, float DeltaTime)
{
	bShowHUD = !bShowHUD;
}