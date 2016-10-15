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
	Epic::MemoryBlock::SizeType AlignPad;

	static inline void Set(const Blk& blk, const Epic::MemoryBlock::SizeType& value) noexcept
	{
		auto pSuffix = reinterpret_cast<ForceAlignSuffix*>(reinterpret_cast<char*>(blk.Ptr) + blk.Size - sizeof(ForceAlignSuffix));
		pSuffix->AlignPad = value;
	}

	static inline auto Get(const Blk& blk) noexcept
	{
		const auto pSuffix = reinterpret_cast<const ForceAlignSuffix*>(reinterpret_cast<const char*>(blk.Ptr) + blk.Size);
		return pSuffix->AlignPad;
	}
};
