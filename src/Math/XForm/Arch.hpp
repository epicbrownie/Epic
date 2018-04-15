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
		struct ArchImpl;
	}

	template<size_t N, class Inner = Linear>
	struct Arch;
}

//////////////////////////////////////////////////////////////////////////////

template<class T, size_t N, class Inner>
struct Epic::Math::XForm::detail::ArchImpl
{
	Inner ArchInner;

	// NOTE: Equivalent to Modulate<Inner, Flip<Inner>>
	constexpr T operator() (T t) const noexcept
	{
		const T tprime = ArchInner(t);

		return Power<N>(T(4) * tprime * (T(1) - tprime));
	}
};

template<class T, size_t N>
struct Epic::Math::XForm::detail::ArchImpl<T, N, Epic::Math::XForm::detail::LinearImpl<T>>
{
	// NOTE: Equivalent to Minify<Inverse>
	constexpr T operator() (T t) const noexcept
	{
		return Power<N>(T(4) * t * (T(1) - t));
	}
};

//////////////////////////////////////////////////////////////////////////////

template<size_t N, class Inner>
struct Epic::Math::XForm::Arch
	: public detail::XFormNImpl1<N, Inner, detail::ArchImpl> { };

template<size_t N, template<class...> class Inner, class... InnerArgs>
struct Epic::Math::XForm::Arch<N, Inner<InnerArgs...>>
	: public detail::XFormNImpl1<N, Inner<InnerArgs...>, detail::ArchImpl> { };

//////////////////////////////////////////////////////////////////////////////

namespace Epic::Math::XForm
{
	using Arch2 = Arch<2>;
	using Arch3 = Arch<3>;
}
