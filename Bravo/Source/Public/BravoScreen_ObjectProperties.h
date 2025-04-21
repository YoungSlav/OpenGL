#pragma once

#include "stdafx.h"
#include "BravoScreen.h"

class BravoScreen_ObjectProperties : public BravoScreen
{
public:
	template <typename... Args>
	BravoScreen_ObjectProperties(Args&&... args) :
		BravoScreen(std::forward<Args>(args)...)
	{}

	void SetTargetObjects(const std::list<std::weak_ptr<class BravoObject>>& _TargetObjects);
	void Clear();

protected:
	virtual bool Initialize_Internal() override;
	virtual void Render_Internal(float DeltaTime) override;

	void ShowProperties(std::shared_ptr<class BravoObject> TargetObject);

	bool HandleClass(rttr::variant& var, const std::string& propName, rttr::instance& inst, const std::string& ParentName);
	bool HandleContainer(rttr::variant& var, const std::string& propName, rttr::instance& inst, const std::string& ParentName);
	bool HandleEnumeration(rttr::variant& var, const std::string& propName, rttr::instance& inst, const std::string& ParentName);

	bool Dispatch(rttr::variant& var, const std::string& propName, rttr::instance& inst, const std::string& ParentName);
	bool HandleValue(float&, rttr::variant& var, const std::string& propName, rttr::instance& inst, const std::string& ParentName);
	bool HandleValue(bool&, rttr::variant& var, const std::string& propName, rttr::instance& inst, const std::string& ParentName);
	bool HandleValue(std::string&, rttr::variant& var, const std::string& propName, rttr::instance& inst, const std::string& ParentName);
	bool HandleValue(glm::vec3&, rttr::variant& var, const std::string& propName, rttr::instance& inst, const std::string& ParentName);
	bool HandleValue(BravoObject*&, rttr::variant& var, const std::string& propName, rttr::instance& inst, const std::string& ParentName);
	bool HandleValue(class BravoStaticMeshInstance*&, rttr::variant& var, const std::string& propName, rttr::instance& inst, const std::string& ParentName);
	bool HandleValue(BravoHandle&, rttr::variant& var, const std::string& propName, rttr::instance& inst, const std::string& ParentName);


	float DrawLabel(const std::string& prop, const std::string& parent) const;

	void OnToggleHUD(bool ButtonState, float DeltaTime);
	void OnSelectionChanged();

private:

	std::list<std::weak_ptr<class BravoObject>> TargetObjects;

	using HandlerFn = std::function<bool(rttr::variant&, const std::string&, rttr::instance&, const std::string&)>;
	std::unordered_map<rttr::type, HandlerFn> DispatchTable;

	const float NameWidthPercent = 0.35f;

	bool bShowHUD = false;
};

