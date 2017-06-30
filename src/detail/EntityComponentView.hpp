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
#include <Epic/detail/EntityComponentIterator.hpp>
#include <Epic/detail/EntityHelpers.hpp>

//////////////////////////////////////////////////////////////////////////////

namespace Epic::detail
{
	template<class Iterator, class... Components>
	class EntityComponentViewImpl;

	template<class... Components>
	using EntityComponentView = EntityComponentViewImpl<Epic::detail::EntityComponentIterator<Components...>, Components...>;

	template<class... Components>
	using ConstEntityComponentView = EntityComponentViewImpl<Epic::detail::ConstEntityComponentIterator<Components...>, Components...>;
}

//////////////////////////////////////////////////////////////////////////////

// EntityComponentViewImpl<Iterator, Components>
template<class I, class... Components>
class Epic::detail::EntityComponentViewImpl
{
public:
	using Type = Epic::detail::EntityComponentViewImpl<I, Components...>;
	using Iterator = I;

private:
	Iterator m_IterBegin;
	const Iterator m_IterEnd;

public:
	EntityComponentViewImpl(const Iterator& itBegin, const Iterator& itEnd) noexcept
		: m_IterBegin{ itBegin }, m_IterEnd{ itEnd }
	{
		if (*m_IterBegin == nullptr ||
			((*m_IterBegin)->IsDestroyPending() && !m_IterBegin.IncludeDestroyed()) ||
			!detail::EntityHasComponents<Components...>::Apply(*m_IterBegin) )
		{
			++m_IterBegin;
		}
	}

public:
	inline auto begin() noexcept
	{
		return m_IterBegin;
	}

	inline auto end() noexcept
	{
		return m_IterEnd;
	}
};
