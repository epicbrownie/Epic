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

//////////////////////////////////////////////////////////////////////////////

namespace Epic::detail
{
	using EntityComponentID = Epic::StringHash::HashType;

	template<class T>
	struct EntityComponentTraits;
}

//////////////////////////////////////////////////////////////////////////////

#define MAKE_ENTITY_COMPONENT(type)	 \
	template<>																	\
	struct Epic::detail::EntityComponentTraits<type>							\
	{																			\
		static constexpr auto ID = Epic::detail::MakeEntityComponentID(#type);	\
	};

//////////////////////////////////////////////////////////////////////////////

// MakeEntityComponentID()
namespace Epic::detail
{
	namespace
	{
		template<size_t N>
		constexpr EntityComponentID MakeEntityComponentID(const char(&cstr)[N])
		{
			return Epic::Hash(cstr).Value();
		}
	}
}

//////////////////////////////////////////////////////////////////////////////

template<class T>
struct Epic::detail::EntityComponentTraits
{
	static_assert(!std::is_same<T, T>::value, 
		"EntityComponentTraits<T> has not been defined."
		"Use MAKE_ENTITY_COMPONENT(T) to define the traits for this entity component.");
};
