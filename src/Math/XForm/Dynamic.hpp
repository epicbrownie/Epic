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
public:
	Epic::UniquePtr<IFilter<T>> pFilter;

private:
	struct NullFilter : public IFilter<T>
	{
		T Apply(T t) const noexcept override { return t; }
	};

public:
	DynamicImpl() : pFilter(Epic::MakeUnique<NullFilter>()) { }
	
public:
	constexpr T operator() (T t) const noexcept
	{
		return pFilter->Apply(t);
	}
};

//////////////////////////////////////////////////////////////////////////////

struct Epic::Math::XForm::Dynamic 
	: public detail::XFormImpl0<detail::DynamicImpl> { };
