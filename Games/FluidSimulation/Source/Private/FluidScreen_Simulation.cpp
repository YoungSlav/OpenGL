#include "FluidScreen_Simulation.h"
#include "BravoInput.h"
#include "BravoEngine.h"
#include "FluidSimulation.h"


bool FluidScreen_Simulation::Initialize_Internal()
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
		sub.Callback.BindSP(Self<FluidScreen_Simulation>(), &FluidScreen_Simulation::OnToggleHUD);
		Engine->GetInput()->SubscribeKey(sub);
	}


	glm::vec2 trueSize = GetTrueSize();
	SliderWidth = trueSize.x;
	BtnWidth = trueSize.x;
	
	return true;
}

void FluidScreen_Simulation::Spacing()
{
	ImGui::Dummy(ImVec2(0.0f, 10.0f));
}

bool FluidScreen_Simulation::Slider(const std::string& Label, int32* value, int32 min, int32 max)
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

bool FluidScreen_Simulation::Slider(const std::string& Label, float* value, float min, float max)
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

void FluidScreen_Simulation::Display(const std::string& Label, const std::string& value)
{
	ImGui::SetNextItemWidth(SliderWidth);
	ImGui::TextDisabled(Label.c_str());
	ImGui::SameLine();
	ImGui::TextDisabled(value.c_str());
}

void FluidScreen_Simulation::Render_Internal(float DeltaTime)
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


		bool UpdateSettings = false;
		
		if ( Slider("Size", &Simulation->ParticleRadius, 0.01f, 1.0) )
		{
			Simulation->Reset();
		}

		Spacing();
		UpdateSettings |= Slider("Target density", &Simulation->TargetDensity, 0.1f, 2000.0f);

		Spacing();
		UpdateSettings |= Slider("Preassure", &Simulation->Preassure, 0.1f, 500.0f);

		Spacing();
		UpdateSettings |= Slider("Viscosity", &Simulation->ViscosityFactor, 0.0f, 0.2f);

		Spacing();
		UpdateSettings |= Slider("Collision Damping", &Simulation->CollisionDamping, 0.0f, 1.0f);

		Spacing();
		UpdateSettings |= Slider("Gravity", &Simulation->Gravity, -100.0f, 100.0f);

		Spacing();
		UpdateSettings |= Slider("Interaction force", &Simulation->InteractionAcceleration, 0.0f, 100.0f);

		Spacing();
		UpdateSettings |= Slider("Interaction radius", &Simulation->InteractionRadius, 1.0f, 10.0f);

		Spacing();
		UpdateSettings |= Slider("Max velocity display", &Simulation->MaxVelocity, 0.0f, 50.0f);

		Spacing();
		UpdateSettings |= Slider("Steps per tick", &Simulation->StepsPerTick, 1, 4);

		if ( UpdateSettings )
			Simulation->UpdateShaderUniformParams();

		Display("Particle count: ", Log::to_string(Simulation->GetParticleCount()));
		Display("Smoothing radius: ", Log::to_string(Simulation->GetSmoothingRadius()));
		Display("Near pressure: ", Log::to_string(Simulation->GetNearPreassure()));
		Display("Particle mass: ", Log::to_string(Simulation->GetParticleMass()));
		Display("World size: ", Log::to_string(Simulation->GetWorldSize()));

	ImGui::End();
	ImGui::GetStyle().ScaleAllSizes(2.0f);
}

void FluidScreen_Simulation::OnToggleHUD(bool ButtonState, float DeltaTime)
{
	bShowHUD = !bShowHUD;
}