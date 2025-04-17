#include "BravoLightActor.h"


RTTR_REGISTRATION
{
	using namespace rttr;
	registration::class_<BravoLightActor>("LightActor")
		.property("LightColor", &BravoLightActor::GetLightColor, &BravoLightActor::SetLightColor)
		;
}