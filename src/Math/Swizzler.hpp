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

#include <Epic/Math/detail/SwizzlerFwd.hpp>
#include <Epic/Math/detail/VectorFwd.hpp>
#include <Epic/Math/detail/VectorHelpers.hpp>
#include <Epic/TMP/Sequence.hpp>
#include <algorithm>
#include <type_traits>

//////////////////////////////////////////////////////////////////////////////

// Swizzler
template<class T, size_t VS, size_t... Indices>
class Epic::Swizzler
{
public:
	using Type = Epic::Swizzler<T, VS, Indices...>;

public:
	static constexpr size_t Size = sizeof...(Indices);

	using ScalarType = T;
	using VectorType = Epic::Vector<T, sizeof...(Indices)>;

private:
	static constexpr size_t ParentVectorSize = VS;
	
	static constexpr bool IsAssignableLValue = 
		TMP::IsSequenceUnique<TMP::Sequence<size_t, Indices...>>::value;
		
	using TArray = typename detail::SVectorHelper<ScalarType, ParentVectorSize>::TArray;
	using IndexSeq = TMP::Sequence<size_t, Indices...>;
	using VectorSeq = TMP::MakeSequence<size_t, Size>;

private:
	TArray m_Values;

public:
	Swizzler() noexcept = default;

public:
	VectorType ToVector() const noexcept
	{
		VectorType result;

		TMP::ForEach2<VectorSeq, IndexSeq>::Apply([&] (size_t dest, size_t src) {
			result[dest] = m_Values[src];
		});

		return result;
	}

public:
	auto operator() () const noexcept -> decltype(ToVector())
	{
		return ToVector();
	}

public:
	#pragma region Assignment Operators

	Type& operator = (const T& value) noexcept
	{
		if constexpr (!IsAssignableLValue)
			static_assert(false, "All swizzled indices must be unique to be used as an L-value.");
		
		TMP::ForEach<IndexSeq>::Apply([&] (size_t i) {
			m_Values[i] = value; 
		});

		return *this;
	}

	Type& operator += (const T& value) noexcept
	{
		if constexpr (!IsAssignableLValue)
			static_assert(false, "All swizzled indices must be unique to be used as an L-value.");

		TMP::ForEach<IndexSeq>::Apply([&] (size_t i) { 
			m_Values[i] += value; 
		});

		return *this;
	}

	Type& operator -= (const T& value) noexcept
	{
		if constexpr (!IsAssignableLValue)
			static_assert(false, "All swizzled indices must be unique to be used as an L-value.");

		TMP::ForEach<IndexSeq>::Apply([&] (size_t i) { 
			m_Values[i] -= value; 
		});

		return *this;
	}

	Type& operator *= (const T& value) noexcept
	{
		if constexpr (!IsAssignableLValue)
			static_assert(false, "All swizzled indices must be unique to be used as an L-value.");

		TMP::ForEach<IndexSeq>::Apply([&] (size_t i) { 
			m_Values[i] *= value; 
		});

		return *this;
	}

	Type& operator /= (const T& value) noexcept
	{
		if constexpr (!IsAssignableLValue)
			static_assert(false, "All swizzled indices must be unique to be used as an L-value.");

		TMP::ForEach<IndexSeq>::Apply([&] (size_t i) { 
			m_Values[i] /= value;
		});

		return *this;
	}

	Type& operator = (const Type& other) noexcept
	{
		if constexpr (!IsAssignableLValue)
			static_assert(false, "All swizzled indices must be unique to be used as an L-value.");

		TMP::ForEach<IndexSeq>::Apply([&] (size_t i) { 
			m_Values[i] = other.m_Values[i];
		});

		return *this;
	}

	Type& operator += (const Type& other) noexcept
	{
		if constexpr (!IsAssignableLValue)
			static_assert(false, "All swizzled indices must be unique to be used as an L-value.");

		TMP::ForEach<IndexSeq>::Apply([&] (size_t i) { 
			m_Values[i] += other.m_Values[i]; 
		});

		return *this;
	}

	Type& operator -= (const Type& other) noexcept
	{
		if constexpr (!IsAssignableLValue)
			static_assert(false, "All swizzled indices must be unique to be used as an L-value.");

		TMP::ForEach<IndexSeq>::Apply([&] (size_t i) {
			m_Values[i] -= other.m_Values[i]; 
		});

		return *this;
	}

	Type& operator *= (const Type& other) noexcept
	{
		if constexpr (!IsAssignableLValue)
			static_assert(false, "All swizzled indices must be unique to be used as an L-value.");

		TMP::ForEach<IndexSeq>::Apply([&] (size_t i) {
			m_Values[i] *= other.m_Values[i];
		});

		return *this;
	}

	Type& operator /= (const Type& other) noexcept
	{
		if constexpr (!IsAssignableLValue)
			static_assert(false, "All swizzled indices must be unique to be used as an L-value.");

		TMP::ForEach<IndexSeq>::Apply([&] (size_t i) {
			m_Values[i] /= other.m_Values[i];
		});

		return *this;
	}

	Type& operator = (const T(&values)[Size]) noexcept
	{
		if constexpr (!IsAssignableLValue)
			static_assert(false, "All swizzled indices must be unique to be used as an L-value.");

		TMP::ForEach2<VectorSeq, IndexSeq>::Apply([&] (size_t i, size_t j) {
			m_Values[i] = values[j]; 
		});

		return *this;
	}

	Type& operator += (const T(&values)[Size]) noexcept
	{
		if constexpr (!IsAssignableLValue)
			static_assert(false, "All swizzled indices must be unique to be used as an L-value.");

		TMP::ForEach2<VectorSeq, IndexSeq>::Apply([&] (size_t i, size_t j) {
			m_Values[i] += values[j]; 
		});

		return *this;
	}

	Type& operator -= (const T(&values)[Size]) noexcept
	{
		if constexpr (!IsAssignableLValue)
			static_assert(false, "All swizzled indices must be unique to be used as an L-value.");

		TMP::ForEach2<VectorSeq, IndexSeq>::Apply([&] (size_t i, size_t j) {
			m_Values[i] -= values[j]; 
		});

		return *this;
	}

	Type& operator *= (const T(&values)[Size]) noexcept
	{
		if constexpr (!IsAssignableLValue)
			static_assert(false, "All swizzled indices must be unique to be used as an L-value.");

		TMP::ForEach2<VectorSeq, IndexSeq>::Apply([&] (size_t i, size_t j) {
			m_Values[i] *= values[j]; 
		});

		return *this;
	}

	Type& operator /= (const T(&values)[Size]) noexcept
	{
		if constexpr (!IsAssignableLValue)
			static_assert(false, "All swizzled indices must be unique to be used as an L-value.");

		TMP::ForEach2<VectorSeq, IndexSeq>::Apply([&] (size_t i, size_t j) {
			m_Values[i] /= values[j]; 
		});

		return *this;
	}

	Type& operator = (const VectorType& vec) noexcept
	{
		if constexpr (!IsAssignableLValue)
			static_assert(false, "All swizzled indices must be unique to be used as an L-value.");

		TMP::ForEach2<VectorSeq, IndexSeq>::Apply([&] (size_t i, size_t j) {
			m_Values[i] = vec[j]; 
		});

		return *this;
	}

	Type& operator += (const VectorType& vec) noexcept
	{
		if constexpr (!IsAssignableLValue)
			static_assert(false, "All swizzled indices must be unique to be used as an L-value.");

		TMP::ForEach2<VectorSeq, IndexSeq>::Apply([&] (size_t i, size_t j) {
			m_Values[i] += vec[j]; 
		});

		return *this;
	}

	Type& operator -= (const VectorType& vec) noexcept
	{
		if constexpr (!IsAssignableLValue)
			static_assert(false, "All swizzled indices must be unique to be used as an L-value.");

		TMP::ForEach2<VectorSeq, IndexSeq>::Apply([&] (size_t i, size_t j) {
			m_Values[i] -= vec[j]; 
		});

		return *this;
	}

	Type& operator *= (const VectorType& vec) noexcept
	{
		if constexpr (!IsAssignableLValue)
			static_assert(false, "All swizzled indices must be unique to be used as an L-value.");

		TMP::ForEach2<VectorSeq, IndexSeq>::Apply([&] (size_t i, size_t j) { 
			m_Values[i] *= vec[j]; 
		});

		return *this;
	}

	Type& operator /= (const VectorType& vec) noexcept
	{
		if constexpr (!IsAssignableLValue)
			static_assert(false, "All swizzled indices must be unique to be used as an L-value.");

		TMP::ForEach2<VectorSeq, IndexSeq>::Apply([&] (size_t i, size_t j) {
			m_Values[i] /= vec[j]; 
		});

		return *this;
	}

	template<size_t TS, size_t... Is>
	Type& operator = (const Swizzler<T, TS, Is...>& vec) noexcept
	{
		if constexpr (!IsAssignableLValue)
			static_assert(false, "All swizzled indices must be unique to be used as an L-value.");

		using IndexSeq2 = TMP::Sequence<size_t, Is...>;

		TMP::ForEach2<IndexSeq, IndexSeq2>::Apply([&] (size_t i, size_t j) {
			m_Values[i] = vec.m_Values[j];
		});

		return *this;
	}

	template<size_t TS, size_t... Is>
	Type& operator += (const Swizzler<T, TS, Is...>& vec) noexcept
	{
		if constexpr (!IsAssignableLValue)
			static_assert(false, "All swizzled indices must be unique to be used as an L-value.");

		using IndexSeq2 = TMP::Sequence<size_t, Is...>;

		TMP::ForEach2<IndexSeq, IndexSeq2>::Apply([&] (size_t i, size_t j) { 
			m_Values[i] += vec.m_Values[j]; 
		});

		return *this;
	}

	template<size_t TS, size_t... Is>
	Type& operator -= (const Swizzler<T, TS, Is...>& vec) noexcept
	{
		if constexpr (!IsAssignableLValue)
			static_assert(false, "All swizzled indices must be unique to be used as an L-value.");

		using IndexSeq2 = TMP::Sequence<size_t, Is...>;

		TMP::ForEach2<IndexSeq, IndexSeq2>::Apply([&] (size_t i, size_t j) {
			m_Values[i] -= vec.m_Values[j]; 
		});

		return *this;
	}

	template<size_t TS, size_t... Is>
	Type& operator *= (const Swizzler<T, TS, Is...>& vec) noexcept
	{
		if constexpr (!IsAssignableLValue)
			static_assert(false, "All swizzled indices must be unique to be used as an L-value.");

		using IndexSeq2 = TMP::Sequence<size_t, Is...>;

		TMP::ForEach2<IndexSeq, IndexSeq2>::Apply([&] (size_t i, size_t j) { 
			m_Values[i] *= vec.m_Values[j]; 
		});

		return *this;
	}

	template<size_t TS, size_t... Is>
	Type& operator /= (const Swizzler<T, TS, Is...>& vec) noexcept
	{
		if constexpr (!IsAssignableLValue)
			static_assert(false, "All swizzled indices must be unique to be used as an L-value.");

		using IndexSeq2 = TMP::Sequence<size_t, Is...>;

		TMP::ForEach2<IndexSeq, IndexSeq2>::Apply([&] (size_t i, size_t j) {
			m_Values[i] /= vec.m_Values[j]; 
		});

		return *this;
	}

	#pragma endregion

	#pragma region Arithmetic Operators
	
	auto operator + (const T& value) const noexcept { return ToVector() + value; }
	auto operator - (const T& value) const noexcept { return ToVector() - value; }
	auto operator * (const T& value) const noexcept { return ToVector() * value; }
	auto operator / (const T& value) const noexcept { return ToVector() / value; }

	auto operator + (const Type& vec) const noexcept { return ToVector() + vec; }
	auto operator - (const Type& vec) const noexcept { return ToVector() - vec; }
	auto operator * (const Type& vec) const noexcept { return ToVector() * vec; }
	auto operator / (const Type& vec) const noexcept { return ToVector() / vec; }
		
	auto operator + (const T(&values)[Size]) const noexcept { return ToVector() + values; }
	auto operator - (const T(&values)[Size]) const noexcept { return ToVector() - values; }
	auto operator * (const T(&values)[Size]) const noexcept { return ToVector() * values; }
	auto operator / (const T(&values)[Size]) const noexcept { return ToVector() / values; }
																	
	auto operator + (const VectorType& vec) const noexcept { return ToVector() + vec; }
	auto operator - (const VectorType& vec) const noexcept { return ToVector() - vec; }
	auto operator * (const VectorType& vec) const noexcept { return ToVector() * vec; }
	auto operator / (const VectorType& vec) const noexcept { return ToVector() / vec; }

	template<size_t TS, size_t... Is, typename = std::enable_if_t<(sizeof...(Is) == Size)>>
	auto operator + (const Swizzler<T, TS, Is...>& vec) const noexcept
	{
		return ToVector() + vec;
	}

	template<size_t TS, size_t... Is, typename = std::enable_if_t<(sizeof...(Is) == Size)>>
	auto operator - (const Swizzler<T, TS, Is...>& vec) const noexcept 
	{
		return ToVector() - vec;
	}

	template<size_t TS, size_t... Is, typename = std::enable_if_t<(sizeof...(Is) == Size)>>
	auto operator * (const Swizzler<T, TS, Is...>& vec) const noexcept
	{
		return ToVector() * vec;
	}
	
	template<size_t TS, size_t... Is, typename = std::enable_if_t<(sizeof...(Is) == Size)>>
	auto operator / (const Swizzler<T, TS, Is...>& vec) const noexcept
	{
		return ToVector() / vec;
	}

	friend VectorType operator + (const T& value, const Type& vec) noexcept
	{
		return VectorType(value) + vec;
	}

	friend VectorType operator - (const T& value, const Type& vec) noexcept
	{
		return VectorType(value) - vec;
	}

	friend VectorType operator * (const T& value, const Type& vec) noexcept
	{
		return VectorType(value) * vec;
	}

	friend VectorType operator / (const T& value, const Type& vec) noexcept
	{
		return VectorType(value) / vec;
	}

	#pragma endregion

private:
	template<class, size_t, size_t...>
	friend class Swizzler;

	template<class, size_t>
	friend class Vector;
};
