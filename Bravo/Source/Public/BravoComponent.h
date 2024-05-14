#pragma once
#include "stdafx.h"
#include "BravoObject.h"
#include "BravoMath.h"
#include "ITransformable.h"

class BravoComponent : public BravoObject, public ITransformable
{
public:

	std::shared_ptr<class BravoActor> GetOwningActor() const;
protected:
	virtual bool Initialize_Internal() override;
};