#pragma once
#include "stdafx.h"

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
	RTTR_ENABLE();
	

public:
	IBravoRenderable() = default;

	IBravoRenderable(ERenderPriority _RenderPriority);

	IBravoRenderable(ERenderPriority _RenderPriority, ERenderGroup _RenderGroup);

	virtual ~IBravoRenderable() = default;

	ERenderPriority GetRenderPriority() const;
	ERenderGroup GetRenderGroup() const;

	void SetVisisble(bool bNewVisible);
	virtual bool IsVisisble() const;

	void SetCastShadows(bool _bCastShadows);
	bool GetCastShadows() const;


	virtual void Render() = 0;
	virtual void RenderDepthMap(std::shared_ptr<class BravoShaderAsset> Shader) {}
	
	// selection
	virtual void RenderSelectionID() {};
	virtual void RenderOutlineMask() {};

	OnObjectClickedSignature OnObjectClicked;

	void ObjectClicked(int32 InstanceIndex = 0);

protected:
	const ERenderPriority RenderPriority = Starndart;
	const ERenderGroup RenderGroup = Main;
	
private:

	bool bCastShadows = false;
	bool bVisible = true;
	
};
