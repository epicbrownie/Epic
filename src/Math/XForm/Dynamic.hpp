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
#include <Epic/STL/UniquePtr.hpp>
#include <Epic/detail/ReadConfig.hpp>

//////////////////////////////////////////////////////////////////////////////

namespace Epic::Math::XForm
{
	namespace detail
	{
		template<class T>
		class DynamicImpl;
	}

	struct Dynamic;
}

//////////////////////////////////////////////////////////////////////////////

template<class T>
class Epic::Math::XForm::detail::DynamicImpl
{
private:
	using DynamicFilterPtr = typename Epic::detail::GetConfigFactoryOr<
		Epic::detail::eConfigFactory::DynamicFilterPtr,
		Epic::UniquePtr,
		IFilter<T>
	>::type;

public:
	DynamicFilterPtr pFilter;

public:
	constexpr T operator() (T t) const noexcept
	{
		return pFilter->Apply(t);
	}
};

//////////////////////////////////////////////////////////////////////////////

struct Epic::Math::XForm::Dynamic 
	: public detail::XFormImpl0<detail::DynamicImpl> { };
