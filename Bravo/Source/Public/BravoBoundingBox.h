#pragma once

#include "BravoEngine.h"
#include "BravoComponent.h"
#include "IBravoRenderable.h"

class BravoBoundingBox : public BravoComponent, public IBravoRenderable
{

public:
	
	template <typename... Args>
	BravoBoundingBox(Args&&... args) :
		BravoComponent(std::forward<Args>(args)...),
		IBravoRenderable(ERenderPriority::Late)
	{}

	void SetColor(const glm::vec3& _Color) { Color = _Color; }
	void SetEdgeSize(const float _EdgeSize) { EdgeSize = _EdgeSize; }

protected:
	void UpdateVertices(const class IBravoTransformable* );

	virtual bool Initialize_Internal() override;
	virtual void Render() override;
	virtual void OnDestroy() override;



private:

	float EdgeSize = 0.3f;
	glm::vec3 Color = glm::vec3(1.0f);

	GLuint VAO = 0;
	GLuint VBO = 0;
	GLuint EBO = 0;
	std::shared_ptr<class BravoShaderAsset> Shader;
};