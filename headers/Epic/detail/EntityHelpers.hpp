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

//////////////////////////////////////////////////////////////////////////////

namespace Epic::detail
{
	template<class... Components>
	struct EntityHasComponents;
}

//////////////////////////////////////////////////////////////////////////////

template<class... Components>
struct Epic::detail::EntityHasComponents
{
	inline static bool Apply(Epic::Entity* pEntity) noexcept
	{
		return pEntity->Has<Components...>();
	}
};

template<>
struct Epic::detail::EntityHasComponents<void>
{
	inline static bool Apply(Epic::Entity*) noexcept
	{
		return true;
	}
};
