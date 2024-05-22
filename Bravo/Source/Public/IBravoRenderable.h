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

typedef MulticastDelegate<const int32&> OnObjectClickedSignature;

class IBravoRenderable
{
public:
	IBravoRenderable() = default;
	IBravoRenderable(ERenderPriority _RenderPriority)  : RenderPriority(_RenderPriority), RenderGroup(ERenderGroup::Main) {}
	IBravoRenderable(ERenderPriority _RenderPriority, ERenderGroup _RenderGroup) : RenderPriority(_RenderPriority), RenderGroup(_RenderGroup) {}

	ERenderPriority GetRenderPriority() const { return RenderPriority; }
	ERenderGroup GetRenderGroup() const { return RenderGroup; }

	void SetVisisble(bool bNewVisible) { bVisible = bNewVisible; }
	bool IsVisisble() const { return bVisible; }

	void SetCastShadows(bool _bCastShadows) { bCastShadows = _bCastShadows; }
	bool GetCastShadows() const { return bCastShadows; }


	virtual void Render() = 0;
	virtual void RenderDepthMap(std::shared_ptr<class BravoShaderAsset> Shader) {}
	
	// selection
	virtual void ClearSelection() {};
	virtual void UpdateSelection(const std::vector<int32>& SelectedInstances) {};
	virtual void RenderSelectionID() {};
	virtual void RenderOutlineMask() {};

	OnObjectClickedSignature OnObjectClicked;

	void ObjectClicked(int32 InstanceIndex = 0)
	{
		OnObjectClicked.Broadcast(InstanceIndex);
	};


	
private:

	const ERenderPriority RenderPriority = Starndart;
	const ERenderGroup RenderGroup = Main;

	bool bCastShadows = false;
	bool bVisible = true;
	
};
