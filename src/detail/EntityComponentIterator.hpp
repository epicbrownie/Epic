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
	class EntityComponentIteratorBase;

	template<class... Components>
	class EntityComponentIterator;

	class ConstEntityComponentIteratorBase;

	template<class... Components>
	class ConstEntityComponentIterator;
}

//////////////////////////////////////////////////////////////////////////////

// EntityComponentIteratorBase
class Epic::detail::EntityComponentIteratorBase
{
public:
	using Type = Epic::detail::EntityComponentIteratorBase;
	using ValueType = Epic::Entity*;

protected:
	size_t m_Index;
	Epic::EntityManager* m_pManager;
	bool m_AtEnd;
	bool m_IncludeDestroyed;

public:
	EntityComponentIteratorBase(Epic::EntityManager* pManager, size_t index, bool includeDestroyed) noexcept
		: m_pManager{ pManager }, m_Index{ index }, m_AtEnd{ true }, m_IncludeDestroyed{ includeDestroyed }
	{ }

public:
	bool IncludeDestroyed() const noexcept
	{
		return m_IncludeDestroyed;
	}

	size_t GetIndex() const noexcept
	{
		return m_Index;
	}

	const Epic::EntityManager* GetEntityManager() const noexcept
	{
		return m_pManager;
	}
};

// EntityComponentIterator<Components>
template<class... Components>
class Epic::detail::EntityComponentIterator : public Epic::detail::EntityComponentIteratorBase
{
public:
	using Type = Epic::detail::EntityComponentIterator<Components...>;
	
private:
	using Base = Epic::detail::EntityComponentIteratorBase;

public:
	EntityComponentIterator(Epic::EntityManager* pManager, size_t index, bool includeDestroyed) noexcept
		: Base(pManager, index, includeDestroyed)
	{
		m_AtEnd = (m_Index >= m_pManager->GetEntityCount());
	}

public:
	inline ValueType operator*() const noexcept
	{
		return Get();
	}

	bool operator== (const Type& other) const noexcept
	{
		if (m_pManager != other.m_pManager)
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

		while (m_Index < m_pManager->GetEntityCount() &&
			(pValue = Get()) &&
			(!detail::EntityHasComponents<Components...>::Apply(pValue) ||
			(pValue->IsDestroyPending() && !m_IncludeDestroyed)
				))
			++m_Index;

		if (m_Index >= m_pManager->GetEntityCount())
		{
			m_AtEnd = true;
			m_Index = m_pManager->GetEntityCount();
		}

		return *this;
	}

public:
	inline ValueType Get() const noexcept
	{
		return AtEnd() ? nullptr : m_pManager->GetEntityByIndex(m_Index);
	}

	inline bool AtEnd() const noexcept
	{
		return m_AtEnd || m_Index >= m_pManager->GetEntityCount();
	}
};

//////////////////////////////////////////////////////////////////////////////

// ConstEntityComponentIteratorBase
class Epic::detail::ConstEntityComponentIteratorBase
{
public:
	using Type = Epic::detail::ConstEntityComponentIteratorBase;
	using ValueType = const Epic::Entity*;

protected:
	size_t m_Index;
	const Epic::EntityManager* m_pManager;
	bool m_AtEnd;
	bool m_IncludeDestroyed;

public:
	ConstEntityComponentIteratorBase(const Epic::EntityManager* pManager, size_t index, bool includeDestroyed) noexcept
		: m_pManager{ pManager }, m_Index{ index }, m_AtEnd{ true }, m_IncludeDestroyed{ includeDestroyed }
	{ }

public:
	bool IncludeDestroyed() const noexcept
	{
		return m_IncludeDestroyed;
	}

	size_t GetIndex() const noexcept
	{
		return m_Index;
	}

	const Epic::EntityManager* GetEntityManager() const noexcept
	{
		return m_pManager;
	}
};

// ConstEntityComponentIterator<Components>
template<class... Components>
class Epic::detail::ConstEntityComponentIterator : public Epic::detail::ConstEntityComponentIteratorBase
{
public:
	using Type = Epic::detail::ConstEntityComponentIterator<Components...>;
	
private:
	using Base = Epic::detail::ConstEntityComponentIteratorBase;

public:
	ConstEntityComponentIterator(const Epic::EntityManager* pManager, size_t index, bool includeDestroyed) noexcept
		: Base(pManager, index, includeDestroyed)
	{
		m_AtEnd = (m_Index >= m_pManager->GetEntityCount());
	}

public:
	inline ValueType operator*() const noexcept
	{
		return Get();
	}

	bool operator== (const Type& other) const noexcept
	{
		if (m_pManager != other.m_pManager)
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

		while (m_Index < m_pManager->GetEntityCount() &&
			(pValue = Get()) &&
			(!detail::EntityHasComponents<Components...>::Apply(pValue) ||
			(pValue->IsDestroyPending() && !m_IncludeDestroyed)
				))
			++m_Index;

		if (m_Index >= m_pManager->GetEntityCount())
		{
			m_AtEnd = true;
			m_Index = m_pManager->GetEntityCount();
		}

		return *this;
	}

public:
	inline ValueType Get() const noexcept
	{
		return AtEnd() ? nullptr : m_pManager->GetEntityByIndex(m_Index);
	}

	inline bool AtEnd() const noexcept
	{
		return m_AtEnd || m_Index >= m_pManager->GetEntityCount();
	}
};
