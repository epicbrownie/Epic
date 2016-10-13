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

#include <type_traits>
#include <utility>

//////////////////////////////////////////////////////////////////////////////

/// HasMemberFn<>
namespace Epic::TMP
{
	/*
		IsValidExpr is used to determine whether or not Expr<T> is valid and evaluates to
		the type R.
	*/
	
	template<typename T, class R, template <typename> class Expr, typename = void>
	struct IsValidExpr: std::false_type { };

	template<typename T, class R, template <typename> class Expr>
	struct IsValidExpr<T, R, Expr, std::void_t<Expr<T>>>
		: std::is_same<R, Expr<T>> { };


	/*
		Derives from std::true_type if Function(Args...) could be called and would result in
		a type that is implicitly convertible to ReturnType.
	*/

	template<class Function, class Return, class Enable = void>
	struct IsCallable : std::false_type { };

	template<class Function, class Return, class... Ts>
	struct IsCallable<Function(Ts...), Return,
		std::void_t< decltype(std::declval<Function>() (std::declval<Ts>()...))> >
		: std::is_convertible<decltype(std::declval<Function>() (std::declval<Ts>()...)), Return>
	{ };


	/*
		determines whether or not type U is explicitly convertible to T.
		True if T(U&) is valid, but an implicit conversion to T from U is not.
	*/

	template <class U, class T>
	struct IsExplicitlyConvertible
	{
		static constexpr bool value = std::is_constructible<T, U>::value && !std::is_convertible<U, T>::value;
	};
}
