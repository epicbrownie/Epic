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

#include <Epic/EON/detail/Tags.hpp>
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
		bool ConvertIf(Converter convertFn, To& to, From from)
		{
			if constexpr (std::is_invocable_r_v<bool, Converter, To&, From>)
				return convertFn(to, std::move(from));
			else
				return DefaultConverter() (to, std::move(from));
		}
	}
}

//////////////////////////////////////////////////////////////////////////////

template<class F, class T>
struct Epic::EON::detail::Assign
{
	bool operator() (T& to, F from)
	{
		to = std::move(from);
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
	bool operator() (T& to, F from)
	{
		to = static_cast<T>(from);
		return true;
	}
};

template<class F, class T>
struct Epic::EON::detail::ConstructConvert
{
	bool operator() (T& to, F from)
	{
		to = T(std::move(from));
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

	bool DoConvert(T& to, F from, FromTag)
	{
		using UF = std::underlying_type_t<F>;

		return Convert<UF, T>() (to, static_cast<UF>(from));
	}

	bool DoConvert(T& to, F from, ToTag)
	{
		using UT = std::underlying_type_t<T>;
		
		UT item;
		if (!Convert<F, UT>() (item, std::move(from)))
			return false;

		to = static_cast<T>(item);

		return true;
	}

	bool DoConvert(T& to, F from, BothTag)
	{
		using UF = std::underlying_type_t<F>;
		using UT = std::underlying_type_t<T>;

		UT item;
		if (!Convert<UF, UT>() (item, static_cast<UF>(from)))
			return false;
		
		to = static_cast<T>(item);

		return true;
	}

public:
	bool operator() (T& to, F from)
	{
		return DoConvert(to, std::move(from), MakeEnumTag());
	}
};

template<class F, class T>
struct Epic::EON::detail::NullConvert
{
	bool operator() (T& /*to*/, F /*from*/)
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
		std::conditional_t<std::is_invocable_r_v<bool, TypeConvert<F, T>, T&, F>, TypeConvert<F, T>,
		NullConvert<F, T>>>>>>;
};

template<class F, class T>
class Epic::EON::detail::Convert
{
private:
	using Traits = EONTraits<T>;

	using MakeConversionTag =
		std::conditional_t<!Traits::IsContainer, AutoTag,
		std::conditional_t<Traits::IsVectorLike, ArrayTag,
		std::conditional_t<Traits::IsSetLike, SetTag,
		FailTag>>>;

private:
	bool DoConversion(T&, F, FailTag) 
	{ 
		return false; 
	}

	bool DoConversion(T& to, F from, AutoTag)
	{
		return AutoConvert<F, T>::Type() (to, std::move(from));
	}

	bool DoConversion(T& to, F from, ArrayTag)
	{
		using Item = typename T::value_type;

		static_assert(std::is_default_constructible_v<Item>, "Array value type must be default constructible");

		Item& item = to.emplace_back();
		
		return Convert<F, Item>() (item, std::move(from));
	}

	bool DoConversion(T& to, F from, SetTag)
	{
		using Item = typename T::key_type;
		
		static_assert(std::is_default_constructible_v<Item>, "Set key type must be default constructible");

		Item item;
		
		if (!Convert<F, Item>() (item, std::move(from)))
			return false;

		to.emplace(std::move(item));

		return true;
	}

public:
	bool operator() (T& to, F from)
	{
		return DoConversion(to, std::move(from), MakeConversionTag());
	}
};

//////////////////////////////////////////////////////////////////////////////

struct Epic::EON::DefaultConverter
{
	template<class T, class F>
	bool operator() (T& to, F from)
	{
		return detail::Convert<F, T>() (to, std::move(from));
	}
};
