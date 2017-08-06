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
	VectorSwizzler() noexcept = default;

private:
	using AssignableThis =
		std::conditional_t<
			Epic::TMP::IsSequenceUnique<Epic::TMP::Sequence<size_t, Indices...>>::value,
			Type&,
			struct UnassignableLValue>;

public:
	inline auto operator() () const noexcept
	{
		return ToVector();
	}

	inline VectorType ToVector() const noexcept
	{
		VectorType result;

		Epic::TMP::ForEach2<
			Epic::TMP::MakeSequence<size_t, sizeof...(Indices)>,
			Epic::TMP::Sequence<size_t, Indices...>>
		::Apply([&](size_t dest, size_t src) 
		{ 
			result[dest] = m_Values[src]; 
		});

		return result;
	}

public:
	#pragma region Assignment Operators
	#define CREATE_ASSIGNMENT_OPERATOR(Op)	\
																									\
	template<class T>																				\
	inline AssignableThis operator Op (const T(&values)[sizeof...(Indices)]) noexcept				\
	{																								\
		Epic::TMP::ForEach2<																		\
			Epic::TMP::Sequence<size_t, Indices...>,												\
			Epic::TMP::MakeSequence<size_t, sizeof...(Indices)>>									\
			::Apply([&](size_t iThis, size_t iOther)												\
		{																							\
				m_Values[iThis] Op values[iOther];													\
		});																							\
																									\
		return *this;																				\
	}																								\
																									\
	template<class T>																				\
	inline AssignableThis operator Op (const Epic::Vector<T, sizeof...(Indices)>& vec) noexcept		\
	{																								\
		Epic::TMP::ForEach2<																		\
			Epic::TMP::Sequence<size_t, Indices...>,												\
			Epic::TMP::MakeSequence<size_t, sizeof...(Indices)>>									\
		::Apply([&](size_t iThis, size_t iOther) { m_Values[iThis] Op vec[iOther]; });				\
																									\
		return *this;																				\
	}																								\
																									\
	inline AssignableThis operator Op (const Type& other) noexcept									\
	{																								\
		ForEach([&](size_t index) { m_Values[index] Op other.m_Values[index]; });					\
		return *this;																				\
	}																								\
																									\
	template<class VT, class TArray2, size_t... Is>													\
	inline AssignableThis operator Op (const VectorSwizzler<VT, TArray2, Is...>& vec) noexcept		\
	{																								\
		if (static_cast<const void*>(this) == static_cast<const void*>(&vec))						\
			return *this Op vec.ToVector();															\
																									\
		Epic::TMP::ForEach2<																		\
			Epic::TMP::Sequence<size_t, Indices...>,												\
			Epic::TMP::Sequence<size_t, Is...>>														\
		::Apply([&](size_t iThis, size_t iOther)													\
		{																							\
			m_Values[iThis] Op vec.m_Values[iOther];												\
		});																							\
																									\
		return *this;																				\
	}																								\
																									\
	inline AssignableThis operator Op (const ScalarType& value) noexcept							\
	{																								\
		ForEach([&](size_t index) { m_Values[index] Op value; });									\
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
	template<class U>																			\
	inline auto operator Op (const U(&values)[sizeof...(Indices)]) const noexcept				\
	{																							\
		auto result = ToVector();																\
		result Op= values;																		\
		return result;																			\
	}																							\
																								\
	template<class U>																			\
	inline auto operator Op (const Epic::Vector<U, sizeof...(Indices)>& vec) const noexcept		\
	{																							\
		auto result = ToVector();																\
		result Op= vec;																			\
		return result;																			\
	}																							\
																								\
	inline auto operator Op (const Type& vec) const noexcept									\
	{																							\
		auto result = ToVector();																\
		result Op= vec;																			\
		return result;																			\
	}																							\
																								\
	template<class VT, class TArray2, size_t... Is>												\
	inline auto operator Op (const VectorSwizzler<VT, TArray2, Is...>& vec) const noexcept		\
	{																							\
		auto result = ToVector();																\
		result Op= vec;																			\
		return result;																			\
	}																							\
																								\
	inline auto operator Op (const ScalarType& value) const noexcept							\
	{																							\
		auto result = ToVector();																\
		result Op= value;																		\
		return result;																			\
	}																							\
																								\
	friend inline auto operator Op (const ScalarType& value, const Type& vec) noexcept			\
	{																							\
		auto result = vec.ToVector();															\
		result Op= value;																		\
		return result;																			\
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
	template<class Function>
	inline void ForEach(Function fn) noexcept
	{
		Epic::TMP::ForEach<Epic::TMP::Sequence<size_t, Indices...>>::Apply(fn);
	}

	template<class Function>
	inline void ForEach(Function fn) const noexcept
	{
		Epic::TMP::ForEach<Epic::TMP::Sequence<size_t, Indices...>>::Apply(fn);
	}

private:
	template<class VectorType2, class TArray2, size_t... Indices2>
	friend class VectorSwizzler;

	template<class T, size_t Size>
	friend class Vector;
};
