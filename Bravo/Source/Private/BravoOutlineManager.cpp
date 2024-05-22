#include "BravoOutlineManager.h"
#include "BravoEngine.h"
#include "BravoViewport.h"
#include "BravoRenderTarget.h"
#include "IBravoRenderable.h"
#include "BravoShaderAsset.h"
#include "BravoAssetManager.h"
#include "BravoTextureUnitManager.h"
#include "BravoCamera.h"


bool BravoOutlineManager::Initialize_Internal()
{
	if ( !BravoObject::Initialize_Internal() )
		return false;

	glm::ivec2 Size = Engine->GetViewport()->GetViewportSize();
	OutlineRenderTarget = NewObject<BravoRenderTarget>("OutlineRenderTarget");
	OutlineRenderTarget->Setup(Size, GL_R32F, GL_RED, GL_FLOAT, true);

	Engine->GetViewport()->OnResizeDelegate.AddSP(Self<BravoOutlineManager>(), &BravoOutlineManager::OnViewportResized);

	if (auto AssetManager = Engine->GetAssetManager())
	{
		OutlinePostProccessShader = AssetManager->FindOrLoad<BravoShaderAsset>("OutlinePostProccess", BravoShaderLoadingParams("Shaders\\OutlinePostProccess"));
	}

	// vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
	static float planeVertices[] = {
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };
	glGenVertexArrays(1, &PlaneVAO);
    glGenBuffers(1, &PlaneVBO);
    glBindVertexArray(PlaneVAO);
    glBindBuffer(GL_ARRAY_BUFFER, PlaneVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	return true;
}


void BravoOutlineManager::OnDestroy()
{
	glDeleteBuffers(1, &PlaneVBO);
	glDeleteVertexArrays(1, &PlaneVAO);
	OutlineRenderTarget->Destroy();
	BravoObject::OnDestroy();
}

void BravoOutlineManager::OnViewportResized(const glm::ivec2& Size)
{
	OutlineRenderTarget->Resize(Size);
}

void BravoOutlineManager::RenderSelections()
{
	auto SelectionManager = Engine->GetSelectionManager();
	if ( !SelectionManager )
		return;

	auto ActiveSelections = SelectionManager->GetSelections();

	if ( !ActiveSelections.size() )
		return;

	if ( !OutlinePostProccessShader || !OutlinePostProccessShader->EnsureReady() )
		return;

	const std::shared_ptr<BravoCamera> camera = Engine->GetCamera();
	if ( !camera )
		return;

	OutlineRenderTarget->Bind();
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	for ( auto it : ActiveSelections )
	{
		it.first->RenderOutlineMask();
	}
	OutlineRenderTarget->Unbind();



	OutlinePostProccessShader->Use();

		int32 OutlineColorMask = BravoTextureUnitManager::BindTexture();
		glActiveTexture(GL_TEXTURE0 + OutlineColorMask);
		glBindTexture(GL_TEXTURE_2D, OutlineRenderTarget->GetColorTexture());
		OutlinePostProccessShader->SetInt("OutlineColorMask", OutlineColorMask);
		OutlinePostProccessShader->SetVector3d("OutlineColor", OutlineColor);
				
		glBindVertexArray(PlaneVAO);
			glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);

		BravoTextureUnitManager::UnbindTexture(OutlineColorMask);

	OutlinePostProccessShader->StopUsage();
}