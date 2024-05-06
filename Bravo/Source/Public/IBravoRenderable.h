#pragma once

enum ERenderPriority : int32
{
	Early = 0,
	Starndart = 1,
	Late = 2
};

enum ERenderGroup : int32
{
	 Main = 0,
	 Overlay = 1
};

class IBravoRenderable
{
public:
	IBravoRenderable() = default;
	IBravoRenderable(ERenderPriority _RenderPriority)  : RenderPriority(_RenderPriority), RenderGroup(ERenderGroup::Main) {}
	IBravoRenderable(ERenderPriority _RenderPriority, ERenderGroup _RenderGroup) : RenderPriority(_RenderPriority), RenderGroup(_RenderGroup) {}

	ERenderPriority GetRenderPriority() const { return RenderPriority; }
	ERenderGroup GetRenderGroup() const { return RenderGroup; }

	void SetCastShadows(bool _bCastShadows) { bCastShadows = _bCastShadows; }
	bool GetCastShadows() const { return bCastShadows; }

	virtual void Render() = 0;
	virtual void RenderSelectionID() {};
	virtual void RenderOutlineMask(int32 InstanceID) {};
	virtual void RenderDepthMap(std::shared_ptr<class BravoShaderAsset> Shader) {}
	
private:
	const ERenderPriority RenderPriority = Starndart;
	const ERenderGroup RenderGroup = Main;

	bool bCastShadows = false;
	
};
