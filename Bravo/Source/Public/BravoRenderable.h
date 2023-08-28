#pragma once

class BravoRenderable
{
	friend class BravoEngine;

public:
	BravoRenderable() = default;
	BravoRenderable(int32 _RenderPriority) : RenderPriority(_RenderPriority) {}

	void SetRenderPriority(int32 _RenderPriority) { RenderPriority = _RenderPriority; }
	int32 GetRenderPriority() const { return RenderPriority; }

	void SetCastShadows(bool _bCastShadows) { bCastShadows = _bCastShadows; }
	bool GetCastShadows() const { return bCastShadows; }

protected:
	virtual void Render(const glm::vec3& CameraLocation, const glm::mat4& CameraProjection, const glm::mat4& CameraView) = 0;
	virtual void RenderDepthMap(std::shared_ptr<class BravoShaderAsset> Shader) {}
	
private:
	int32 RenderPriority = 0;
	bool bCastShadows = false;
};
