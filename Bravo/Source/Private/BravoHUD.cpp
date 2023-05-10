#include "BravoHUD.h"
#include "BravoRenderTarget.h"
#include "BravoAssetManager.h"
#include "BravoShader.h"
#include "BravoWidget.h"
#include "BravoWidget_Text.h"
#include "BravoFont.h"


#include <iomanip>
#include <sstream>

void BravoHUD::Initialize_Internal()
{
	Size = TargetSize;
	ModelMatrix = glm::ortho(0.0f, Size.x,  Size.y, 0.0f);

	auto font = GetEngine()->GetAssetManager()->LoadAsset<BravoFont>("Fonts\\arial.ttf", {"100", "50", "25"});
	std::shared_ptr<BravoWidget_Text> _fps = std::shared_ptr<BravoWidget_Text>(new BravoWidget_Text(Self<BravoHUD>(), GetEngine()->GetAssetManager()));
	_fps->Initialize();
	_fps->SetPosition(glm::vec2(1.0f, 0.0f));
	_fps->SetOrigin(glm::vec2(1.0f, 0.0f));
	_fps->SetMargin(glm::vec2(2.0f, 2.0f));
	_fps->SetTextSize(20);
	_fps->SetText("FPS: ");
	_fps->SetFont(font);
	fps = _fps;
	Widgets.push_back(_fps);
}

void BravoHUD::SetSize(const glm::vec2& _Size)
{
	Size = _Size;
	ModelMatrix = glm::ortho(0.0f, Size.x, Size.y, 0.0f);
}

void BravoHUD::Render()
{
	for ( auto w : Widgets )
		w->Render();
}

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

