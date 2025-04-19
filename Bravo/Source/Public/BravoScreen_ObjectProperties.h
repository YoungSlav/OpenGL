#pragma once

#include "stdafx.h"
#include "BravoScreen.h"

class BravoScreen_ObjectProperties : public BravoScreen
{
public:
	template <typename... Args>
	BravoScreen_ObjectProperties(std::shared_ptr<class BravoObject> _TargetObject, Args&&... args) :
		BravoScreen(std::forward<Args>(args)...),
		TargetObject(_TargetObject)
	{}


protected:
	virtual bool Initialize_Internal() override;
	virtual void Render_Internal(float DeltaTime) override;

private:
	bool ShowProperties(rttr::instance var, const std::string& ParentName);
private:
	std::shared_ptr<class BravoObject> TargetObject;

};

