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
		struct ConstantImpl;
	}

	struct Constant;
}

//////////////////////////////////////////////////////////////////////////////

template<class T>
struct Epic::Math::XForm::detail::ConstantImpl
{
	T Value = T(0);

	constexpr T operator() (T) const noexcept
	{
		return Value;
	}
};

//////////////////////////////////////////////////////////////////////////////

struct Epic::Math::XForm::Constant 
	: public detail::XFormImpl0<detail::ConstantImpl> { };
