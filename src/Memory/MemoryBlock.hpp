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

#include <utility>

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	struct MemoryBlock;
}

//////////////////////////////////////////////////////////////////////////////

/// MemoryBlock
struct Epic::MemoryBlock
{
	using pointer = void*;
	using size_type = size_t;

	pointer Ptr = nullptr;
	size_type Size = 0;

	constexpr MemoryBlock() noexcept = default;
	constexpr MemoryBlock(pointer ptr, size_type sz) noexcept : Ptr{ ptr }, Size{ sz }  { }
	constexpr MemoryBlock(const MemoryBlock&) noexcept = default;

	MemoryBlock(MemoryBlock&& o) noexcept 
		: Ptr{ nullptr }, Size{ 0 } 
	{ 
		std::swap(Ptr, o.Ptr);
		std::swap(Size, o.Size);
	}

	~MemoryBlock() noexcept = default;

	MemoryBlock& operator = (const MemoryBlock&) noexcept = default;

	MemoryBlock& operator = (MemoryBlock&& o) noexcept
	{
		std::swap(Ptr, o.Ptr);
		std::swap(Size, o.Size);

		return *this;
	}

	explicit constexpr operator bool() const 
	{ 
		return (Ptr != nullptr) && (Size != 0); 
	}

	constexpr bool operator == (const MemoryBlock& o) const noexcept 
	{ 
		return (Ptr == o.Ptr) && (Size == o.Size); 
	}
	
	constexpr bool operator < (const MemoryBlock& o) const noexcept 
	{ 
		return Ptr < o.Ptr; 
	}
};

//////////////////////////////////////////////////////////////////////////////

/// Aliases
namespace Epic
{
	using Blk = MemoryBlock;
}
