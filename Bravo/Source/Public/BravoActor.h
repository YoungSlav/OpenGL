#pragma once
#include "stdafx.h"
#include "BravoObject.h"
#include "BravoMath.h"
#include "ITransformable.h"
#include "BravoComponent.h"

class BravoActor : public BravoObject, public ITransformable
{
public:

	const std::vector<std::shared_ptr<BravoComponent>>& GetComponents() const { return Components; }

protected:
	virtual void OnChildObjectAdded(std::weak_ptr<BravoObject> _OwnedObject) override;
	virtual void OnDestroy() override;

protected:

	std::vector<std::shared_ptr<BravoComponent>> Components;
};
