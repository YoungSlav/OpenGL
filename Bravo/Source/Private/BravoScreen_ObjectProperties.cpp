#include "BravoScreen_ObjectProperties.h"

#include "BravoActor.h"
#include "BravoComponent.h"
#include "BravoEngine.h"

bool BravoScreen_ObjectProperties::Initialize_Internal()
{
	if ( !BravoScreen::Initialize_Internal() || TargetObject == nullptr )
		return false;

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
		ImGuiWindowFlags_NoCollapse);
		
		ShowProperties(TargetObject, "");

	ImGui::End();

}

bool BravoScreen_ObjectProperties::ShowProperties(rttr::instance inputInstance, const std::string& ParentName)
{
	bool bModification = false;
	rttr::instance objInstance = inputInstance.get_wrapped_instance().is_valid() ? inputInstance.get_wrapped_instance() : inputInstance;
	rttr::type objType = objInstance.get_derived_type();
	for (auto& prop : objType.get_properties())
	{
		rttr::variant value = prop.get_value(objInstance);
		rttr::type valueType = prop.get_type();
		std::string propName = std::string(prop.get_name().data(), prop.get_name().size());
		
		if (value.is_sequential_container())
		{
			continue;
		}
		else if (valueType.is_class() && !valueType.get_properties().empty() && value.is_valid())
		{
			ImGui::Text(propName.c_str());
			if ( ShowProperties(value, ParentName + "." + propName) )
			{
				prop.set_value(objInstance, value);
				bModification = true;
			}
		}
		else if (valueType.is_arithmetic())
		{
			if (valueType == rttr::type::get<int>())
			{
				int& v = value.get_value<int>();
				if (ImGui::InputInt((propName + "##" + ParentName).c_str(), &v))
				{
					prop.set_value(objInstance, v);
					bModification = true;
				}
			}
			else if (valueType == rttr::type::get<float>())
			{
				float& v = value.get_value<float>();
				if (ImGui::InputFloat((propName + "##" + ParentName).c_str(), &v))
				{
					prop.set_value(objInstance, v);
					bModification = true;
				}
			}
			else if (valueType == rttr::type::get<double>())
			{
				double& v = value.get_value<double>();
				if (ImGui::InputDouble((propName + "##" + ParentName).c_str(), &v))
				{
					prop.set_value(objInstance, v);
					bModification = true;
				}
			}
			else if (valueType == rttr::type::get<bool>())
			{
				bool& v = value.get_value<bool>();
				if (ImGui::Checkbox((propName + "##" + ParentName).c_str(), &v))
				{
					prop.set_value(objInstance, v);
					bModification = true;
				}
			}
		}
		else if (valueType == rttr::type::get<std::string>())
 		{
			//if ( tmp.empty() ) tmp = value.get_value<std::string>();
 			std::string& str = value.get_value<std::string>();
				
			if ( ImGui::InputText(
				(propName + "##" + ParentName).c_str(),
				str.data(),
				str.capacity() + 1,
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
				&str) )
			{
				prop.set_value(objInstance, str);
				bModification = true;
			}
 		}
	}
	return bModification;
}
