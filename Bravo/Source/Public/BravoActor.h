#pragma once
#include "stdafx.h"
#include "BravoObject.h"
#include "BravoMath.h"
#include "IBravoTransformable.h"
#include "BravoComponent.h"

class BravoActor : public BravoObject, public IBravoTransformable
{
	RTTR_ENABLE(BravoObject, IBravoTransformable);
public:
	template <typename... Args>
	BravoActor(Args&&... args) :
		BravoObject(std::forward<Args>(args)...),
		IBravoTransformable()
	{}

	const std::vector<std::shared_ptr<BravoComponent>>& GetComponents() const { return Components; }

protected:
	virtual void OnChildObjectAdded(std::weak_ptr<BravoObject> _OwnedObject) override;
	virtual void OnDestroy() override;

protected:

	std::vector<std::shared_ptr<BravoComponent>> Components;
};
