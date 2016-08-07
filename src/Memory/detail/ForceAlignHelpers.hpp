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

#include <Epic/Memory/MemoryBlock.hpp>

//////////////////////////////////////////////////////////////////////////////

namespace Epic::detail
{
	struct ForceAlignSuffix;
}

//////////////////////////////////////////////////////////////////////////////

struct Epic::detail::ForceAlignSuffix
{
	Epic::MemoryBlock::size_type AlignPad;

	static void Set(const Blk& blk, const Epic::MemoryBlock::size_type& value) noexcept
	{
		auto pSuffix = reinterpret_cast<ForceAlignSuffix*>(reinterpret_cast<char*>(blk.Ptr) + blk.Size - sizeof(ForceAlignSuffix));
		pSuffix->AlignPad = value;
	}

	static Epic::MemoryBlock::size_type Get(const Blk& blk) noexcept
	{
		auto pSuffix = reinterpret_cast<ForceAlignSuffix*>(reinterpret_cast<char*>(blk.Ptr) + blk.Size);
		return pSuffix->AlignPad;
	}
};
