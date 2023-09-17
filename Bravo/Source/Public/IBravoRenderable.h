#pragma once

class IBravoRenderable
{
public:
	IBravoRenderable() = default;
	IBravoRenderable(int32 _RenderPriority) : RenderPriority(_RenderPriority) {}

	void SetRenderPriority(int32 _RenderPriority) { RenderPriority = _RenderPriority; }
	int32 GetRenderPriority() const { return RenderPriority; }

	void SetCastShadows(bool _bCastShadows) { bCastShadows = _bCastShadows; }
	bool GetCastShadows() const { return bCastShadows; }

	virtual void Render() = 0;
	virtual void RenderSelectionID() {};
	virtual void RenderOutlineMask(const glm::vec3& Color, int32 InstanceID) {};
	virtual void RenderDepthMap(std::shared_ptr<class BravoShaderAsset> Shader) {}
	
private:
	int32 RenderPriority = 0;
	bool bCastShadows = false;
	
};
