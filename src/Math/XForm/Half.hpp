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
		struct HalfImpl;
	}

	struct Half;
}

//////////////////////////////////////////////////////////////////////////////

template<class T>
struct Epic::Math::XForm::detail::HalfImpl
{
	// NOTE: Equivalent to Divide<2, Linear>
	constexpr T operator() (T t) const noexcept
	{
		return T(0.5) * t;
	}
};

//////////////////////////////////////////////////////////////////////////////

struct Epic::Math::XForm::Half
	: public detail::XFormImpl0<detail::HalfImpl> { };
