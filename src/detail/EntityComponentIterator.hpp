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

#include <Epic/Entity.hpp>
#include <Epic/detail/EntityManagerFwd.hpp>
#include <Epic/detail/EntityHelpers.hpp>

//////////////////////////////////////////////////////////////////////////////

namespace Epic::detail
{
	template<class... Components>
	struct EntityHasComponents;

	template<class... Components>
	class EntityComponentIterator;
}

//////////////////////////////////////////////////////////////////////////////

// EntityComponentIterator<Components>
template<class... Components>
class Epic::detail::EntityComponentIterator
{
public:
	using Type = Epic::detail::EntityComponentIterator<Components...>;
	using ValueType = Epic::Entity*;

private:
	size_t m_Index;
	Epic::EntityManager* m_pSystem;
	bool m_AtEnd;
	bool m_IncludeDestroyed;

public:
	EntityComponentIterator(Epic::EntityManager* pSystem, size_t index, bool includeDestroyed) noexcept
		: m_pSystem{ pSystem }, m_Index{ index }, m_AtEnd{ false }, m_IncludeDestroyed{ includeDestroyed }
	{
		if (m_Index >= m_pSystem->GetEntityCount())
			m_AtEnd = true;
	}

public:
	inline ValueType operator*() const noexcept
	{
		return Get();
	}

	bool operator== (const Type& other) const noexcept
	{
		if (m_pSystem != other.m_pSystem)
			return false;

		if (AtEnd())
			return other.AtEnd();

		return m_Index == other.m_Index;
	}

	inline bool operator!= (const Type& other) const noexcept
	{
		return !(this->operator== (other));
	}

	Type& operator++ () noexcept
	{
		++m_Index;
		ValueType pValue = nullptr;

		while (m_Index < m_pSystem->GetEntityCount() &&
			(pValue = Get()) &&
			(!detail::EntityHasComponents<Components...>::Apply(pValue) ||
			(pValue->IsDestroyPending() && !m_IncludeDestroyed)
				))
			++m_Index;

		if (m_Index >= m_pSystem->GetEntityCount())
		{
			m_AtEnd = true;
			m_Index = m_pSystem->GetEntityCount();
		}

		return *this;
	}

public:
	inline ValueType Get() const noexcept
	{
		return AtEnd() ? nullptr : m_pSystem->GetEntityByIndex(m_Index);
	}

public:
	inline bool AtEnd() const noexcept
	{
		return m_AtEnd || m_Index >= m_pSystem->GetEntityCount();
	}

	constexpr bool IncludeDestroyed() const noexcept
	{
		return m_IncludeDestroyed;
	}

	constexpr size_t GetIndex() const noexcept
	{
		return m_Index;
	}

	constexpr Epic::EntityManager* GetEntityManager() const noexcept
	{
		return m_pSystem;
	}
};
