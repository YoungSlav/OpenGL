#pragma once
#include <memory>
#include <type_traits>

class _SharedFromThis : public std::enable_shared_from_this<_SharedFromThis>
{
public:
	_SharedFromThis() = default;
	virtual ~_SharedFromThis() {}
private:
	_SharedFromThis(const _SharedFromThis& ) = delete;
};

class SharedFromThis : virtual public _SharedFromThis
{
protected:
	template <typename ChildClass>
	std::shared_ptr<ChildClass> Self()
	{
		if ( !std::is_base_of_v<SharedFromThis, ChildClass> )
			return nullptr;
		
		return std::dynamic_pointer_cast<ChildClass>(shared_from_this());
	}
};