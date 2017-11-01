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

#include <Epic/EON/detail/TypeConvert.hpp>
#include <Epic/EON/detail/Traits.hpp>
#include <Epic/TMP/TypeTraits.hpp>

//////////////////////////////////////////////////////////////////////////////

namespace Epic::EON::detail
{
	template<class F, class T>
	struct Assign;

	template<class F, class T>
	struct CastConvert;

	template<class F, class T>
	struct ConstructConvert;

	template<class F, class T>
	struct EnumConvert;

	template<class F, class T>
	struct NullConvert;

	template<class F, class T>
	struct AutoConvert;

	template<class F, class T>
	class Convert;
}

//////////////////////////////////////////////////////////////////////////////

namespace Epic::EON
{
	struct DefaultConverter;
}

//////////////////////////////////////////////////////////////////////////////

namespace Epic::EON::detail
{
	namespace
	{
		template<class Converter, class From, class To>
		bool ConvertIf(Converter convertFn, To& to, const From& from)
		{
			if constexpr (std::is_invocable_r_v<bool, Converter, To&, const From&>)
				return convertFn(to, from);
			else
				return DefaultConverter() (to, from);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////

template<class F, class T>
struct Epic::EON::detail::Assign
{
	bool operator() (T& to, const F& from)
	{
		to = from;
		return true;
	}
};

template<class U, size_t N>
struct Epic::EON::detail::Assign<U[N], U[N]>
{
	bool operator() (U(&to)[N], const U(&from)[N])
	{
		std::memcpy(to, from, N * sizeof(U));
		return true;
	}
};

template<class F, class T>
struct Epic::EON::detail::CastConvert
{
	bool operator() (T& to, const F& from)
	{
		to = static_cast<T>(from);
		return true;
	}
};

template<class F, class T>
struct Epic::EON::detail::ConstructConvert
{
	bool operator() (T& to, const F& from)
	{
		to = T(from);
		return true;
	}
};

template<class F, class T>
struct Epic::EON::detail::EnumConvert
{
private:
	struct FromTag{ };
	struct ToTag{ };
	struct BothTag{ };

	using MakeEnumTag =
		std::conditional_t<std::is_enum_v<F> && std::is_enum_v<T>, BothTag,
		std::conditional_t<std::is_enum_v<F>, FromTag, ToTag>>;

	bool DoConvert(T& to, const F& from, FromTag)
	{
		using UF = std::underlying_type_t<F>;

		return Convert<UF, T>() (to, static_cast<UF>(from));
	}

	bool DoConvert(T& to, const F& from, ToTag)
	{
		using UT = std::underlying_type_t<T>;
		
		return Convert<F, UT>() (reinterpret_cast<UT&>(to), from);
	}

	bool DoConvert(T& to, const F& from, BothTag)
	{
		using UF = std::underlying_type_t<F>;
		using UT = std::underlying_type_t<T>;

		return Convert<F, UT>() (reinterpret_cast<UT&>(to), static_cast<UF>(from));
	}

public:
	bool operator() (T& to, const F& from)
	{
		return DoConvert(to, from, MakeEnumTag());
	}
};

template<class F, class T>
struct Epic::EON::detail::NullConvert
{
	bool operator() (T& to, const F& from)
	{
		return false;
	}
};

template<class F, class T>
struct Epic::EON::detail::AutoConvert
{
	using Type =
		std::conditional_t<std::is_same_v<F, T>, Assign<F, T>,
		std::conditional_t<std::is_enum_v<F> || std::is_enum_v<T>, EnumConvert<F, T>,
		std::conditional_t<std::is_convertible_v<F, T>, CastConvert<F, T>,
		std::conditional_t<Epic::TMP::IsExplicitlyConvertibleV<F, T>, ConstructConvert<F, T>,
		std::conditional_t<std::is_invocable_r_v<bool, TypeConvert<F, T>, T&, const F&>, TypeConvert<F, T>,
		NullConvert<F, T>>>>>>;
};

template<class F, class T>
class Epic::EON::detail::Convert
{
private:
	using Traits = EONTraits<T>;

	struct FailTag { };
	struct AutoTag { };
	struct ArrayTag { };
	struct SetTag { };

	using MakeConversionTag =
		std::conditional_t<!Traits::IsContainer, AutoTag,
		std::conditional_t<Traits::IsVectorLike, ArrayTag,
		std::conditional_t<Traits::IsSetLike, SetTag,
		FailTag>>>;

private:
	bool DoConversion(T&, const F&, FailTag) 
	{ 
		return false; 
	}

	bool DoConversion(T& to, const F& from, AutoTag)
	{
		return AutoConvert<F, T>::Type() (to, from);
	}

	bool DoConversion(T& to, const F& from, ArrayTag)
	{
		static_assert(std::is_default_constructible_v<typename T::value_type>, "Value must be default constructible");

		typename T::value_type item;
		Convert<F, typename T::value_type> fnConvert;

		if (!fnConvert(item, from))
			return false;

		to.emplace_back(std::move(item));
		
		return true;
	}

	bool DoConversion(T& to, const F& from, SetTag)
	{
		static_assert(std::is_default_constructible_v<typename T::key_type>, "Value must be default constructible");

		typename T::key_type item;
		Convert<F, typename T::key_type> fnConvert;

		if (!fnConvert(item, from))
			return false;

		to.emplace(std::move(item));

		return true;
	}

public:
	bool operator() (T& to, const F& from)
	{
		return DoConversion(to, from, MakeConversionTag());
	}
};

//////////////////////////////////////////////////////////////////////////////

struct Epic::EON::DefaultConverter
{
	template<class T, class F>
	bool operator() (T& to, const F& from)
	{
		return detail::Convert<F, T>()(to, from);
	}
};
