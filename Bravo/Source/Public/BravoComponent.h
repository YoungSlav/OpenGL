#pragma once
#include "stdafx.h"
#include "BravoObject.h"
#include "BravoMath.h"
#include "ITransformable.h"

class BravoComponent : public BravoObject, public ITransformable
{
public:

	virtual BravoTransform GetTransform_World() const override;
	virtual void SetTransform_World(const BravoTransform& InTransform) override;
	


	std::shared_ptr<class BravoActor> GetOwningActor() const;
protected:
	virtual bool Initialize_Internal() override;
};