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

namespace Epic
{
	template<class ComponentTag>
	class VertexComponent;
}

//////////////////////////////////////////////////////////////////////////////

#define MAKE_VERTEX_COMPONENT_TAG(TAG)								\
	struct TAG;
	
#define MAKE_VERTEX_COMPONENT(TAG, DTYPE, DNAME, CMPS)				\
	template<> class Epic::VertexComponent<TAG>						\
	{																\
	protected:														\
		using ValueType = DTYPE;									\
		static constexpr size_t Components = CMPS;					\
																	\
	public:															\
		ValueType DNAME;											\
																	\
	protected:														\
		constexpr static const char* GetName() noexcept				\
		{ return #DNAME; }											\
																	\
		ValueType& Value() noexcept									\
		{ return this->DNAME; }										\
																	\
		constexpr const ValueType& Value() const noexcept			\
		{ return this->DNAME; }										\
	};
