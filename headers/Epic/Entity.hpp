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
//
//    This simple ECS system was inspired by Sam Bloomberg's ECS system
//        available for download at: https://github.com/redxdev/ECS
//
//////////////////////////////////////////////////////////////////////////////

#pragma once

#include <Epic/StringHash.hpp>
#include <Epic/STL/Map.hpp>
#include <Epic/STL/UniquePtr.hpp>
#include <Epic/detail/EntityComponent.hpp>
#include <Epic/detail/EntityComponentContainer.hpp>
#include <Epic/detail/EntityManagerFwd.hpp>
#include <functional>

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	class Entity;
}

//////////////////////////////////////////////////////////////////////////////

// Entity
class Epic::Entity
{
public:
	using Type = Epic::Entity;

public:
	friend class Epic::EntityManager;

	constexpr static Epic::StringHash NoEntityName = Epic::Hash("");

private:
	using ComponentPtr = Epic::ImplPtr<Epic::detail::EntityComponentContainerBase>;
	using ComponentMap = Epic::STLUnorderedMap<Epic::detail::EntityComponentID, ComponentPtr>;

private:
	ComponentMap m_Components;				// Maps component type IDs to instances of component data
	Epic::EntityManager* m_pEntityManager;	// This Entity's controlling EntityManager
	Epic::StringHash m_Name;				// This Entity's name
	size_t m_ID;							// This Entity's ID (assigned by controlling EntityManager)
	bool m_DestroyPending;					// Whether or not this Entity is awaiting destruction

public:
	inline Entity(Epic::EntityManager* pSystem, const Epic::StringHash& name, const size_t id) noexcept
		: m_pEntityManager{ pSystem }, m_Name{ name }, m_ID{ id }, m_DestroyPending { false }
	{ }

	~Entity() noexcept 
	{
		EraseAll();
	}

public:
	constexpr Epic::EntityManager* GetEntityManager() const noexcept
	{
		return m_pEntityManager;
	}

	constexpr const Epic::StringHash& GetName() const
	{
		return m_Name;
	}

	constexpr const size_t GetID() const noexcept
	{
		return m_ID;
	}

	constexpr bool IsDestroyPending() const
	{
		return m_DestroyPending;
	}

protected:
	// Mark this Entity for destruction.
	// NOTE: This Entity's controlling EntityManager will handle this.
	void Destroy() noexcept
	{
		m_DestroyPending = true;
	}

public:
	// Query whether or not this Entity has a component
	template<class Component>
	inline bool Has() const noexcept
	{
		////////////////////////////////////////////////////////////////////////////////
		//
		//  If the compiler is complaining about 'ID' not being a member, it's likely
		//  because the component type it's trying to query has no traits defined for it.
		//  To define the traits, simply call the MAKE_ENTITY_COMPONENT macro.
		//
		//	Example: 
		//			namespace MyNamespace {
		//				struct MyComponent { ... Data Fields ... };
		//			}
		//
		//			MAKE_ENTITY_COMPONENT(MyComponent);
		//
		////////////////////////////////////////////////////////////////////////////////

		return m_Components.find(detail::EntityComponentTraits<Component>::ID) != std::end(m_Components);
	}

	// Query whether or not this Entity has ALL the components
	template<class Component1, class Component2, class... Components>
	inline bool Has() const noexcept
	{
		return Has<Component1>() && Has<Component2, Components...>();
	}

public:
	// Attach a component to this Entity.
	// Args... are forwarded to Component's ctor
	template<class Component, class... Args>
	Component& Assign(Args&&... args) noexcept
	{
		////////////////////////////////////////////////////////////////////////////////
		//
		//  If the compiler is complaining about 'ID' not being a member, it's likely
		//  because the component type it's trying to add has no traits defined for it.
		//  To define the traits, simply call the MAKE_ENTITY_COMPONENT macro.
		//
		//	Example: 
		//			namespace MyNamespace {
		//				struct MyComponent { ... Data Fields ... };
		//			}
		//
		//			MAKE_ENTITY_COMPONENT(MyComponent);
		//
		////////////////////////////////////////////////////////////////////////////////

		auto it = m_Components.find(detail::EntityComponentTraits<Component>::ID);

		if (it != std::end(m_Components))
		{
			auto pContainer = static_cast<detail::EntityComponentContainer<Component>*>((*it).second.get());
			pContainer->Component = Component{ std::forward<Args>(args)... };
			
			return pContainer->Component;
		}
		else
		{
			auto pContainerBase 
				= Epic::MakeImpl<detail::EntityComponentContainerBase, 
								 detail::EntityComponentContainer<Component>>
									 (Component{ std::forward<Args>(args)... });
			auto pContainer = static_cast<detail::EntityComponentContainer<Component>*>(pContainerBase.get());
			
			m_Components[detail::EntityComponentTraits<Component>::ID] = std::move(pContainerBase);

			return pContainer->Component;
		}
	}

	// Erase a component from this Entity.
	// Returns whether or not the component was found and removed.
	template<class Component>
	bool Erase() noexcept
	{
		auto it = m_Components.find(detail::EntityComponentTraits<Component>::ID);
		if (it != std::end(m_Components))
		{
			m_Components.erase(it);
			return true;
		}

		return false;
	}

	// Erase all components from this Entity.
	inline void EraseAll() noexcept
	{
		m_Components.clear();
	}

	// Get a component that has been attached to this Entity.
	// Will fail an assertion if this Entity does not have the component.
	// Use Has<Component>() if unsure.
	template<class Component>
	Component& Get() noexcept
	{
		auto it = m_Components.find(detail::EntityComponentTraits<Component>::ID);
		assert(it != std::end(m_Components));

		auto pContainer = static_cast<detail::EntityComponentContainer<Component>*>((*it).second.get());
		return pContainer->Component;
	}

public:
	// Calls 'fn', passing references to component data, if this Entity
	// has ALL Components.
	// Returns whether or not the function was called.
	template<class... Components>
	bool With(std::function<void(Components&...)> fn) noexcept
	{
		if (!Has<Components...>())
			return false;

		fn(Get<Components>()...);
		return true;
	}
};
