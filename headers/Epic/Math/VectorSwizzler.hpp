//////////////////////////////////////////////////////////////////////////////
//
//            Copyright (c) 2016 Ronnie Brohn (EpicBrownie)      
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

#include <Epic/Math/detail/VectorSwizzlerFwd.hpp>
#include <Epic/Math/detail/VectorFwd.hpp>
#include <Epic/TMP/Sequence.hpp>
#include <algorithm>
#include <type_traits>

//////////////////////////////////////////////////////////////////////////////

// VectorSwizzler
template<class VectorType, class TArray, size_t... Indices>
class Epic::VectorSwizzler
{
public:
	using Type = Epic::VectorSwizzler<VectorType, TArray, Indices...>;

private:
	using ScalarType = typename TArray::value_type;

	TArray m_Values;

public:
	VectorSwizzler() = default;

private:
	using AssignableThis =
		std::conditional_t<
			Epic::TMP::IsSequenceUnique<Epic::TMP::Sequence<size_t, Indices...>>::value,
			Type&,
			struct UnassignableLValue>;

public:
	VectorType ToVector() const
	{
		VectorType result;

		Epic::TMP::ForEach2<
			Epic::TMP::MakeSequence<size_t, sizeof...(Indices)>,
			Epic::TMP::Sequence<size_t, Indices...>
		>::Apply([&](size_t dest, size_t src) { result[dest] = m_Values[src]; });

		return result;
	}

public:
	#pragma region Assignment Operators
	#define CREATE_ASSIGNMENT_OPERATOR(Op)	\
																									\
	AssignableThis operator Op (std::initializer_list<ScalarType> values)							\
	{																								\
		auto it = std::begin(values);																\
																									\
		Epic::TMP::ForEach<Epic::TMP::Sequence<size_t, Indices...>>::Apply(							\
			[&](size_t index)																		\
			{																						\
				if (it != std::end(values))															\
					m_Values[index] Op *it++;														\
			});																						\
																									\
		return *this;																				\
	}																								\
																									\
	template<class T, size_t Size>																	\
	AssignableThis operator Op (const Epic::Vector<T, Size>& vec)									\
	{																								\
		Epic::TMP::ForEach2<																		\
			Epic::TMP::Sequence<size_t, Indices...>,												\
			Epic::TMP::MakeSequence<size_t, Size>>													\
		::Apply([&](size_t iThis, size_t iOther) { m_Values[iThis] Op vec[iOther]; });				\
																									\
		return *this;																				\
	}																								\
																									\
	AssignableThis operator Op (const Type& other)													\
	{																								\
		Epic::TMP::ForEach<Epic::TMP::Sequence<size_t, Indices...>>									\
			::Apply([&](size_t index) { m_Values[index] Op other.m_Values[index]; });				\
																									\
		return *this;																				\
	}																								\
																									\
	template<class VectorType2, class TArray2, size_t... Indices2>									\
	AssignableThis operator Op (const VectorSwizzler<VectorType2, TArray2, Indices2...>& other)		\
	{																								\
		if (static_cast<const void*>(this) == static_cast<const void*>(&other))						\
			return *this Op other.ToVector();														\
																									\
		Epic::TMP::ForEach2<																		\
			Epic::TMP::Sequence<size_t, Indices...>,												\
			Epic::TMP::Sequence<size_t, Indices2...>>												\
		::Apply([&](size_t iThis, size_t iOther) { m_Values[iThis] Op other.m_Values[iOther]; });	\
																									\
		return *this;																				\
	}																								\
																									\
	AssignableThis operator Op (const ScalarType& value)											\
	{																								\
		Epic::TMP::ForEach<Epic::TMP::Sequence<size_t, Indices...>>									\
			::Apply([&](size_t index) { m_Values[index] Op value; });								\
																									\
		return *this;																				\
	}

	CREATE_ASSIGNMENT_OPERATOR(= );
	CREATE_ASSIGNMENT_OPERATOR(+= );
	CREATE_ASSIGNMENT_OPERATOR(-= );
	CREATE_ASSIGNMENT_OPERATOR(*= );
	CREATE_ASSIGNMENT_OPERATOR(/= );

	// The following assignment operators will fail for non-integral types
	CREATE_ASSIGNMENT_OPERATOR(|= );
	CREATE_ASSIGNMENT_OPERATOR(&= );
	CREATE_ASSIGNMENT_OPERATOR(^= );
	CREATE_ASSIGNMENT_OPERATOR(%= );
	CREATE_ASSIGNMENT_OPERATOR(<<= );
	CREATE_ASSIGNMENT_OPERATOR(>>= );

	#undef CREATE_ASSIGNMENT_OPERATOR
	#pragma endregion

public:
	#pragma region Arithmetic Operators
	#define CREATE_ARITHMETIC_OPERATOR(Op) 	\
																							\
	template<class U>																		\
	inline auto operator Op (std::initializer_list<U> values)								\
	{																						\
		auto result = ToVector();															\
		result Op= values;																	\
		return result;																		\
	}																						\
																							\
	template<class U>																		\
	inline auto operator Op (const Epic::Vector<U, sizeof...(Indices)>& vec)				\
	{																						\
		auto result = ToVector();															\
		result Op= vec;																		\
		return result;																		\
	}																						\
																							\
	inline auto operator Op (const Type& vec)												\
	{																						\
		auto result = ToVector();															\
		result Op= vec;																		\
		return result;																		\
	}																						\
																							\
	template<class VectorType2, class TArray2, size_t... Indices2>							\
	inline auto operator Op (const VectorSwizzler<VectorType2, TArray2, Indices2...>& vec)	\
	{																						\
		auto result = ToVector();															\
		result Op= vec;																		\
		return result;																		\
	}																						\
																							\
	inline auto operator Op (const ScalarType& value)										\
	{																						\
		auto result = ToVector();															\
		result Op= value;																	\
		return result;																		\
	}																						\
																							\
	friend auto operator Op (const ScalarType& value, const Type& vec)						\
	{																						\
		auto result = vec.ToVector();														\
		result Op= value;																	\
		return result;																		\
	}
	
	CREATE_ARITHMETIC_OPERATOR(+);
	CREATE_ARITHMETIC_OPERATOR(-);
	CREATE_ARITHMETIC_OPERATOR(*);
	CREATE_ARITHMETIC_OPERATOR(/);

	// The following arithmetic operators are only defined for integral types
	CREATE_ARITHMETIC_OPERATOR(|);
	CREATE_ARITHMETIC_OPERATOR(&);
	CREATE_ARITHMETIC_OPERATOR(^);
	CREATE_ARITHMETIC_OPERATOR(%);
	CREATE_ARITHMETIC_OPERATOR(<<);
	CREATE_ARITHMETIC_OPERATOR(>>);

	#undef CREATE_ARITHMETIC_OPERATOR
	#pragma endregion

private:
	template<class VectorType2, class TArray2, size_t... Indices2>
	friend class VectorSwizzler;

	template<class T, size_t Size>
	friend class Vector;
};
