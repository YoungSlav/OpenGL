#pragma once
#include "stdafx.h"
#include "BravoObject.h"
#include "IBravoRenderable.h"
#include "BravoTransform2D.h"

class FluidContainer : public BravoObject, public IBravoRenderable
{
public:
	template <typename... Args>
	FluidContainer(Args&&... args) :
		BravoObject(std::forward<Args>(args)...),
		IBravoRenderable(ERenderPriority::Late)
	{}

	void SetLocation(const glm::vec2& Location) { Transform.SetLocation(Location); }
	void SetRotation(const float Rotation) { Transform.SetRotation(Rotation); }
	void SetSize(const glm::vec2& Size) { Transform.SetScale(Size); }
	const glm::vec2& GetSize(bool Inside = true) const;

	void SetTransform(const BravoTransform2D& _Transform) { Transform = _Transform; }
	bool CheckRoundCollision(glm::vec2& Location, float Radius, glm::vec2& OutVelocityModify) const;


	void SetOutlineColor(const glm::vec3& _color) { OutlineColor = _color; }

	void SetSimulation(std::shared_ptr<class FluidSimulation> _Simulation) { Simulation = _Simulation; }



protected:
	virtual bool Initialize_Internal() override;
	virtual void Render() override;

protected:

	BravoTransform2D Transform;

	glm::vec3 Color = glm::vec3(0.0f);
	glm::vec3 OutlineColor = glm::vec3(1.0f);

private:
	std::shared_ptr<class BravoShaderAsset> Shader;
	std::shared_ptr<class FluidSimulation> Simulation;
	GLuint VAO = 0;
	GLuint VBO = 0;

	const float BorderWidth = 1.0f;
};
