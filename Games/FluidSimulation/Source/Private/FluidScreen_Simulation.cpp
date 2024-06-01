#include "FluidScreen_Simulation.h"
#include "BravoInput.h"
#include "BravoEngine.h"
#include "FluidSimulation.h"
#include "FluidContainer.h"


bool FluidScreen_Simulation::Initialize_Internal()
{
	if ( !BravoScreen::Initialize_Internal() )
		return false;

	SetTrueScaling(false);
	SetSize(glm::vec2(0.3f, 0.5f));
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
	
	return true;
}

void FluidScreen_Simulation::Render_Internal(float DeltaTime)
{
	if ( !bShowHUD )
		return;

	assert(Simulation != nullptr);

	glm::vec2 trueSize = GetTrueSize();
	const float sliderWidth = trueSize.x * 0.4f;
	const float btnWidth = trueSize.x;

	ImGui::GetStyle().ScaleAllSizes(0.5f);
	BravoScreen::Render_Internal(DeltaTime);
	ImGui::Begin("Simulation", nullptr,
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoCollapse );

		ImGui::SetNextItemWidth(sliderWidth);
		if ( ImGui::SliderInt("Particle count", &Simulation->ParticleCount, 0, 100) )
		{
			Simulation->SpawnParticles(Simulation->ParticleCount, Simulation->bRandomPositions);
		}
		ImGui::Spacing();
		if ( ImGui::Checkbox("Random Spawn", &Simulation->bRandomPositions) )
		{
			Simulation->SpawnParticles(Simulation->ParticleCount, Simulation->bRandomPositions);
		}

		ImGui::SetNextItemWidth(sliderWidth);
		ImGui::SliderFloat("Mass", &Simulation->ParticleMass, 0.0f, 10.0f);
		ImGui::Spacing();

		ImGui::SetNextItemWidth(sliderWidth);
		ImGui::SliderFloat("Size", &Simulation->ParticleSize, 0.0f, 20.0f);
		ImGui::Spacing();

		ImGui::SetNextItemWidth(sliderWidth);
		ImGui::SliderFloat("Collision Damping", &Simulation->CollisionDamping, 0.0f, 1.0f);
		ImGui::Spacing();

		ImGui::SetNextItemWidth(sliderWidth);
		ImGui::SliderFloat("Gravity", &Simulation->Gravity, -100.0f, 100.0f);
		ImGui::Spacing();

		ImGui::SetNextItemWidth(sliderWidth);
		ImGui::SliderFloat("Max Velocity", &Simulation->MaxVelocity, 0.0f, 100.0f);
		ImGui::Spacing();

		
		ImGui::SetNextItemWidth(btnWidth);
		std::string pauseText = Simulation->IsPaused() ? "Play" : "Pause";
		if (ImGui::Button(pauseText.c_str(), ImVec2(btnWidth, 0)))
		{
			Simulation->TogglePause();
		}
		ImGui::Spacing();


		ImGui::SetNextItemWidth(btnWidth);
		if (ImGui::Button("Restart", ImVec2(btnWidth, 0)))
		{
			Simulation->Restart();
		}

	ImGui::End();
	ImGui::GetStyle().ScaleAllSizes(2.0f);
}

void FluidScreen_Simulation::OnToggleHUD(bool ButtonState, float DeltaTime)
{
	bShowHUD = !bShowHUD;
}