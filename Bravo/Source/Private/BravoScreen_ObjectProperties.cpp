#include "BravoScreen_ObjectProperties.h"

#include "BravoActor.h"
#include "BravoComponent.h"
#include "BravoInput.h"
#include "BravoEngine.h"
#include "BravoSelectionManager.h"
#include "BravoStaticMeshComponent.h"

#define REGISTER_HANDLER(map, Type) \
    map.insert_or_assign(rttr::type::get<Type>(), [this](rttr::variant& var, const std::string& propName, rttr::instance& inst, const std::string& ParentName) { \
        Type& value = var.get_value<Type>(); \
        return HandleValue(value, var, propName, inst, ParentName); \
    });

bool BravoScreen_ObjectProperties::Initialize_Internal()
{
	if ( !BravoScreen::Initialize_Internal() )
		return false;

	REGISTER_HANDLER(DispatchTable, float);
	REGISTER_HANDLER(DispatchTable, bool);
	REGISTER_HANDLER(DispatchTable, std::string);
	REGISTER_HANDLER(DispatchTable, glm::vec3);
	REGISTER_HANDLER(DispatchTable, BravoObject*);
	REGISTER_HANDLER(DispatchTable, BravoStaticMeshInstance*);
	REGISTER_HANDLER(DispatchTable, BravoHandle);

	if ( Engine->GetInput() )
	{
		BravoKeySubscription sub;
		sub.Key = GLFW_KEY_GRAVE_ACCENT;
		sub.SubscribedType = EKeySubscriptionType::Released;
		sub.Callback.BindSP(Self<BravoScreen_ObjectProperties>(), &BravoScreen_ObjectProperties::OnToggleHUD);
		Engine->GetInput()->SubscribeKey(sub);
	}
	if ( Engine->GetSelectionManager() )
	{
		Engine->GetSelectionManager()->OnSelectionChanged.AddSP(Self<BravoScreen_ObjectProperties>(), &BravoScreen_ObjectProperties::OnSelectionChanged);
	}

	SetTrueScaling(false);
	SetSize(glm::vec2(0.3f, 1.0f));
	SetMaxSize(glm::vec2(1.0f, 1.0f));
	SetOrigin(glm::vec2(1.0f, 0.0f));
	SetPosition(glm::vec2(1.0f, 0.0f));
	return true;
}

void BravoScreen_ObjectProperties::OnSelectionChanged()
{
	TargetObjects.clear();
	if ( Engine->GetSelectionManager() )
	{
		auto ActiveSelections = Engine->GetSelectionManager()->GetActiveSelections();
		for ( auto it : ActiveSelections )
		{
			if ( it.second.empty() )
			{
				TargetObjects.push_back(it.first);
			}
			else
			{
				if ( std::shared_ptr<BravoStaticMeshComponent> asComponent = std::dynamic_pointer_cast<BravoStaticMeshComponent>(it.first.lock()) )
				{
					for ( int32 i : it.second )
					{
						TargetObjects.push_back(asComponent->GetInstance(i));
					}
				}
			}
		}
	}
	else
	{
		Clear();
	}
}

void BravoScreen_ObjectProperties::OnToggleHUD(bool ButtonState, float DeltaTime)
{
	bShowHUD = !bShowHUD;
}

void BravoScreen_ObjectProperties::SetTargetObjects(const std::list<std::weak_ptr<class BravoObject>>& _TargetObjects)
{
	TargetObjects = _TargetObjects;
}
void BravoScreen_ObjectProperties::Clear()
{
	TargetObjects.clear();
}


void BravoScreen_ObjectProperties::Render_Internal(float DeltaTime)
{
	if ( !bShowHUD || TargetObjects.empty() )
		return;

	BravoScreen::Render_Internal(DeltaTime);

	ImGui::SetNextWindowBgAlpha(1.0f);
	ImGui::Begin(GetLabel().c_str(), nullptr,
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoCollapse);

	
	ImGuiTreeNodeFlags ThreeFlags = ImGuiTreeNodeFlags_OpenOnArrow;
	if ( TargetObjects.size() == 1 ) ThreeFlags |= ImGuiTreeNodeFlags_DefaultOpen;
		
	for ( auto it : TargetObjects )
	{
		if ( it.expired() )
			continue;
		std::shared_ptr<BravoObject> obj = it.lock();

		if (ImGui::TreeNodeEx((obj->GetName() +"##" + std::to_string(obj->GetHandle())).c_str(), ThreeFlags ))
		{
			ShowProperties(obj);
			ImGui::TreePop();
		}

		if ( TargetObjects.size() > 1 )
			ImGui::Separator();
	}
			

	ImGui::End();

}

inline rttr::instance unwrap(const rttr::instance& inst)
{
	if ( inst.get_wrapped_instance().is_valid() )
		return unwrap(inst.get_wrapped_instance());
	return inst;
}

inline rttr::variant unwrap(const rttr::variant& var)
{
	if ( var.extract_wrapped_value() )
		return unwrap(var.extract_wrapped_value());
	return var;
}

inline rttr::type unwrap(const rttr::type& t)
{
	if ( t.is_pointer() )
		return unwrap(t.get_raw_type());
	return t;
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
		if ( prop.is_readonly() ) continue;

		rttr::variant value = prop.get_value(objInstance);
		rttr::type valueType = prop.get_type();
		std::string propName = std::string(prop.get_name().data(), prop.get_name().size());

		if ( Dispatch(value, propName, objInstance, TargetObject->GetName() + "." + propName) )
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

bool BravoScreen_ObjectProperties::Dispatch(rttr::variant& var, const std::string& propName, rttr::instance& inst, const std::string& ParentName)
{
	var = unwrap(var);
	rttr::type varType = var.get_type();

	if ( var.is_sequential_container() || var.is_associative_container() )
	{
		return HandleContainer(var, propName, inst, ParentName);
	}
	
	if ( varType.is_enumeration() )
	{
		return HandleEnumeration(var, propName, inst, ParentName);
	}

	// look for dispatch in dispach table
	// if not found, try to find parent's variant to dispatch
	
	while ( varType.is_valid() )
	{
		auto it = DispatchTable.find(varType);
		if ( it != DispatchTable.end() )
			return it->second(var, propName, inst, ParentName);

		// we need to unwrap from ptr type to raw type
		auto bases = unwrap(varType).get_base_classes();
		if ( bases.empty() )
			break;
		
		// wrap raw type back into ptr, since we operate on pointers for object types
		std::string pointerName = std::string(unwrap(*(bases.begin())).get_name()) + "*";

		varType = rttr::type::get_by_name(pointerName);
	}
	return HandleClass(var, propName, inst, ParentName);
}

bool BravoScreen_ObjectProperties::HandleClass(rttr::variant& var, const std::string& propName, rttr::instance& inst, const std::string& ParentName)
{
	rttr::instance inputInstance(var);
	rttr::instance objInstance = unwrap(inputInstance);
	rttr::type objType = objInstance.get_derived_type();
	
	if ( !objType.is_valid() )
		return false;
	
	bool bModify = false;
	if (ImGui::TreeNodeEx((propName + "##." + ParentName).c_str(), ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_DefaultOpen ))
	{
		for (rttr::property prop : objType.get_properties())
		{
			if ( prop.is_readonly() ) continue;

			rttr::variant value = prop.get_value(objInstance);
			rttr::type valueType = prop.get_type();
			std::string cPropName = std::string(prop.get_name().data(), prop.get_name().size());
			
			if ( Dispatch(value, cPropName, objInstance, ParentName + "." + cPropName) )
			{
				prop.set_value(objInstance, value);
				bModify = true;
			}
		}
		ImGui::TreePop();
	}
	return bModify;
}

bool BravoScreen_ObjectProperties::HandleContainer(rttr::variant& var, const std::string& propName, rttr::instance& inst, const std::string& ParentName)
{
	bool bModify = false;
	if ( ImGui::TreeNodeEx((propName + "##." + ParentName).c_str(), ImGuiTreeNodeFlags_OpenOnArrow ) )
	{
		rttr::variant_sequential_view view = var.create_sequential_view();
		for (size_t i = 0; i < view.get_size(); ++i)
		{
			rttr::variant elem = view.get_value(i);
			const std::string cPropName = std::to_string(i);
			if ( Dispatch(elem, cPropName, inst, ParentName + "." + propName) )
			{
				view.set_value(i, elem);
				bModify = true;
			}
		}

		ImGui::TreePop();
	}
	return bModify;
}

bool BravoScreen_ObjectProperties::HandleEnumeration(rttr::variant& var, const std::string& propName, rttr::instance& inst, const std::string& ParentName)
{
	return false;
}

bool BravoScreen_ObjectProperties::HandleValue(BravoHandle&, rttr::variant& var, const std::string& propName, rttr::instance& inst, const std::string& ParentName)
{
	return false;
}
bool BravoScreen_ObjectProperties::HandleValue(BravoStaticMeshInstance*&, rttr::variant& var, const std::string& propName, rttr::instance& inst, const std::string& ParentName)
{
	return HandleClass(var, propName, inst, ParentName);
}
bool BravoScreen_ObjectProperties::HandleValue(BravoObject*& val, rttr::variant& var, const std::string& propName, rttr::instance& inst, const std::string& ParentName)
{
	const float valueWidth = DrawLabel(propName, ParentName);
	ImGui::SetNextItemWidth(valueWidth);
	ImGui::LabelText(("##" + ParentName + "."+ propName + "." + val->GetName()).c_str(), val->GetName().c_str());
	return false;
}
bool BravoScreen_ObjectProperties::HandleValue(glm::vec3& val, rttr::variant& var, const std::string& propName, rttr::instance& inst, const std::string& ParentName)
{
	bool bModify = false;

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
		
	return bModify;
}
bool BravoScreen_ObjectProperties::HandleValue(float& val, rttr::variant& var, const std::string& propName, rttr::instance& inst, const std::string& ParentName)
{
	const float valueWidth = DrawLabel(propName, ParentName);
	ImGui::SetNextItemWidth(valueWidth);

	if (ImGui::InputFloat((propName + "##" + ParentName).c_str(), &val))
	{
		return true;
	}
	return false;
}
bool BravoScreen_ObjectProperties::HandleValue(bool& val, rttr::variant& var, const std::string& propName, rttr::instance& inst, const std::string& ParentName)
{
	return false;
}
bool BravoScreen_ObjectProperties::HandleValue(std::string& val, rttr::variant& var, const std::string& propName, rttr::instance& inst, const std::string& ParentName)
{
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
		return true;
	}
	return false;
}