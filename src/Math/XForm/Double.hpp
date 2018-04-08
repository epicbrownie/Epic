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

//////////////////////////////////////////////////////////////////////////////

namespace Epic::Math::XForm
{
	namespace detail
	{
		template<class T>
		struct DoubleImpl;
	}

	struct Double;
}

//////////////////////////////////////////////////////////////////////////////

template<class T>
struct Epic::Math::XForm::detail::DoubleImpl
{
	// NOTE: Equivalent to Multiply<2, Linear>
	constexpr T operator() (T t) const noexcept
	{
		return T(2) * t;
	}
};

//////////////////////////////////////////////////////////////////////////////

struct Epic::Math::XForm::Double
	: public detail::XFormImpl0<detail::DoubleImpl> { };
