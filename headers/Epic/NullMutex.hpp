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

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	struct NullMutex
};

//////////////////////////////////////////////////////////////////////////////

// NullMutex
struct Epic::NullMutex
{
	void lock() noexcept { }
	bool try_lock() noexcept { return true; }
	void unlock() noexcept { }
};
