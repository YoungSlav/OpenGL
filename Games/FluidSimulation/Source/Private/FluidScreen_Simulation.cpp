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
		
		bool UpdateSettings = false;

		Spacing();
		UpdateSettings |= Slider("Mass", &Simulation->ParticleMass, 50.0f, 200.0f);
		
		Spacing();
		if ( Slider("Size", &Simulation->ParticleRadius, 1.0f, 100.0) )
		{
			if ( !Simulation->HasStarted() )
			{
				Simulation->SpawnParticles();
			}
			UpdateSettings |= true;
		}

		Spacing();
		if ( Slider("Smoothing radius", &Simulation->SmoothingRadius, 1.0f, 100.0f) )
		{
			if ( !Simulation->HasStarted() )
			{
				Simulation->SpawnParticles();
			}
			UpdateSettings |= true;
		}

		Spacing();
		Spacing();
		UpdateSettings |= Slider("Target density", &Simulation->TargetDensity, 0.0f, 2.0f);

		Spacing();
		UpdateSettings |= Slider("Preassure", &Simulation->Preassure, 0.0f, 5000.0f);

		Spacing();
		UpdateSettings |= Slider("Viscosity", &Simulation->ViscosityFactor, 1.0f, 100000.0f);


		Spacing();
		Spacing();
		UpdateSettings |= Slider("Collision Damping", &Simulation->CollisionDamping, 0.0f, 1.0f);

		Spacing();
		UpdateSettings |= Slider("Gravity", &Simulation->Gravity, -100.0f, 100.0f);

		Spacing();
		UpdateSettings |= Slider("Interaction force", &Simulation->InteractionAcceleration, -100000.0f, 100000.0f);

		Spacing();
		UpdateSettings |= Slider("Interaction radius", &Simulation->InteractionRadius, 10.0f, 1000.0f);

		Spacing();
		UpdateSettings |= Slider("Max Velocity", &Simulation->MaxVelocity, 0.0f, 100.0f);

		if ( UpdateSettings )
			Simulation->UpdateShaderUniformParams();

	ImGui::End();
	ImGui::GetStyle().ScaleAllSizes(2.0f);
}

void FluidScreen_Simulation::OnToggleHUD(bool ButtonState, float DeltaTime)
{
	bShowHUD = !bShowHUD;
}