#pragma once
#include "BravoActor.h"


class BravoLightActor : public BravoActor
{
	RTTR_ENABLE(BravoActor);
public:
	template <typename... Args>
	BravoLightActor(const glm::vec3& _LightColor, Args&&... args) :
		BravoActor(std::forward<Args>(args)...),
		LightColor(_LightColor)
	{}

	void SetLightColor(const glm::vec3& InLightColor) { LightColor = InLightColor; }
	const glm::vec3& GetLightColor() const { return LightColor; }

protected:
	glm::vec3 LightColor;
};

