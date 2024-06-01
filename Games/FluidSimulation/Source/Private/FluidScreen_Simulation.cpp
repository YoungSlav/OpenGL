#include "FluidScreen_Simulation.h"
#include "BravoInput.h"
#include "BravoEngine.h"
#include "FluidSimulation.h"


bool FluidScreen_Simulation::Initialize_Internal()
{
	if ( !BravoScreen::Initialize_Internal() )
		return false;

	SetTrueScaling(false);
	SetSize(glm::vec2(0.3f, 0.7f));
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

	ImGui::SetNextItemWidth(SliderWidth);
	std::string inputName = "##" + Label;
	bool ret = ImGui::SliderInt(inputName.c_str(), value, min, max);

	return ret;
}

bool FluidScreen_Simulation::Slider(const std::string& Label, float* value, float min, float max)
{
	ImGui::SetNextItemWidth(SliderWidth);
	ImGui::Text(Label.c_str());
	
	ImGui::SetNextItemWidth(SliderWidth);
	std::string inputName = "##" + Label;
	bool ret = ImGui::SliderFloat(inputName.c_str(), value, min, max);

	return ret;
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

		if ( Slider("Particle count", &Simulation->ParticleCount, 0, 2000) )
		{
			Simulation->SpawnParticles(Simulation->ParticleCount, Simulation->bRandomPositions);
		}
		
		if ( ImGui::Checkbox("Random", &Simulation->bRandomPositions) )
		{
			Simulation->SpawnParticles(Simulation->ParticleCount, Simulation->bRandomPositions);
		}

		Spacing();
		Slider("Mass", &Simulation->ParticleMass, 0.0f, 10.0f);
		
		Spacing();
		if ( Slider("Size", &Simulation->ParticleSize, 0.0f, Simulation->CalcMaxParticleSize()) )
		{
			if ( !Simulation->HasStarted() )
			{
				Simulation->SpawnParticles(Simulation->ParticleCount, Simulation->bRandomPositions);
			}
		}

		Spacing();
		Slider("Collision Damping", &Simulation->CollisionDamping, 0.0f, 1.0f);

		Spacing();
		Slider("Gravity", &Simulation->Gravity, -100.0f, 100.0f);

		Spacing();
		Slider("Max Velocity", &Simulation->MaxVelocity, 0.0f, 100.0f);

		
		Spacing();
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

	ImGui::End();
	ImGui::GetStyle().ScaleAllSizes(2.0f);
}

void FluidScreen_Simulation::OnToggleHUD(bool ButtonState, float DeltaTime)
{
	bShowHUD = !bShowHUD;
}