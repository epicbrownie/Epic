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
		struct LinearImpl;
	}

	struct Linear;
}

//////////////////////////////////////////////////////////////////////////////

template<class T>
struct Epic::Math::XForm::detail::LinearImpl
{
	constexpr T operator() (T t) const noexcept
	{
		return t;
	}
};

//////////////////////////////////////////////////////////////////////////////

struct Epic::Math::XForm::Linear 
	: public detail::XFormImpl0<detail::LinearImpl> { };
