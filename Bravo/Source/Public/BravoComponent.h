#pragma once
#include "stdafx.h"
#include "BravoObject.h"
#include "BravoMath.h"
#include "IBravoTransformable.h"

class BravoComponent : public BravoObject, public IBravoTransformable
{
public:
	template <typename... Args>
	BravoComponent(Args&&... args) :
		BravoObject(std::forward<Args>(args)...),
		IBravoTransformable()
	{}


	std::shared_ptr<class BravoActor> GetOwningActor() const;
protected:
	virtual bool Initialize_Internal() override;
};