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
template<class T, std::size_t VS, std::size_t... Indices>
class Epic::Swizzler
{
public:
	using Type = Epic::Swizzler<T, VS, Indices...>;

public:
	static constexpr std::size_t Size = sizeof...(Indices);

	using ScalarType = T;
	using VectorType = Epic::Vector<T, sizeof...(Indices)>;

private:
	static constexpr std::size_t ParentVectorSize = VS;
	
	static constexpr bool IsAssignableLValue = 
		Epic::TMP::IsSequenceUnique<Epic::TMP::Sequence<std::size_t, Indices...>>::value;
		
	using TArray = typename Epic::detail::SVectorHelper<ScalarType, ParentVectorSize>::TArray;
	using IndexSeq = Epic::TMP::Sequence<std::size_t, Indices...>;
	using VectorSeq = Epic::TMP::MakeSequence<std::size_t, Size>;

private:
	TArray m_Values;

public:
	Swizzler() noexcept = default;

public:
	VectorType ToVector() const noexcept
	{
		VectorType result;

		Epic::TMP::ForEach2<VectorSeq, IndexSeq>::Apply([&] (std::size_t dest, std::size_t src)
		{
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

	template<typename = std::enable_if_t<IsAssignableLValue>>
	Type& operator = (const T& value) noexcept
	{
		Epic::TMP::ForEach<IndexSeq>::Apply([&] (std::size_t i) { m_Values[i] = value; });
		return *this;
	}

	template<typename = std::enable_if_t<IsAssignableLValue>>
	Type& operator += (const T& value) noexcept
	{
		Epic::TMP::ForEach<IndexSeq>::Apply([&] (std::size_t i) { m_Values[i] += value; });
		return *this;
	}

	template<typename = std::enable_if_t<IsAssignableLValue>>
	Type& operator -= (const T& value) noexcept
	{
		Epic::TMP::ForEach<IndexSeq>::Apply([&] (std::size_t i) { m_Values[i] -= value; });
		return *this;
	}

	template<typename = std::enable_if_t<IsAssignableLValue>>
	Type& operator *= (const T& value) noexcept
	{
		Epic::TMP::ForEach<IndexSeq>::Apply([&] (std::size_t i) { m_Values[i] *= value; });
		return *this;
	}

	template<typename = std::enable_if_t<IsAssignableLValue>>
	Type& operator /= (const T& value) noexcept
	{
		Epic::TMP::ForEach<IndexSeq>::Apply([&] (std::size_t i) { m_Values[i] /= value; });
		return *this;
	}

	template<typename = std::enable_if_t<IsAssignableLValue>>
	Type& operator = (const Type& other) noexcept
	{
		Epic::TMP::ForEach<IndexSeq>::Apply([&] (std::size_t i) { m_Values[i] = other.m_Values[i]; });
		return *this;
	}

	template<typename = std::enable_if_t<IsAssignableLValue>>
	Type& operator += (const Type& other) noexcept
	{
		Epic::TMP::ForEach<IndexSeq>::Apply([&] (std::size_t i) { m_Values[i] += other.m_Values[i]; });
		return *this;
	}

	template<typename = std::enable_if_t<IsAssignableLValue>>
	Type& operator -= (const Type& other) noexcept
	{
		Epic::TMP::ForEach<IndexSeq>::Apply([&] (std::size_t i) { m_Values[i] -= other.m_Values[i]; });
		return *this;
	}

	template<typename = std::enable_if_t<IsAssignableLValue>>
	Type& operator *= (const Type& other) noexcept
	{
		Epic::TMP::ForEach<IndexSeq>::Apply([&] (std::size_t i) { m_Values[i] *= other.m_Values[i]; });
		return *this;
	}

	template<typename = std::enable_if_t<IsAssignableLValue>>
	Type& operator /= (const Type& other) noexcept
	{
		Epic::TMP::ForEach<IndexSeq>::Apply([&] (std::size_t i) { m_Values[i] /= other.m_Values[i]; });
		return *this;
	}

	template<typename = std::enable_if_t<IsAssignableLValue>>
	Type& operator = (const T(&values)[Size]) noexcept
	{
		Epic::TMP::ForEach2<VectorSeq, IndexSeq>::Apply([&] (std::size_t i, std::size_t j) { m_Values[i] = values[j]; });
		return *this;
	}

	template<typename = std::enable_if_t<IsAssignableLValue>>
	Type& operator += (const T(&values)[Size]) noexcept
	{
		Epic::TMP::ForEach2<VectorSeq, IndexSeq>::Apply([&] (std::size_t i, std::size_t j) { m_Values[i] += values[j]; });
		return *this;
	}

	template<typename = std::enable_if_t<IsAssignableLValue>>
	Type& operator -= (const T(&values)[Size]) noexcept
	{
		Epic::TMP::ForEach2<VectorSeq, IndexSeq>::Apply([&] (std::size_t i, std::size_t j) { m_Values[i] -= values[j]; });
		return *this;
	}

	template<typename = std::enable_if_t<IsAssignableLValue>>
	Type& operator *= (const T(&values)[Size]) noexcept
	{
		Epic::TMP::ForEach2<VectorSeq, IndexSeq>::Apply([&] (std::size_t i, std::size_t j) { m_Values[i] *= values[j]; });
		return *this;
	}

	template<typename = std::enable_if_t<IsAssignableLValue>>
	Type& operator /= (const T(&values)[Size]) noexcept
	{
		Epic::TMP::ForEach2<VectorSeq, IndexSeq>::Apply([&] (std::size_t i, std::size_t j) { m_Values[i] /= values[j]; });
		return *this;
	}

	template<typename = std::enable_if_t<IsAssignableLValue>>
	Type& operator = (const VectorType& vec) noexcept
	{
		Epic::TMP::ForEach2<VectorSeq, IndexSeq>::Apply([&] (std::size_t i, std::size_t j) { m_Values[i] = vec[j]; });
		return *this;
	}

	template<typename = std::enable_if_t<IsAssignableLValue>>
	Type& operator += (const VectorType& vec) noexcept
	{
		Epic::TMP::ForEach2<VectorSeq, IndexSeq>::Apply([&] (std::size_t i, std::size_t j) { m_Values[i] += vec[j]; });
		return *this;
	}

	template<typename = std::enable_if_t<IsAssignableLValue>>
	Type& operator -= (const VectorType& vec) noexcept
	{
		Epic::TMP::ForEach2<VectorSeq, IndexSeq>::Apply([&] (std::size_t i, std::size_t j) { m_Values[i] -= vec[j]; });
		return *this;
	}

	template<typename = std::enable_if_t<IsAssignableLValue>>
	Type& operator *= (const VectorType& vec) noexcept
	{
		Epic::TMP::ForEach2<VectorSeq, IndexSeq>::Apply([&] (std::size_t i, std::size_t j) { m_Values[i] *= vec[j]; });
		return *this;
	}

	template<typename = std::enable_if_t<IsAssignableLValue>>
	Type& operator /= (const VectorType& vec) noexcept
	{
		Epic::TMP::ForEach2<VectorSeq, IndexSeq>::Apply([&] (std::size_t i, std::size_t j) { m_Values[i] /= vec[j]; });
		return *this;
	}

	template<std::size_t TS, std::size_t... Is, typename = std::enable_if_t<IsAssignableLValue>>
	Type& operator = (const Swizzler<T, TS, Is...>& vec) noexcept
	{
		using IndexSeq2 = Epic::TMP::Sequence<std::size_t, Is...>;

		Epic::TMP::ForEach2<IndexSeq, IndexSeq2>::Apply(
			[&] (std::size_t i, std::size_t j) { m_Values[i] = vec.m_Values[j]; });

		return *this;
	}

	template<std::size_t TS, std::size_t... Is, typename = std::enable_if_t<IsAssignableLValue>>
	Type& operator += (const Swizzler<T, TS, Is...>& vec) noexcept
	{
		using IndexSeq2 = Epic::TMP::Sequence<std::size_t, Is...>;

		Epic::TMP::ForEach2<IndexSeq, IndexSeq2>::Apply(
			[&] (std::size_t i, std::size_t j) { m_Values[i] += vec.m_Values[j]; });

		return *this;
	}

	template<std::size_t TS, std::size_t... Is, typename = std::enable_if_t<IsAssignableLValue>>
	Type& operator -= (const Swizzler<T, TS, Is...>& vec) noexcept
	{
		using IndexSeq2 = Epic::TMP::Sequence<std::size_t, Is...>;

		Epic::TMP::ForEach2<IndexSeq, IndexSeq2>::Apply(
			[&] (std::size_t i, std::size_t j) { m_Values[i] -= vec.m_Values[j]; });

		return *this;
	}

	template<std::size_t TS, std::size_t... Is, typename = std::enable_if_t<IsAssignableLValue>>
	Type& operator *= (const Swizzler<T, TS, Is...>& vec) noexcept
	{
		using IndexSeq2 = Epic::TMP::Sequence<std::size_t, Is...>;

		Epic::TMP::ForEach2<IndexSeq, IndexSeq2>::Apply(
			[&] (std::size_t i, std::size_t j) { m_Values[i] *= vec.m_Values[j]; });

		return *this;
	}

	template<std::size_t TS, std::size_t... Is, typename = std::enable_if_t<IsAssignableLValue>>
	Type& operator /= (const Swizzler<T, TS, Is...>& vec) noexcept
	{
		using IndexSeq2 = Epic::TMP::Sequence<std::size_t, Is...>;

		Epic::TMP::ForEach2<IndexSeq, IndexSeq2>::Apply(
			[&] (std::size_t i, std::size_t j) { m_Values[i] /= vec.m_Values[j]; });

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

	template<std::size_t TS, std::size_t... Is, typename = std::enable_if_t<(sizeof...(Is) == Size)>>
	auto operator + (const Swizzler<T, TS, Is...>& vec) const noexcept
	{
		return ToVector() + vec;
	}

	template<std::size_t TS, std::size_t... Is, typename = std::enable_if_t<(sizeof...(Is) == Size)>>
	auto operator - (const Swizzler<T, TS, Is...>& vec) const noexcept 
	{
		return ToVector() - vec;
	}

	template<std::size_t TS, std::size_t... Is, typename = std::enable_if_t<(sizeof...(Is) == Size)>>
	auto operator * (const Swizzler<T, TS, Is...>& vec) const noexcept
	{
		return ToVector() * vec;
	}
	
	template<std::size_t TS, std::size_t... Is, typename = std::enable_if_t<(sizeof...(Is) == Size)>>
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
	template<class, std::size_t, std::size_t...>
	friend class Swizzler;

	template<class, std::size_t>
	friend class Vector;
};
