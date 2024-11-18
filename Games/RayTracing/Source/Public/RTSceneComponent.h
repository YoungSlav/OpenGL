#pragma once
#include "BravoComponent.h"


enum ERTComponentType : uint32
{
	ERTComponentType_Sphere = 0,
	ERTComponentType_Plane = 1
};

struct RTMaterial
{
	alignas(16) glm::vec3 Albedo = glm::vec3(0.0f);
	alignas(4) float Metallic = 0.0f;
	alignas(4) float Roughness = 0.0f;
	alignas(4) float Specular = 0.0f;
};

struct RTSceneComponentData
{
	alignas(4) ERTComponentType Type;

	alignas(16) glm::mat4 Model;
	alignas(16) glm::mat4 ModelInverse;

	alignas(16) RTMaterial Material;
};

typedef MulticastDelegate<const class RTSceneComponent*> OnComponentUpdatedSignature;

class RTSceneComponent : public BravoComponent
{
public:
	template <typename... Args>
	RTSceneComponent(ERTComponentType Type, Args&&... args) :
		BravoComponent(std::forward<Args>(args)...),
		ComponentType(Type)
	{}

	ERTComponentType GetComponentType() const { return ComponentType; }

	const RTMaterial& GetMaterial() const;
	void SetMaterial(const RTMaterial& NewMaterial);

	OnComponentUpdatedSignature OnComponentUpdatedDelegate;

protected:
	virtual bool Initialize_Internal() override;
	void OnMyTransformUpdated(const IBravoTransformable*);
private:

	RTMaterial Material;

	const ERTComponentType ComponentType;

	std::shared_ptr<class BravoStaticMeshComponent> PreviewComponent;
};