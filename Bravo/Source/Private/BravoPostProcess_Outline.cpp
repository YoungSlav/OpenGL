#include "BravoPostProcess_Outline.h"
#include "BravoEngine.h"
#include "BravoViewport.h"
#include "BravoRenderTarget.h"
#include "IBravoRenderable.h"
#include "BravoShaderAsset.h"
#include "BravoAssetManager.h"
#include "BravoTextureUnitManager.h"
#include "BravoCamera.h"


bool BravoPostProcess_Outline::Initialize_Internal()
{
	if ( !BravoPostProcess::Initialize_Internal() )
		return false;

	glm::ivec2 Size = Engine->GetViewport()->GetViewportSize();
	OutlineRenderTarget = NewObject<BravoRenderTarget>("OutlineRenderTarget", Size, GL_R32F, GL_RED, GL_FLOAT, true);

	Engine->GetViewport()->OnResizeDelegate.AddSP(Self<BravoPostProcess_Outline>(), &BravoPostProcess_Outline::OnViewportResized);

	Shader = Engine->GetAssetManager()->FindOrLoad<BravoShaderAsset>("OutlinePostProccess", BravoShaderLoadingParams("Shaders\\OutlinePostProccess"));
	if ( !Shader )
		return false;

	return true;
}


void BravoPostProcess_Outline::OnViewportResized(const glm::ivec2& Size)
{
	OutlineRenderTarget->Resize(Size);
}

bool BravoPostProcess_Outline::EnsureReady()
{
	if ( !BravoPostProcess::EnsureReady() )
		return false;

	auto SelectionManager = Engine->GetSelectionManager();
	if ( !SelectionManager )
		return false;

	auto ActiveSelections = SelectionManager->GetSelections();

	if ( !ActiveSelections.size() )
		return false;
	
	const std::shared_ptr<BravoCamera> camera = Engine->GetCamera();
	if ( !camera )
		return false;

	return true;
}

void BravoPostProcess_Outline::Render_Internal()
{
	auto SelectionManager = Engine->GetSelectionManager();

	auto ActiveSelections = SelectionManager->GetSelections();
		
	const std::shared_ptr<BravoCamera> camera = Engine->GetCamera();

	OutlineRenderTarget->Bind();
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	for ( auto it : ActiveSelections )
	{
		it.first->RenderOutlineMask();
	}
	OutlineRenderTarget->Unbind();

	Shader->Use();

		int32 OutlineColorMask = BravoTextureUnitManager::BindTexture();
		glActiveTexture(GL_TEXTURE0 + OutlineColorMask);
		glBindTexture(GL_TEXTURE_2D, OutlineRenderTarget->GetColorTexture());
		Shader->SetInt("OutlineColorMask", OutlineColorMask);
		Shader->SetFloat3("OutlineColor", OutlineColor);
				
		Draw();

		BravoTextureUnitManager::UnbindTexture(OutlineColorMask);

	Shader->StopUsage();

}