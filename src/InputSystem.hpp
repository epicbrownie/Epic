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

#include <Epic/Event.hpp>
#include <Epic/InputAction.hpp>
#include <Epic/InputContext.hpp>
#include <Epic/InputData.hpp>
#include <Epic/InputDeviceManager.hpp>
#include <Epic/InputResolver.hpp>
#include <Epic/STL/Map.hpp>
#include <Epic/STL/UniquePtr.hpp>
#include <Epic/STL/Vector.hpp>
#include <cassert>

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	class InputSystem;

	extern const Epic::StringHash GlobalContext;
}

//////////////////////////////////////////////////////////////////////////////

// InputSystem
class Epic::InputSystem
{
public:
	using Type = Epic::InputSystem;

private:
	using DeviceManagerPtr = Epic::UniquePtr<Epic::InputDeviceManager>;
	using ContextPtr = Epic::UniquePtr<Epic::InputContext>;
	using ActionPtr = Epic::UniquePtr<Epic::InputAction>;
	using InputResolverPtr = Epic::InputAction::InputResolverPtr;

	using ActionList = Epic::STLVector<ActionPtr>;
	using ActionMap = Epic::STLUnorderedMap<Epic::StringHash, ActionList::size_type>;
	using ContextMap = Epic::STLUnorderedMap<Epic::StringHash, ContextPtr>;
	using ActiveContextList = Epic::STLVector<ContextPtr::pointer>;
	using DataStream = Epic::STLVector<InputData>;

private:
	DeviceManagerPtr m_pDeviceManager;
	ActionList m_Actions;
	ActionMap m_ActionMap;
	ContextMap m_Contexts;
	ActiveContextList m_ActiveContexts;
	DataStream m_DataStream;
	bool m_SafeToIterateContexts;
	bool m_SafeToIterateBindings;

public:
	InputSystem() noexcept
		: m_SafeToIterateContexts{ true }, m_SafeToIterateBindings{ true }
	{
		m_pDeviceManager = Epic::MakeUnique<Epic::InputDeviceManager>();
		m_pDeviceManager->Input.Connect(this, &Type::OnDeviceInput);

		ActivateContext(GlobalContext);
	}

	~InputSystem() noexcept = default;

	InputSystem(const Type&) = delete;
	Type& operator = (const Type&) = delete;

public:
	inline void Update() noexcept
	{
		m_pDeviceManager->UpdateDevices();

		for (const auto& data : m_DataStream)
			ProcessInput(data);

		m_DataStream.shrink_to_fit();
		m_DataStream.clear();
	}

private:
	InputContext* GetContext(const Epic::StringHash contextName) noexcept
	{
		auto pContext = m_Contexts[contextName].get();

		if (!pContext)
		{
			auto newContext = Epic::MakeUnique<Epic::InputContext>(contextName);
			pContext = newContext.get();
			m_Contexts[contextName] = std::move(newContext);
		}

		return pContext;
	}

	InputAction* GetAction(const Epic::StringHash actionName) noexcept
	{
		auto itAction = m_ActionMap.find(actionName);
		
		if (itAction == std::end(m_ActionMap))
		{
			m_Actions.emplace_back(Epic::MakeUnique<Epic::InputAction>(actionName));
			m_ActionMap[actionName] = m_Actions.size() - 1;
			
			return m_Actions.back().get();
		}
		else
			return m_Actions[(*itAction).second].get();
	}

	void ProcessInput(const Epic::InputData& data)
	{
		bool isConsumed = false;
		m_SafeToIterateContexts = true;

		// Iterate over active contexts
		for (auto itContext = m_ActiveContexts.rbegin(); itContext != m_ActiveContexts.rend(); ++itContext)
		{
			// Iterate over bindings in each active context
			for (auto actionName : *(*itContext))
			{
				// A quick safety check.  If this context's bind list is changed while we are
				// iterating, this flag will be cleared and the remaining binds will not be checked.
				m_SafeToIterateBindings = true;

				auto pAction = m_Actions[m_ActionMap[actionName]].get();

				for (InputAction::Slot s = 0; s < InputAction::Slots; ++s)
				{
					auto pResolver = pAction->GetResolver(s);
					if (pResolver && pResolver->Resolve(data))
						isConsumed = pAction->Action.InvokeUntil(true, data);

					if (isConsumed || !m_SafeToIterateBindings || !m_SafeToIterateContexts)
						break;
				}

				if (isConsumed || !m_SafeToIterateBindings || !m_SafeToIterateContexts)
					break;
			}

			if (isConsumed || !m_SafeToIterateContexts)
				break;
		}

		m_SafeToIterateContexts = false;
		m_SafeToIterateBindings = false;
	}

private:
	void OnDeviceInput(const Epic::InputData& data)
	{
		m_DataStream.emplace_back(data);
	}

public:
	// Get the device manager for this input system
	inline Epic::InputDeviceManager* GetDeviceManager() const noexcept
	{
		return m_pDeviceManager.get();
	}

	// Get the event for an Action.
	// If it does not exist, it will be created.
	auto Action(const Epic::StringHash actionName) noexcept -> decltype(InputAction::Action)&
	{
		return GetAction(actionName)->Action;
	}

	// Activate an input context. If it does not exist, it will be created.
	void ActivateContext(const Epic::StringHash contextName) noexcept
	{
		auto pContext = GetContext(contextName);

		auto it = std::find(std::begin(m_ActiveContexts), std::end(m_ActiveContexts), pContext);
		if (it == std::end(m_ActiveContexts))
		{
			m_ActiveContexts.emplace_back(pContext);
			m_SafeToIterateContexts = false;
		}
	}

	// Deactivate an input context. If it does not exist, it will be created.
	void DeactivateContext(const Epic::StringHash contextName) noexcept
	{
		assert(contextName != GlobalContext);

		auto pContext = GetContext(contextName);

		auto it = std::find(std::begin(m_ActiveContexts), std::end(m_ActiveContexts), pContext);
		if (it != m_ActiveContexts.end())
		{
			m_ActiveContexts.erase(it);
			m_SafeToIterateContexts = false;
		}
	}

	// Create a binding between an Action and a resolver for a context.
	// If 'clearFirst' is true, all other bindings for this Action will be cleared first.
	// The binding will occur in the first available slot.
	// Returns whether or not the Bind was successful.
	bool Bind(const Epic::StringHash actionName, const Epic::StringHash contextName, InputResolverPtr&& pResolver, bool clearFirst = false) noexcept
	{
		// Create the binding
		auto pAction = GetAction(actionName);

		if (clearFirst)
			pAction->Bind(nullptr);

		if (!pAction->Bind(Epic::FirstAvailableSlot, std::move(pResolver)))
			return false;

		// Add the binding to the context
		auto pContext = GetContext(contextName);
		if (pContext->AddAction(actionName))
		{
			// Iterators for this context may have been invalidated
			m_SafeToIterateBindings = false;
		}

		return true;
	}

	// Create a binding between an Action and a resolver for a context.
	// If 'clearFirst' is true, all other bindings for this Action will be cleared first.
	// Returns whether or not the Bind was successful.
	bool Bind(const Epic::StringHash actionName, const Epic::StringHash contextName, InputAction::Slot slot, InputResolverPtr&& pResolver, bool clearFirst = false) noexcept
	{
		// Create the binding
		auto pAction = GetAction(actionName);

		if (clearFirst)
			pAction->Bind(nullptr);

		if (!pAction->Bind(slot, std::move(pResolver)))
			return false;

		// Add the binding to the context
		auto pContext = GetContext(contextName);
		if (pContext->AddAction(actionName))
		{
			// Iterators for this context may have been invalidated
			m_SafeToIterateBindings = false;
		}

		return true;
	}

	// Create a binding between an Action and a resolver for the global Context.
	// If 'clearFirst' is true, all other bindings for this Action will be cleared first.
	// Returns whether or not the Bind was successful.
	inline bool Bind(const Epic::StringHash actionName, InputAction::Slot slot, InputResolverPtr&& pResolver, bool clearFirst = false) noexcept
	{
		return Bind(actionName, GlobalContext, slot, std::move(pResolver), clearFirst);
	}

	// Create a binding between an Action and a resolver for the global Context.
	// If 'clearFirst' is true, all other bindings for this Action will be cleared first.
	// The binding will occur in the first available slot.
	// Returns whether or not the Bind was successful.
	inline bool Bind(const Epic::StringHash actionName, InputResolverPtr&& pResolver, bool clearFirst = false) noexcept
	{
		return Bind(actionName, GlobalContext, std::move(pResolver), clearFirst);
	}
};
