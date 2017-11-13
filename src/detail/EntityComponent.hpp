//////////////////////////////////////////////////////////////////////////////
//
//            Copyright (c) 2017 Ronnie Brohn (EpicBrownie)      
//
//                Distributed under The MIT License (MIT).
//             (See accompanying file License.txt or copy at 
//                 https://opensource.org/licenses/MIT)
//
//           Please report any bugs, typos, or suggestions to
//              https://github.com/epicbrownie/Epic/issues
//
//////////////////////////////////////////////////////////////////////////////

#pragma once

#include <Epic/Memory/Default.hpp>

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	namespace detail
	{
		struct EntityComponentBase;

		template<class Component>
		struct EntityComponent;
	}
}

//////////////////////////////////////////////////////////////////////////////

// EntityComponentBase
struct Epic::detail::EntityComponentBase
{
	virtual ~EntityComponentBase() { };
};

//////////////////////////////////////////////////////////////////////////////

// EntityComponent<Component>
template<class C>
struct Epic::detail::EntityComponent : Epic::detail::EntityComponentBase
{
	using Type = Epic::detail::EntityComponent<C>;
	using Base = Epic::detail::EntityComponentBase;
	using ComponentType = C;
	
	using DefaultAllocator = Epic::DefaultAllocatorFor<ComponentType, Epic::eAllocatorFor::UniquePtr>;

	ComponentType Component;

	EntityComponent() noexcept { }
	EntityComponent(const ComponentType& component) noexcept : Component{ component } { }
	EntityComponent(ComponentType&& component) noexcept : Component{ std::move(component) } { }
};
