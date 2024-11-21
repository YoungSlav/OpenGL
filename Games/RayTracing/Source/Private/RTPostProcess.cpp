#include "RTPostProcess.h"
#include "BravoEngine.h"
#include "BravoAssetManager.h"
#include "BravoShaderAsset.h"
#include "BravoRenderTarget.h"
#include "BravoViewport.h"
#include "RTSceneComponent.h"
#include "BravoTextureUnitManager.h"
#include "BravoCamera.h"

bool RTPostProcess::Initialize_Internal()
{
	if ( !BravoPostProcess::Initialize_Internal() )
		return false;

	// TODO: replace by shader that would average output from all past frames
	Shader = Engine->GetAssetManager()->FindOrLoad<BravoShaderAsset>("AverageFrameShader", BravoShaderLoadingParams("Shaders\\AverageFrame"));
	if ( !Shader )
		return false;

	RayTracingCompute = Engine->GetAssetManager()->FindOrLoad<BravoShaderAsset>("RayTracing", BravoShaderLoadingParams("Shaders\\RayTracing"));
	if ( !RayTracingCompute )
		return false;

	glm::ivec2 Size = Engine->GetViewport()->GetViewportSize();
	RenderTarget = NewObject<BravoRenderTarget>("RayTracingRenderTarget", Size, GL_RGBA32F, GL_RGBA, GL_FLOAT, false);

	Engine->GetViewport()->OnResizeDelegate.AddSP(Self<RTPostProcess>(), &RTPostProcess::OnViewportResize);

	glGenBuffers(1, &SceneComponentsSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, SceneComponentsSSBO);
		glBufferData(GL_SHADER_STORAGE_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	
	return true;
}

void RTPostProcess::OnViewportResize(const glm::ivec2& NewSize)
{
	RenderTarget->Resize(NewSize);
}

void RTPostProcess::RegisterSkybox(std::shared_ptr<class BravoCubemapAsset> _CubeMap)
{
	Skybox = _CubeMap;
}

void RTPostProcess::RegisterSceneComponent(std::shared_ptr<class RTSceneComponent> _NewComponent)
{
	_NewComponent->OnObjectDestroyDelegate.AddSP(Self<RTPostProcess>(), &RTPostProcess::OnComponentDestroy);
	_NewComponent->OnComponentUpdatedDelegate.AddSP(Self<RTPostProcess>(), &RTPostProcess::OnComponentUpdate);

	RegisteredComponents.insert({_NewComponent->GetHandle(), _NewComponent});
	
	bDirtyData = true;
}

void RTPostProcess::OnComponentDestroy(std::shared_ptr<BravoObject> _Object)
{
	RegisteredComponents.erase(_Object->GetHandle());
	
	bDirtyData = true;
}

void RTPostProcess::OnComponentUpdate(const RTSceneComponent* _Component)
{
	bDirtyData = true;
}

void RTPostProcess::UpdateSceneData()
{
	if ( !bDirtyData )
		return;

	std::vector<RTSceneComponentData> SceneData;
	SceneData.reserve(RegisteredComponents.size());
	for ( auto it : RegisteredComponents )
	{
		RTSceneComponentData ComponentData;
		ComponentData.Type = it.second->GetComponentType();

		ComponentData.Model = it.second->GetTransform_World().GetTransformMatrix();
		ComponentData.ModelInverse = glm::inverse(ComponentData.Model);

		ComponentData.Material = it.second->GetMaterial();

		SceneData.push_back(ComponentData);
	}
	uint32 size = SceneData.size() * sizeof(RTSceneComponentData);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, SceneComponentsSSBO);
		glBufferData(GL_SHADER_STORAGE_BUFFER, SceneData.size() * sizeof(RTSceneComponentData), SceneData.data(), GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, SceneComponentsSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	RayTracingCompute->Use();
		RayTracingCompute->SetInt("componentsCount", SceneData.size());
	RayTracingCompute->StopUsage();

	bDirtyData = false;
}

bool RTPostProcess::EnsureReady()
{
	if ( Skybox == nullptr || !Skybox->EnsureGPUReady() )
		return false;

	if ( !Shader->EnsureGPUReady() || !RayTracingCompute->EnsureGPUReady() )
		return false;

	UpdateSceneData();
	return true;
}

void RTPostProcess::Render_Internal()
{
	static int32 frameNumber = 0;
	frameNumber++;


	glm::ivec2 OutpitImageSize = RenderTarget->GetSize();

	const std::shared_ptr<BravoCamera> camera = Engine->GetCamera();
	if ( !camera )
		return;

	static glm::quat camDir = camera->GetRotation_World();
	static glm::vec3 camLoc = camera->GetLocation_World();
	glm::quat newCamDir = camera->GetRotation_World();
	glm::vec3 newCamLoc = camera->GetLocation_World();
	if ( camDir != newCamDir || camLoc != newCamLoc )
	{
		camLoc = newCamLoc;
		camDir = newCamDir;
		frameNumber = 0;
	}

	const glm::mat4 view = camera->GetViewMatrix();
	const glm::mat4 proj = camera->GetProjectionMatrix();
	//const glm::mat4 ViewProj = proj * view;
	

	int32 OutputTextureUnit = BravoTextureUnitManager::BindTexture();
	glActiveTexture(GL_TEXTURE0 + OutputTextureUnit);
	glBindTexture(GL_TEXTURE_2D, RenderTarget->GetColorTexture());

	RayTracingCompute->Use();
		glBindImageTexture(OutputTextureUnit, RenderTarget->GetColorTexture(), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

		RayTracingCompute->SetInt("outputTexture", OutputTextureUnit);

		RayTracingCompute->SetFloat3("eyeLocation", camera->GetLocation_World());

		RayTracingCompute->SetInt("maxBounceCount", 10);
		RayTracingCompute->SetInt("raysPerPixel", 10);
		RayTracingCompute->SetInt("frameNumber", frameNumber);

		RayTracingCompute->SetCubemap("skybox", Skybox);
		

		RayTracingCompute->SetMatrix4d("invViewMat", glm::inverse(view));
		RayTracingCompute->SetMatrix4d("invProjMat", glm::inverse(proj));
		RayTracingCompute->SetFloat1("antiAliasingStrength", 0.1f);

		GLuint workGroupSizeX = (OutpitImageSize.x + 15) / 16;  // Round up to the nearest multiple of 16
		GLuint workGroupSizeY = (OutpitImageSize.y + 15) / 16;
		glDispatchCompute(workGroupSizeX, workGroupSizeY, 1);

		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	RayTracingCompute->StopUsage();



	Shader->Use();
		Shader->SetInt("screenTexture", OutputTextureUnit);
		Draw();
	Shader->StopUsage();

	BravoTextureUnitManager::UnbindTexture(OutputTextureUnit);
}