#include "IBravoRenderable.h"

RTTR_REGISTRATION
{
	using namespace rttr;

    registration::enumeration<ERenderPriority>("ERenderPriority")
    (
        value("Early", ERenderPriority::Early),
        value("Starndart", ERenderPriority::Starndart),
        value("Late", ERenderPriority::Late)
    );

    registration::enumeration<ERenderGroup>("ERenderGroup")
    (
        value("Main", ERenderGroup::Main),
        value("Overlay", ERenderGroup::Overlay)
    );

	registration::class_<IBravoRenderable>("IRenderable")
        .property_readonly("RenderPriority", &IBravoRenderable::GetRenderPriority)
        .property_readonly("RenderGroup", &IBravoRenderable::GetRenderGroup)
        .property("CastShadows", &IBravoRenderable::GetCastShadows, &IBravoRenderable::SetCastShadows)
        .property("Visible", &IBravoRenderable::IsVisisble, &IBravoRenderable::SetVisisble)
        ;
}

IBravoRenderable::IBravoRenderable(ERenderPriority _RenderPriority) :
	RenderPriority(_RenderPriority),
	RenderGroup(ERenderGroup::Main)
{}

IBravoRenderable::IBravoRenderable(ERenderPriority _RenderPriority, ERenderGroup _RenderGroup) :
	RenderPriority(_RenderPriority),
	RenderGroup(_RenderGroup)
{}


ERenderPriority IBravoRenderable::GetRenderPriority() const { return RenderPriority; }
ERenderGroup IBravoRenderable::GetRenderGroup() const { return RenderGroup; }

void IBravoRenderable::SetVisisble(bool bNewVisible)
{
    bVisible = bNewVisible;
}
bool IBravoRenderable::IsVisisble() const { return bVisible; }

void IBravoRenderable::SetCastShadows(bool _bCastShadows)
{
    bCastShadows = _bCastShadows;
}
bool IBravoRenderable::GetCastShadows() const { return bCastShadows; }

void IBravoRenderable::ObjectClicked(int32 InstanceIndex)
{
	OnObjectClicked.Broadcast(InstanceIndex);
};