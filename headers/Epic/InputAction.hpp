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
#include <Epic/InputData.hpp>
#include <Epic/InputResolver.hpp>
#include <Epic/StringHash.hpp>
#include <Epic/STL/UniquePtr.hpp>
#include <array>
#include <cassert>

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	class InputAction;

	enum FirstAvailableSlotTag { FirstAvailableSlot };
}

//////////////////////////////////////////////////////////////////////////////

// InputAction
class Epic::InputAction
{
public:
	using Type = Epic::InputAction;

public:
	using Slot = size_t;
	static constexpr Slot Slots = 4;
	using InputResolverPtr = Epic::ImplPtr<InputResolver>;

private:
	using ResolverList = std::array<InputResolverPtr, Slots>;
	using ActionDelegate = Epic::Event<bool(const Epic::InputData&)>;

private:
	Epic::StringHash m_ActionName;
	ResolverList m_Resolvers;

public:
	ActionDelegate Action;

public:
	InputAction() = delete;
	InputAction(const Type&) = delete;
	Type& operator = (const Type&) = delete;
	
	inline explicit InputAction(const Epic::StringHash& actionName) noexcept
		: m_ActionName{ actionName }
	{ }

	inline InputAction(Type&& other) noexcept
		: m_ActionName{ std::move(other.m_ActionName) }, 
		  m_Resolvers{ std::move(other.m_Resolvers) },
		  Action{ std::move(other.Action) }
	{ }
	
public:
	inline const Epic::StringHash& GetActionName() const noexcept
	{
		return m_ActionName;
	}

public:
	inline auto begin() const
	{
		return std::begin(m_Resolvers);
	}

	inline auto end() const
	{
		return std::end(m_Resolvers);
	}

public:
	inline Slot GetAvailableSlots() const noexcept
	{
		Slot n = 0;

		for (Slot s = 0; s < Slots; ++s)
			if (IsAvailable(s)) ++n;

		return n;
	}

	inline Slot GetUsedSlots() const noexcept
	{
		Slot n = 0;

		for (Slot s = 0; s < Slots; ++s)
			if (IsUsed(s)) ++n;

		return n;
	}

	inline bool IsUsed(Slot slot) const noexcept
	{
		return m_Resolvers[slot] != nullptr;
	}

	inline bool IsAvailable(Slot slot) const noexcept
	{
		return m_Resolvers[slot] == nullptr;
	}

public:
	// Rebind 'slot' to 'pResolver'
	inline bool Bind(Slot slot, InputResolverPtr pResolver) noexcept
	{
		assert(slot < Slots);	
		m_Resolvers[slot] = std::move(pResolver);
		return true;
	}

	// Bind the first available slot to 'pResolver'
	bool Bind(FirstAvailableSlotTag, InputResolverPtr pResolver) noexcept
	{
		for (Slot s = 0; s < Slots; ++s)
		{
			if (IsAvailable(s))
			{
				m_Resolvers[s] = std::move(pResolver);
				return true;
			}
		}

		return false;
	}

	// Remove the binding at 'slot'
	inline bool Bind(Slot slot, nullptr_t) noexcept
	{
		assert(slot < Slots);
		m_Resolvers[slot] = nullptr;
		return true;
	}

	// Remove all bindings
	inline bool Bind(nullptr_t) noexcept
	{
		for (auto& p : m_Resolvers)
			p = nullptr;

		return true;
	}

public:
	inline Epic::InputResolver* GetResolver(Slot slot) const noexcept
	{
		assert(slot < Slots);
		return m_Resolvers[slot].get();
	}
};
