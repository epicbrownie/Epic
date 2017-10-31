//////////////////////////////////////////////////////////////////////////////
//
//            Copyright (c) 2016 Ronnie Brohn (EpicBrownie)      
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

#include <array>

//////////////////////////////////////////////////////////////////////////////

namespace Epic::detail
{
	template<class T>
	class QuaternionBase;
}

//////////////////////////////////////////////////////////////////////////////

// QuaternionBase
template<class T>
class Epic::detail::QuaternionBase
{
public:
	using Type = Epic::detail::QuaternionBase<T>;
	using value_type = T;

	union
	{
		// Value Array
		std::array<value_type, 4> Values;

		// Components
		struct
		{
			value_type x, y, z, w;
		};
	};
};
