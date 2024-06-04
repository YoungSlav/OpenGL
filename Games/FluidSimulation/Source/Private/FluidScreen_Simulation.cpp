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
	SetPosition(glm::vec2(0.0f, 0.0f));

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
		ImGuiWindowFlags_NoCollapse );

		Spacing();
		std::string pauseText = Simulation->IsPaused() ? "Play" : "Pause";
		if (ImGui::Button(pauseText.c_str(), ImVec2(BtnWidth, 0)))
		{
			Simulation->TogglePause();
		}

		Spacing();
		if (ImGui::Button("Reset", ImVec2(BtnWidth, 0)))
		{
			Simulation->Reset();
		}

		Spacing();
		Spacing();

		if ( Slider("Particle count", &Simulation->ParticlesCount, 1, 4000) )
		{
			if ( !Simulation->HasStarted() )
			{
				Simulation->SpawnParticles();
			}
		}
		
		if ( ImGui::Checkbox("Random", &Simulation->bRandomPositions) )
		{
			if ( !Simulation->HasStarted() )
			{
				Simulation->SpawnParticles();
			}
		}

		Spacing();
		Slider("Mass", &Simulation->ParticleMass, 0.001f, 10.0f);
		
		Spacing();
		if ( Slider("Size", &Simulation->ParticleSize, 10.0f, 100.0) )
		{
			if ( !Simulation->HasStarted() )
			{
				Simulation->SpawnParticles();
			}
		}

		Spacing();
		if ( Slider("Smoothing radius", &Simulation->SmoothingRadius, 10.0f, 100.0f) )
		{
			if ( !Simulation->HasStarted() )
			{
				Simulation->SpawnParticles();
			}
			Simulation->UpdateMath();
		}

		Spacing();
		Slider("Target density", &Simulation->TargetDensity, 0.0f, 1.0f);

		Spacing();
		Slider("Preassure", &Simulation->Preassure, 0.0f, 1000.0f);

		Spacing();
		Slider("Near Pressure", &Simulation->NearPressureMultiplier, 0.0f, 1000.0f);

		Spacing();
		Slider("Collision Damping", &Simulation->CollisionDamping, 0.0f, 1.0f);

		Spacing();
		Slider("Gravity", &Simulation->Gravity, 0.0f, 100.0f);

		Spacing();
		Slider("Max Velocity", &Simulation->MaxVelocity, 0.0f, 100.0f);

		Spacing();
		Slider("Steps per tick", &Simulation->StepsPerTick, 1, 4);

	ImGui::End();
	ImGui::GetStyle().ScaleAllSizes(2.0f);
}

void FluidScreen_Simulation::OnToggleHUD(bool ButtonState, float DeltaTime)
{
	bShowHUD = !bShowHUD;
}