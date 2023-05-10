#include "BravoHUD.h"
#include "BravoRenderTarget.h"
#include "BravoAssetManager.h"
#include "BravoShader.h"
#include "BravoWidget.h"
#include "BravoFont.h"
#include "BravoTexture.h"

#include "openGL.h"
#include <iomanip>
#include <sstream>

#include "BravoWidget_Text.h"
#include "BravoWidget_Plane.h"

void BravoHUD::Initialize_Internal()
{
	Size = TargetSize;
	ModelMatrix = glm::ortho(0.0f, Size.x,  Size.y, 0.0f);

	auto font = GetEngine()->GetAssetManager()->LoadAsset<BravoFont>("Fonts\\arial.ttf", {"50", "25", "12" });
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

	std::shared_ptr<BravoWidget_Plane> plane = std::shared_ptr<BravoWidget_Plane>(new BravoWidget_Plane(Self<BravoHUD>(), GetEngine()->GetAssetManager()));
	plane->Initialize();
	plane->SetPosition(glm::vec2(0.5f, 0.5f));
	plane->SetSize(glm::vec2(0.1f, 0.1f));
	plane->SetOrigin(glm::vec2(1.0f, 1.0f));
	//plane->SetColor(glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
	plane->SetTexture(GetEngine()->GetAssetManager()->LoadAsset<BravoTexture>("Textures\\noise.png"));
	Widgets.push_back(plane);
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

