//////////////////////////////////////////////////////////////////////////////
//
//            Copyright (c) 2018 Ronnie Brohn (EpicBrownie)      
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

#include <Epic/TMP/List.hpp>
#include <Epic/TMP/Utility.hpp>

//////////////////////////////////////////////////////////////////////////////

namespace Epic::Math::XForm::detail
{
	template<size_t N>
	using _C = Epic::TMP::Literal<size_t, N>;

	template<class A, class B>
	using _CSum = _C<A::value + B::value>;

	template<class T>
	struct _LSum;

	template<size_t N>
	struct BezierCoefficientList;
	
	template<size_t N>
	using BezierCoefficients = typename Epic::TMP::ListToTuple<typename BezierCoefficientList<N>::Type>::Type;
}

//////////////////////////////////////////////////////////////////////////////

template<class A, class B, class... Rest>
struct Epic::Math::XForm::detail::_LSum<Epic::TMP::List<A, B, Rest...>>
{
	using Type = typename Epic::TMP::Concat<
		Epic::TMP::List<_CSum<A, B>>,
		typename _LSum<Epic::TMP::List<B, Rest...>>::Type
	>::Type;
};

template<class A, class B>
struct Epic::Math::XForm::detail::_LSum<Epic::TMP::List<A, B>>
{
	using Type = Epic::TMP::List<_CSum<A, B>>;
};

template<class A>
struct Epic::Math::XForm::detail::_LSum<Epic::TMP::List<A>>
{
	using Type = Epic::TMP::List<>;
};

template<>
struct Epic::Math::XForm::detail::_LSum<Epic::TMP::List<>>
{
	using Type = Epic::TMP::List<>;
};

//////////////////////////////////////////////////////////////////////////////

template<size_t N>
struct Epic::Math::XForm::detail::BezierCoefficientList
{
	using Type = typename Epic::TMP::Concat<
		Epic::TMP::List<_C<1>>,
		typename _LSum<typename BezierCoefficientList<N - 1>::Type>::Type,
		Epic::TMP::List<_C<1>>
	>::Type;
};

template<> 
struct Epic::Math::XForm::detail::BezierCoefficientList<0>
{ 
	using Type = Epic::TMP::List<>; 
};
