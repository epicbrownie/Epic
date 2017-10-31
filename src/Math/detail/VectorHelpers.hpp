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
#include <Epic/Math/detail/SwizzlerFwd.hpp>
#include <array>
#include <type_traits>

//////////////////////////////////////////////////////////////////////////////

namespace Epic::detail
{
	template<class... Ts>
	struct Span;

	template<class T>
	struct SpanOf;

	template<class T, std::size_t Size>
	struct VectorHelper;

	template<class T, std::size_t Size>
	struct SVectorHelper;
}

//////////////////////////////////////////////////////////////////////////////

// SpanOf
template<class T>
struct Epic::detail::SpanOf
{
	constexpr static std::size_t Value = 1;
};

// SpanOf<Vector>
template<class T, std::size_t Size>
struct Epic::detail::SpanOf<Epic::Vector<T, Size>>
{
	constexpr static std::size_t Value = Size;
};

// SpanOf<Swizzler>
template<class T, std::size_t VS, std::size_t... Indices>
struct Epic::detail::SpanOf<Epic::Swizzler<T, VS, Indices...>>
{
	constexpr static std::size_t Value = sizeof...(Indices);
};

// SpanOf<T[N]>
template<class T, std::size_t N>
struct Epic::detail::SpanOf<T[N]>
{
	static constexpr std::size_t Value = N;
};

// SpanOf<std::array>
template<class T, std::size_t Size>
struct Epic::detail::SpanOf<std::array<T, Size>>
{
	static constexpr std::size_t Value = Size;
};

// Span<...>
template<class... Ts>
struct Epic::detail::Span
{
	constexpr static std::size_t Value = 0;
};

// Span<T, ...>
template<class T, class... Ts>
struct Epic::detail::Span<T, Ts...>
{
	constexpr static std::size_t Value = Epic::detail::SpanOf<std::remove_reference_t<std::remove_cv_t<T>>>::Value + Span<Ts...>::Value;
};

//////////////////////////////////////////////////////////////////////////////

// VectorHelper
template<class T, std::size_t Size>
struct Epic::detail::VectorHelper
{
	static_assert(std::is_pod<T>::value, "T must be a POD type.");

	using TArray = std::array<T, Size>;

	template<std::size_t... Indices>
	struct SwizzlerGenerator
	{
		using Type = Epic::Swizzler<T, Size, Indices...>;
	};

	template<std::size_t Index>
	struct SwizzlerGenerator<Index>
	{
		using Type = T;
	};

	using BaseType = Epic::detail::VectorBase<Size, SwizzlerGenerator, TArray>;
};

//////////////////////////////////////////////////////////////////////////////

// SVectorHelper
template<class T, std::size_t Size>
struct Epic::detail::SVectorHelper
{
	static_assert(std::is_pod<T>::value, "T must be a POD type.");

	using VH = VectorHelper<T, Size>;
	using TArray = typename VH::TArray;
	
	using BaseType = Epic::detail::SVectorBase<Size, typename VH::SwizzlerGenerator, TArray>;
};
