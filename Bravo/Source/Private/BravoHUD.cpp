#include "BravoHUD.h"

#include "openGL.h"
#include <iomanip>
#include <sstream>

#include <algorithm>
#include "BravoScreen.h"

bool BravoHUD::Initialize_Internal()
{
	Size = TargetSize;
	ModelMatrix = glm::ortho(0.0f, Size.x,  Size.y, 0.0f);

	//auto font = GetEngine()->GetAssetManager()->LoadAsset<BravoFont>("Fonts\\arial.ttf", {"50", "25", "12" });
	//std::shared_ptr<BravoWidget_Text> _fps = std::shared_ptr<BravoWidget_Text>(new BravoWidget_Text(Self<BravoHUD>(), GetEngine()->GetAssetManager()));
	//_fps->Initialize();
	//_fps->SetPosition(glm::vec2(1.0f, 0.0f));
	//_fps->SetOrigin(glm::vec2(1.0f, 0.0f));
	//_fps->SetMargin(glm::vec2(2.0f, 2.0f));
	//_fps->SetTextSize(20);
	//_fps->SetText("FPS: ");
	//_fps->SetFont(font);
	//fps = _fps;
	//Widgets.push_back(_fps);

	return true;
}

void BravoHUD::AddScreen(std::shared_ptr<BravoScreen> _Screen)
{
	Screens.push_back(_Screen);
	sort(Screens.begin(), Screens.end(), 
			[](std::shared_ptr<BravoScreen> left, std::shared_ptr<BravoScreen> right) -> bool
			{ 
				return left->GetRenderPriority() < right->GetRenderPriority(); 
			});
}

void BravoHUD::RemoveScreen(std::shared_ptr<class BravoScreen> _Screen)
{
	Screens.erase(std::remove(Screens.begin(), Screens.end(), _Screen), Screens.end());
}


void BravoHUD::SetSize(const glm::vec2& _Size)
{
	Size = _Size;
	ModelMatrix = glm::ortho(0.0f, Size.x, Size.y, 0.0f);
}

void BravoHUD::Render()
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	for ( auto screen : Screens )
	{
		screen->Render();
	}
}
/*
void BravoHUD::Tick(float DeltaTime)
{
	std::shared_ptr<BravoWidget_Text> _fps = fps.expired() ? nullptr : fps.lock();

	if ( !_fps )
		return;

	FPSUpdateTimer += DeltaTime;
	if ( FPSUpdateTimer < FPSUpdateRate )
		return;
	FPSUpdateTimer = 0.0f;

	std::stringstream stream;
	float FPS = 1.0f / DeltaTime;
	stream << std::fixed << std::setprecision(0) << FPS;
	_fps->SetText("FPS: " + stream.str());

	if ( FPS >= 59 )
		_fps->SetTextColor(glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
	else if ( FPS >= 29 )
		_fps->SetTextColor(glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));
	else
		_fps->SetTextColor(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
}
*/