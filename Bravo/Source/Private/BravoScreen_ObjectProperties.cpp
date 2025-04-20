#include "BravoScreen_ObjectProperties.h"

#include "BravoActor.h"
#include "BravoComponent.h"
#include "BravoEngine.h"


#define REGISTER_HANDLER(map, Type) \
    map.insert_or_assign(rttr::type::get<Type>(), [this](rttr::variant& var, rttr::property& prop, rttr::instance& inst, const std::string& ParentName) { \
        Type& value = var.get_value<Type>(); \
        return HandleValue(value, var, prop, inst, ParentName); \
    });

bool BravoScreen_ObjectProperties::Initialize_Internal()
{
	if ( !BravoScreen::Initialize_Internal() || TargetObject == nullptr )
		return false;

	REGISTER_HANDLER(DispatchTable, float);
	REGISTER_HANDLER(DispatchTable, std::string);
	REGISTER_HANDLER(DispatchTable, glm::vec3);
	REGISTER_HANDLER(DispatchTable, BravoObject*);
	REGISTER_HANDLER(DispatchTable, BravoHandle);

	SetTrueScaling(false);
	SetSize(glm::vec2(0.6f, 1.0f));
	SetMaxSize(glm::vec2(1.0f, 1.0f));
	SetOrigin(glm::vec2(1.0f, 0.0f));
	SetPosition(glm::vec2(1.0f, 0.0f));
	return true;
}


void BravoScreen_ObjectProperties::Render_Internal(float DeltaTime)
{
	BravoScreen::Render_Internal(DeltaTime);

	ImGui::SetNextWindowBgAlpha(1.0f);
	ImGui::Begin(GetLabel().c_str(), nullptr,
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoCollapse);
		
		rttr::variant var(TargetObject);
		ShowProperties(TargetObject);
		//ShowProperties(TargetObject, "");

	ImGui::End();

}

inline rttr::instance unwrap(const rttr::instance& inst)
{
	if ( inst.get_wrapped_instance().is_valid() )
		return unwrap(inst.get_wrapped_instance());
	return inst;
}

void BravoScreen_ObjectProperties::ShowProperties(std::shared_ptr<class BravoObject> TargetObject)
{
	rttr::instance inputInstance(TargetObject);
	rttr::instance objInstance = unwrap(inputInstance);
	rttr::type objType = objInstance.get_derived_type();
	if ( !objType.is_valid() )
		return;

	for (rttr::property prop : objType.get_properties())
	{
		rttr::variant value = prop.get_value(objInstance);
		rttr::type valueType = prop.get_type();
		std::string propName = std::string(prop.get_name().data(), prop.get_name().size());


		if (value.is_sequential_container())
		{
			// TODO
			continue;
		}
		
		if ( Dispatch(value, prop, objInstance, TargetObject->GetName() + "." + propName) )
			prop.set_value(objInstance, value);
	}

	return;
}

float BravoScreen_ObjectProperties::DrawLabel(const std::string& propName, const std::string& parentName) const
{
	const float totalWidth = ImGui::GetContentRegionAvail().x;
	const float nameWidth = totalWidth * NameWidthPercent;
	const float valueWidth = totalWidth - nameWidth;
	ImVec2 start = ImGui::GetCursorScreenPos();
	ImGui::SetNextItemWidth(nameWidth);
	ImGui::LabelText(("##" + parentName + "."+ propName).c_str(), (propName + ": ").c_str());
	ImGui::SameLine();
	return valueWidth - 8;
}

bool BravoScreen_ObjectProperties::Dispatch(rttr::variant& var, rttr::property& prop, rttr::instance& inst, const std::string& ParentName)
{
	
	auto it = DispatchTable.find(var.get_type());
	if ( it != DispatchTable.end() )
	{
		return it->second(var, prop, inst, ParentName);
	}

	rttr::instance inputInstance(var);
	rttr::instance objInstance = unwrap(inputInstance);
	rttr::type objType = objInstance.get_type();
	it = DispatchTable.find(objType);
	if ( it != DispatchTable.end() )
	{
		return it->second(var, prop, inst, ParentName);
	}
	return HandleGeneric(var, prop, inst, ParentName);
}

bool BravoScreen_ObjectProperties::HandleGeneric(rttr::variant& var, rttr::property& prop, rttr::instance& inst, const std::string& ParentName)
{
	rttr::instance inputInstance(var);
	rttr::instance objInstance = unwrap(inputInstance);
	rttr::type objType = objInstance.get_derived_type();
	
	std::string propName = std::string(prop.get_name().data(), prop.get_name().size());	
	if ( !objType.is_valid() )
		return false;

	bool bModify = false;
	if (ImGui::CollapsingHeader((propName + "##." + ParentName).c_str(), ImGuiTreeNodeFlags_DefaultOpen))
	{
		for (rttr::property prop : objType.get_properties())
		{
			rttr::variant value = prop.get_value(objInstance);
			rttr::type valueType = prop.get_type();
			std::string propName = std::string(prop.get_name().data(), prop.get_name().size());
			if (value.is_sequential_container())
			{
				// TODO
				continue;
			}
		
			if ( Dispatch(value, prop, objInstance, ParentName + "." + propName) )
			{
				prop.set_value(objInstance, value);
				bModify = true;
			}
		}
	}
	return bModify;
}

bool BravoScreen_ObjectProperties::HandleValue(BravoHandle&, rttr::variant& var, rttr::property& prop, rttr::instance& inst, const std::string& ParentName)
{
	return false;
}
bool BravoScreen_ObjectProperties::HandleValue(BravoObject*& val, rttr::variant& var, rttr::property& prop, rttr::instance& inst, const std::string& ParentName)
{
	std::string propName = std::string(prop.get_name().data(), prop.get_name().size());
	const float valueWidth = DrawLabel(propName, ParentName);
	ImGui::SetNextItemWidth(valueWidth);
	ImGui::LabelText(("##" + ParentName + "."+ propName + "." + val->GetName()).c_str(), val->GetName().c_str());
	return false;
}
bool BravoScreen_ObjectProperties::HandleValue(glm::vec3& val, rttr::variant& var, rttr::property& prop, rttr::instance& inst, const std::string& ParentName)
{
	bool bModify = false;
	std::string propName = std::string(prop.get_name().data(), prop.get_name().size());

	const float valueWidth = DrawLabel(propName, ParentName);
	const float itemWidth = valueWidth / 3.0f - 4;

	ImGui::SetNextItemWidth(itemWidth);
	if ( ImGui::InputFloat(("##" + ParentName + "."+ propName + ".x").c_str(), &val.x) )
		bModify = true;
	ImGui::SameLine();
	ImGui::SetNextItemWidth(itemWidth);
	if ( ImGui::InputFloat(("##" + ParentName + "."+ propName + ".y").c_str(), &val.y) )
		bModify = true;
	ImGui::SameLine();
	ImGui::SetNextItemWidth(itemWidth);
	if ( ImGui::InputFloat(("##" + ParentName + "."+ propName + ".z").c_str(), &val.z) )
		bModify = true;

	if ( bModify )
		prop.set_value(inst, val);
	
	return bModify;
}
bool BravoScreen_ObjectProperties::HandleValue(float& val, rttr::variant& var, rttr::property& prop, rttr::instance& inst, const std::string& ParentName)
{
	std::string propName = std::string(prop.get_name().data(), prop.get_name().size());
	const float valueWidth = DrawLabel(propName, ParentName);
	ImGui::SetNextItemWidth(valueWidth);

	if (ImGui::InputFloat((propName + "##" + ParentName).c_str(), &val))
	{
		prop.set_value(inst, val);
		return true;
	}
	return false;
}
bool BravoScreen_ObjectProperties::HandleValue(std::string& val, rttr::variant& var, rttr::property& prop, rttr::instance& inst, const std::string& ParentName)
{
	std::string propName = std::string(prop.get_name().data(), prop.get_name().size());
	
	const float valueWidth = DrawLabel(propName, ParentName);
	ImGui::SetNextItemWidth(valueWidth);

	if ( ImGui::InputText(
		("##" + propName + "##" + ParentName).c_str(),
		val.data(),
		val.capacity() + 1,
		ImGuiInputTextFlags_CallbackResize,
		[](ImGuiInputTextCallbackData* data) -> int {
			if (data->EventFlag == ImGuiInputTextFlags_CallbackResize)
			{
				std::string* str = static_cast<std::string*>(data->UserData);
				str->resize(data->BufTextLen);
				data->Buf = str->data();
			}
			return 0;
		},
		&val) )
	{
		prop.set_value(inst, val);
		return true;
	}
	return false;
}