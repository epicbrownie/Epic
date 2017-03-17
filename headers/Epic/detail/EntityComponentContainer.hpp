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

namespace Epic::detail
{
	struct EntityComponentContainerBase;

	template<class Component>
	struct EntityComponentContainer;
}

//////////////////////////////////////////////////////////////////////////////

// EntityComponentContainerBase
struct Epic::detail::EntityComponentContainerBase
{
	virtual ~EntityComponentContainerBase() { };
};

//////////////////////////////////////////////////////////////////////////////

// EntityComponentContainer<Component>
template<class C>
struct Epic::detail::EntityComponentContainer : Epic::detail::EntityComponentContainerBase
{
	using Type = Epic::detail::EntityComponentContainer<C>;
	using Base = Epic::detail::EntityComponentContainerBase;
	using ComponentType = C;
	
	using DefaultAllocator = Epic::DefaultAllocatorFor<ComponentType, Epic::eAllocatorFor::UniquePtr>;

	ComponentType Component;

	EntityComponentContainer() { }
	EntityComponentContainer(const ComponentType& component) : Component{ component } { }
	EntityComponentContainer(ComponentType&& component) : Component{ std::move(component) } { }
};
