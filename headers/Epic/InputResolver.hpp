//////////////////////////////////////////////////////////////////////////////
//
//            Copyright (c) 2017 Ronnie Brohn (EpicBrownie)      
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

#include <Epic/InputData.hpp>

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	class InputResolver;
}

//////////////////////////////////////////////////////////////////////////////

// InputResolver
class Epic::InputResolver
{
public:
	using Type = Epic::InputResolver;

public:
	InputResolver() noexcept = default;
	virtual ~InputResolver() noexcept = default;

public:
	virtual bool Resolve(const InputData& data) const noexcept = 0;
};
