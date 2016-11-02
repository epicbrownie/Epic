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

#include <Epic/TMP/Sequence.hpp>
#include <Epic/Math/detail/VectorBase.hpp>
#include <Epic/Math/detail/VectorFwd.hpp>
#include <Epic/Math/detail/VectorSwizzlerFwd.hpp>
#include <array>
#include <type_traits>

//////////////////////////////////////////////////////////////////////////////

namespace Epic::detail
{
	template<class... Ts>
	struct Span;

	template<class T>
	struct SpanOf;

	template<class T, size_t Size>
	struct VectorHelper;
}

//////////////////////////////////////////////////////////////////////////////

// SpanOf
template<class T>
struct Epic::detail::SpanOf
{
	constexpr static size_t Value = 1;
};

// SpanOf<Vector>
template<class T, size_t Size>
struct Epic::detail::SpanOf<Epic::Vector<T, Size>>
{
	constexpr static size_t Value = Size;
};

// SpanOf<VectorSwizzler>
template<class VectorType, class TArray, size_t... Indices>
struct Epic::detail::SpanOf<Epic::VectorSwizzler<VectorType, TArray, Indices...>>
{
	constexpr static size_t Value = Epic::detail::SpanOf<VectorType>::Value;
};

// SpanOf<T[N]>
template<class T, size_t N>
struct Epic::detail::SpanOf<T[N]>
{
	static constexpr size_t Value = N;
};

// SpanOf<std::array>
template<class T, size_t Size>
struct Epic::detail::SpanOf<std::array<T, Size>>
{
	static constexpr size_t Value = Size;
};

// Span<...>
template<class... Ts>
struct Epic::detail::Span
{
	constexpr static size_t Value = 0;
};

// Span<T, ...>
template<class T, class... Ts>
struct Epic::detail::Span<T, Ts...>
{
	constexpr static size_t Value = Epic::detail::SpanOf<std::remove_reference_t<std::remove_cv_t<T>>>::Value + Span<Ts...>::Value;
};

//////////////////////////////////////////////////////////////////////////////

// VectorHelper
template<class T, size_t Size>
struct Epic::detail::VectorHelper
{
	static_assert(std::is_pod<T>::value, "T must be a POD type.");

	using TArray = std::array<T, Size>;

	template<size_t... Indices>
	struct SwizzlerGenerator
	{
		using Type = Epic::VectorSwizzler<Epic::Vector<T, sizeof...(Indices)>, TArray, Indices...>;
	};

	template<size_t Index>
	struct SwizzlerGenerator<Index>
	{
		using Type = T;
	};

	using BaseType = Epic::detail::VectorBase<Size, SwizzlerGenerator, TArray>;
};
