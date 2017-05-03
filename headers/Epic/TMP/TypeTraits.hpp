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

namespace Epic::TMP
{
	/*	Derives from std::true_type if Function(Args...) could be called and would result in
		a type that is implicitly convertible to ReturnType.  Derives from std::false_type otherwise. */

	template<class Function, class Return, class Enable = void>
	struct IsCallable : std::false_type { };

	template<class Function, class Return, class... Ts>
	struct IsCallable<Function(Ts...), Return,
		std::void_t< decltype(std::declval<Function>() (std::declval<Ts>()...))> >
		: std::is_convertible<decltype(std::declval<Function>() (std::declval<Ts>()...)), Return>
	{ };
}

//////////////////////////////////////////////////////////////////////////////

namespace Epic::TMP
{
	/*	Derives from std::true_type if type U is explicitly convertible to T.
		That is, if T(U&) is valid, but an implicit conversion to T from U is not.
		Derives from std::false_type otherwise. */

	template <class U, class T>
	struct IsExplicitlyConvertible :
		std::conditional_t<std::is_constructible<T, U>::value && !std::is_convertible<U, T>::value, 
						   std::true_type, std::false_type>
	{ };
}
