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

	void ShowProperties(std::shared_ptr<class BravoObject> TargetObject);

protected:
	virtual bool Initialize_Internal() override;
	virtual void Render_Internal(float DeltaTime) override;


	bool HandleGeneric(rttr::variant& var, rttr::property& prop, rttr::instance& inst, const std::string& ParentName);

	bool Dispatch(rttr::variant& var, rttr::property& prop, rttr::instance& inst, const std::string& ParentName);
	bool HandleValue(float&, rttr::variant& var, rttr::property& prop, rttr::instance& inst, const std::string& ParentName);
	bool HandleValue(std::string&, rttr::variant& var, rttr::property& prop, rttr::instance& inst, const std::string& ParentName);
	bool HandleValue(glm::vec3&, rttr::variant& var, rttr::property& prop, rttr::instance& inst, const std::string& ParentName);
	bool HandleValue(BravoObject*&, rttr::variant& var, rttr::property& prop, rttr::instance& inst, const std::string& ParentName);
	bool HandleValue(BravoHandle&, rttr::variant& var, rttr::property& prop, rttr::instance& inst, const std::string& ParentName);


	float DrawLabel(const std::string& prop, const std::string& parent) const;

private:
	std::shared_ptr<class BravoObject> TargetObject;

	using HandlerFn = std::function<bool(rttr::variant&, rttr::property&, rttr::instance&, const std::string&)>;
	std::unordered_map<rttr::type, HandlerFn> DispatchTable;

	const float NameWidthPercent = 0.35f;
};

