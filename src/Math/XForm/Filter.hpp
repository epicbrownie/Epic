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

//////////////////////////////////////////////////////////////////////////////

namespace Epic::Math::XForm
{
	template<class T = float>
	class IFilter;
}

//////////////////////////////////////////////////////////////////////////////

template<class T>
class Epic::Math::XForm::IFilter
{
public:
	virtual ~IFilter() { };

public:
	virtual T Apply(T) const noexcept = 0;
};
