#pragma once

enum ERenderPriority : int32
{
	Prepass = 0,
	Starndart = 1,
	Overlay = 2
};

class IBravoRenderable
{
public:
	IBravoRenderable() = default;
	IBravoRenderable(ERenderPriority _RenderPriority) : RenderPriority(_RenderPriority) {}

	void SetRenderPriority(ERenderPriority _RenderPriority) { RenderPriority = _RenderPriority; }
	ERenderPriority GetRenderPriority() const { return RenderPriority; }

	void SetCastShadows(bool _bCastShadows) { bCastShadows = _bCastShadows; }
	bool GetCastShadows() const { return bCastShadows; }

	virtual void Render() = 0;
	virtual void RenderSelectionID() {};
	virtual void RenderOutlineMask(int32 InstanceID) {};
	virtual void RenderDepthMap(std::shared_ptr<class BravoShaderAsset> Shader) {}
	
private:
	ERenderPriority RenderPriority = Starndart;
	bool bCastShadows = false;
	
};
