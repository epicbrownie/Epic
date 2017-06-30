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

#include <algorithm>
#include <iterator>

//////////////////////////////////////////////////////////////////////////////

namespace Epic::Ext
{
	/// Perform a generic insertion sort
	template<class ForwardIt, class Compare = std::less<class std::iterator_traits<ForwardIt>::value_type>>
	inline void insertion_sort(ForwardIt begin, ForwardIt end, Compare comp = Compare())
	{
		for (auto i = begin; i != end; ++i)
		{
			auto index = std::upper_bound(begin, i, *i, comp);
			std::rotate(index, i, i + 1);
		}
	};
}
