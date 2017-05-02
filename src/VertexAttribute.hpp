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
	template<class AttributeTag>
	class VertexAttribute;
}

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	enum class eComponentType
	{
		Byte, UByte, 
		Short, UShort,
		Int, UInt,
		HalfFloat, Float, Double, Fixed,
		Int_2_10_10_10, UInt_2_10_10_10, 
		UInt_10F_11F_11F
	};
}

//////////////////////////////////////////////////////////////////////////////

#define MAKE_VERTEX_ATTRIBUTE_TAG(TAG)									\
	struct TAG;

#define MAKE_VERTEX_ATTRIBUTE(TAG, DTYPE, DNAME, CMPS, EDT, NORM)		\
	template<> class Epic::VertexAttribute<TAG>							\
	{																	\
	protected:															\
		using ValueType = DTYPE;										\
		static constexpr size_t Components = CMPS;						\
		static constexpr Epic::eComponentType DataType = EDT;			\
		static constexpr bool Normalize = NORM;							\
																		\
	public:																\
		VertexAttribute() = default;									\
		constexpr VertexAttribute(const ValueType& v)					\
			: DNAME(v) { }												\
		constexpr VertexAttribute(ValueType&& v)						\
			: DNAME(std::move(v)) { }									\
																		\
	public:																\
		ValueType DNAME;												\
																		\
	protected:															\
		constexpr static const char* GetSemantic() noexcept				\
		{ return #DNAME; }												\
																		\
		ValueType& Value() noexcept										\
		{ return this->DNAME; }											\
																		\
		constexpr const ValueType& Value() const noexcept				\
		{ return this->DNAME; }											\
	};
