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

	static void Set(const Blk& blk, Epic::MemoryBlock::size_type value) noexcept
	{
		reinterpret_cast<ForceAlignSuffix*>(
			reinterpret_cast<unsigned char*>(blk.Ptr) + blk.Size - sizeof(ForceAlignSuffix)
		)->AlignPad = value;
	}

	static auto Get(const Blk& blk) noexcept
	{
		return reinterpret_cast<const ForceAlignSuffix*>(
			reinterpret_cast<const unsigned char*>(blk.Ptr) + blk.Size
		)->AlignPad;
	}
};
