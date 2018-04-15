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

#include <Epic/Math/XForm/detail/Implementation.hpp>
#include <Epic/Math/XForm/Linear.hpp>
#include <Epic/Math/Algorithm.hpp>

//////////////////////////////////////////////////////////////////////////////

namespace Epic::Math::XForm
{
	namespace detail
	{
		template<class T, size_t N, class Inner>
		struct BellImpl;
	}

	template<size_t N, class Inner = Linear>
	struct Bell;
}

//////////////////////////////////////////////////////////////////////////////

template<class T, size_t N, class Inner>
struct Epic::Math::XForm::detail::BellImpl
{
	Inner BellInner;

	constexpr T operator() (T t) const noexcept
	{
		const T tprime = BellInner(t);
		const T t2 = tprime * tprime;
		const T ti = T(1) - tprime;
		const T t2i = T(1) - t2;
		const T ti2 = ti * ti;

		return Power<N>(T(28) * (t2 * t2i) * (ti2 * (T(1) - ti2)));
	}
};

template<class T, size_t N>
struct Epic::Math::XForm::detail::BellImpl<T, N, Epic::Math::XForm::detail::LinearImpl<T>>
{
	constexpr T operator() (T t) const noexcept
	{
		const T t2 = t * t;
		const T ti = T(1) - t;
		const T t2i = T(1) - t2;
		const T ti2 = ti * ti;

		return Power<N>( T(28) * (t2 * t2i) * (ti2 * (T(1) - ti2)) );
	}
};

//////////////////////////////////////////////////////////////////////////////

template<size_t N, class Inner>
struct Epic::Math::XForm::Bell
	: public detail::XFormNImpl1<N, Inner, detail::BellImpl> { };

template<size_t N, template<class...> class Inner, class... InnerArgs>
struct Epic::Math::XForm::Bell<N, Inner<InnerArgs...>>
	: public detail::XFormNImpl1<N, Inner<InnerArgs...>, detail::BellImpl> { };

//////////////////////////////////////////////////////////////////////////////

namespace Epic::Math::XForm
{
	using Bell2 = Bell<2>;
	using Bell3 = Bell<3>;
}
