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
#include <Epic/Math/XForm/Filter.hpp>
#include <Epic/Math/XForm/XForms.hpp>

//////////////////////////////////////////////////////////////////////////////

namespace Epic::Math::XForm
{
	template<class Descriptor, class T = float>
	using XForm = typename detail::ImplOf<Descriptor, T>::Type;

	template<class Descriptor, class T = float>
	class XFormFilter;
}

//////////////////////////////////////////////////////////////////////////////

template<class Descriptor, class T>
class Epic::Math::XForm::XFormFilter :
	public IFilter<T>,
	public XForm<Descriptor, T>
{
	using XFormBase = XForm<Descriptor, T>;

	inline T Apply(T t) const noexcept override
	{
		return XFormBase::operator() (t);
	}
};
