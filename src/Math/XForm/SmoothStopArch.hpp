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

#include <Epic/Math/XForm/Arch.hpp>
#include <Epic/Math/XForm/Linear.hpp>
#include <Epic/Math/XForm/Magnify.hpp>

//////////////////////////////////////////////////////////////////////////////

namespace Epic::Math::XForm
{
	namespace detail
	{
		template<class T, size_t N, class Inner>
		class SmoothStopArchImpl;
	}

	template<size_t N, class Inner = Linear>
	struct SmoothStopArch;
}

//////////////////////////////////////////////////////////////////////////////

template<class T, size_t N, class Inner>
class Epic::Math::XForm::detail::SmoothStopArchImpl
{
private:
	detail::ArchImpl<T, N, detail::LinearImpl<T>> m_Arch;
	detail::MagnifyImpl<T, detail::LinearImpl<T>> m_Magnify;

public:
	Inner SmoothArchInner;

public:
	constexpr T operator() (T t) const noexcept
	{
		const T tprime = T(1) - SmoothArchInner(t);

		return m_Arch(m_Magnify(tprime));
	}
};

template<class T, size_t N>
class Epic::Math::XForm::detail::SmoothStopArchImpl<T, N, Epic::Math::XForm::detail::LinearImpl<T>>
{
private:
	detail::ArchImpl<T, N, detail::LinearImpl<T>> m_Arch;
	detail::MagnifyImpl<T, detail::LinearImpl<T>> m_Magnify;

public:
	constexpr T operator() (T t) const noexcept
	{
		const T tprime = T(1) - t;

		return m_Arch(m_Magnify(tprime));
	}
};

//////////////////////////////////////////////////////////////////////////////

template<size_t N, class Inner>
struct Epic::Math::XForm::SmoothStopArch
	: public detail::XFormNImpl1<N, Inner, detail::SmoothStopArchImpl> { };

template<size_t N, template<class...> class Inner, class... InnerArgs>
struct Epic::Math::XForm::SmoothStopArch<N, Inner<InnerArgs...>>
	: public detail::XFormNImpl1<N, Inner<InnerArgs...>, detail::SmoothStopArchImpl> { };

//////////////////////////////////////////////////////////////////////////////

namespace Epic::Math::XForm
{
	using SmoothStopArch2 = SmoothStopArch<2>;
	using SmoothStopArch3 = SmoothStopArch<3>;
}