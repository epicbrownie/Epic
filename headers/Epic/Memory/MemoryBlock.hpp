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
	struct MemoryBlock;
}

//////////////////////////////////////////////////////////////////////////////

/// MemoryBlock
struct Epic::MemoryBlock
{
	void* Ptr;
	size_t Size;

	// C'tors
	constexpr MemoryBlock() noexcept 
		: Ptr{ nullptr }, Size{ 0 }  { }

	constexpr MemoryBlock(void* ptr, size_t sz) noexcept
		: Ptr{ ptr }, Size{ sz }  { }

	// Copy C'tor
	constexpr MemoryBlock(const MemoryBlock& o) noexcept 
		= default;

	// Move C'tor
	MemoryBlock(MemoryBlock&& o) noexcept
		: Ptr{ o.Ptr }, Size{ o.Size }
	{ o.Reset(); }

	// D'tor
	~MemoryBlock() noexcept = default;

	// Assignment Ops
	MemoryBlock& operator = (const MemoryBlock& o) noexcept
		= default;

	MemoryBlock& operator = (MemoryBlock&& o) noexcept
	{
		Ptr = o.Ptr;
		Size = o.Size;
		o.Reset();
		return *this;
	}

	// Bool conversion
	explicit constexpr operator bool() const 
	{ 
		return (Ptr != nullptr) && (Size != 0); 
	}

	// Comparison Ops
	constexpr bool operator == (const MemoryBlock& o) const noexcept 
	{ 
		return (Ptr == o.Ptr) && (Size == o.Size); 
	}
	
	constexpr bool operator < (const MemoryBlock& o) const noexcept 
	{ 
		return Ptr < o.Ptr; 
	}

	// Clear Fields
	void Reset() noexcept
	{
		Ptr = nullptr;
		Size = 0;
	}
};

//////////////////////////////////////////////////////////////////////////////

/// Aliases
namespace Epic
{
	using Blk = MemoryBlock;
}