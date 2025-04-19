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