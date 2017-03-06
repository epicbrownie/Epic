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

#include <type_traits>

//////////////////////////////////////////////////////////////////////////////

namespace Epic::Component
{
	template<class ComponentTag>
	struct VertexComponent;
}

//////////////////////////////////////////////////////////////////////////////

#define MAKE_VERTEX_COMPONENT(TAG, DTYPE, DNAME, CMPS)					\
																		\
	namespace Epic::Component											\
	{																	\
		struct TAG;														\
																		\
		template<> struct VertexComponent<TAG>							\
		{																\
			static constexpr size_t Components = CMPS;					\
																		\
			using ValueType = DTYPE;									\
			ValueType DNAME;											\
																		\
			constexpr static const char* GetName() noexcept				\
			{ return #DNAME; }											\
																		\
			ValueType& Value() noexcept									\
			{ return this->DNAME; }										\
																		\
			constexpr const ValueType& Value() const noexcept			\
			{ return this->DNAME; }										\
		};																\
	}
