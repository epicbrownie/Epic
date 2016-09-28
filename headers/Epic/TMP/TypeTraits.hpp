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

			Example: Determining the presence of member function 'Test(int, float) -> bool'
				template<class T>
				using TestExpr = decltype(std::declval<T&>().Test(int(), float()));

				template<class T>
				using HasTest = HasMemberFn<T, bool, TestExpr>;
	*/
	
	template<typename T, class R, template <typename> class Expr, typename = void>
	struct IsValidExpr: std::false_type { };

	template<typename T, class R, template <typename> class Expr>
	struct IsValidExpr<T, R, Expr, std::void_t<Expr<T>>>
		: std::is_same<R, Expr<T>> { };
}
