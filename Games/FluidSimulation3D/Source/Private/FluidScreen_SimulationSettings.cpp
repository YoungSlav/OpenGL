#include "FluidScreen_SimulationSettings.h"
#include "BravoInput.h"
#include "BravoEngine.h"
#include "FluidSimulation3D.h"
#include "FluidPostProcess.h"


bool FluidScreen_SimulationSettings::Initialize_Internal()
{
	if ( !BravoScreen::Initialize_Internal() )
		return false;

	SetTrueScaling(true);
	SetSize(glm::vec2(0.3f, 2.0f));
	SetOrigin(glm::vec2(0.0f, 0.0f));
	SetPosition(glm::vec2(0.01f, 0.0f));

	if ( Engine->GetInput() )
	{
		BravoKeySubscription sub;
		sub.Key = GLFW_KEY_GRAVE_ACCENT;
		sub.SubscribedType = EKeySubscriptionType::Released;
		sub.Callback.BindSP(Self<FluidScreen_SimulationSettings>(), &FluidScreen_SimulationSettings::OnToggleHUD);
		Engine->GetInput()->SubscribeKey(sub);
	}


	glm::vec2 trueSize = GetTrueSize();
	SliderWidth = trueSize.x;
	BtnWidth = trueSize.x;
	
	return true;
}

void FluidScreen_SimulationSettings::Spacing()
{
	ImGui::Dummy(ImVec2(0.0f, 10.0f));
}

bool FluidScreen_SimulationSettings::Slider(const std::string& Label, int32* value, int32 min, int32 max)
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

bool FluidScreen_SimulationSettings::Slider(const std::string& Label, float* value, float min, float max)
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

void FluidScreen_SimulationSettings::Render_Internal(float DeltaTime)
{
	if ( !bShowHUD )
		return;

	assert(Simulation != nullptr );

	

	ImGui::GetStyle().ScaleAllSizes(0.5f);
	BravoScreen::Render_Internal(DeltaTime);
	ImGui::Begin("Simulation", nullptr,
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_AlwaysVerticalScrollbar );

		Spacing();


		if ( Slider("Particle count", &Simulation->ParticleCount, 1, 100000) )
		{
			if ( !Simulation->HasStarted() )
			{
				Simulation->Reset();
			}
		}
		
		bool UpdateSettings = false;

		Spacing();
		UpdateSettings |= Slider("Mass", &Simulation->ParticleMass, 1.0f, 10.0f);
		
		Spacing();
		if ( Slider("Size", &Simulation->ParticleRadius, 1.0f, 100.0) )
		{
			if ( !Simulation->HasStarted() )
			{
				Simulation->Reset();
			}
			UpdateSettings |= true;
		}

		Spacing();
		Spacing();
		UpdateSettings |= Slider("Target density", &Simulation->TargetDensity, 0.0f, 500.0f);

		Spacing();
		UpdateSettings |= Slider("Preassure", &Simulation->PressureFactor, 0.0f, 500.0f);

		Spacing();
		UpdateSettings |= Slider("Viscosity", &Simulation->ViscosityFactor, 0.001f, 5.0f);

		Spacing();
		Spacing();
		UpdateSettings |= Slider("Collision Damping", &Simulation->CollisionDamping, 0.0f, 1.0f);

		Spacing();
		UpdateSettings |= Slider("Gravity", &Simulation->Gravity, -100.0f, 100.0f);

		Spacing();
		UpdateSettings |= Slider("Max Velocity", &Simulation->MaxVelocity, 0.0f, 40.0f);

		if ( UpdateSettings )
		{
			if ( !Simulation->HasStarted() )
			{
				Simulation->Reset();
			}
			else
			{
				Simulation->UpdateShaderUniformParams();
			}
		}

	ImGui::End();
	ImGui::GetStyle().ScaleAllSizes(2.0f);
}

void FluidScreen_SimulationSettings::OnToggleHUD(bool ButtonState, float DeltaTime)
{
	bShowHUD = !bShowHUD;
}