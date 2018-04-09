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
		struct InverseImpl;
	}

	struct Inverse;
}

//////////////////////////////////////////////////////////////////////////////

template<class T>
struct Epic::Math::XForm::detail::InverseImpl
{
	// NOTE: Equivalent to Flip<Linear>
	constexpr T operator() (T t) const noexcept
	{
		return T(1) - t;
	}
};

//////////////////////////////////////////////////////////////////////////////

struct Epic::Math::XForm::Inverse 
	: public detail::XFormImpl0<detail::InverseImpl> { };
