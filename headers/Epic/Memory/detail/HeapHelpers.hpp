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

#include <cassert>
#include <cstdint>

//////////////////////////////////////////////////////////////////////////////

namespace Epic::detail
{
	template<size_t BitCount>
	struct HeapBitmap;

	template<typename T>
	struct HeapBitmapMask;
}

//////////////////////////////////////////////////////////////////////////////
	
template<>
struct Epic::detail::HeapBitmapMask<uint32_t>
{
	static constexpr uint32_t Masks[] =
	{
		0x00000000, 0x00000001, 0x00000003, 0x00000007,
		0x0000000F, 0x0000001F, 0x0000003F, 0x0000007F,
		0x000000FF, 0x000001FF, 0x000003FF, 0x000007FF,
		0x00000FFF, 0x00001FFF, 0x00003FFF, 0x00007FFF,
		0x0000FFFF, 0x0001FFFF, 0x0003FFFF, 0x0007FFFF,
		0x000FFFFF, 0x001FFFFF, 0x003FFFFF, 0x007FFFFF,
		0x00FFFFFF, 0x01FFFFFF, 0x03FFFFFF, 0x07FFFFFF,
		0x0FFFFFFF, 0x1FFFFFFF, 0x3FFFFFFF, 0x7FFFFFFF
	};
};

template<>
struct Epic::detail::HeapBitmapMask<uint64_t>
{
	static constexpr uint64_t Masks[] =
	{
		0x0000000000000000, 0x0000000000000001, 0x0000000000000003, 0x0000000000000007,
		0x000000000000000F, 0x000000000000001F, 0x000000000000003F, 0x000000000000007F,
		0x00000000000000FF, 0x00000000000001FF, 0x00000000000003FF, 0x00000000000007FF,
		0x0000000000000FFF, 0x0000000000001FFF, 0x0000000000003FFF, 0x0000000000007FFF,
		0x000000000000FFFF, 0x000000000001FFFF, 0x000000000003FFFF, 0x000000000007FFFF,
		0x00000000000FFFFF, 0x00000000001FFFFF, 0x00000000003FFFFF, 0x00000000007FFFFF,
		0x0000000000FFFFFF, 0x0000000001FFFFFF, 0x0000000003FFFFFF, 0x0000000007FFFFFF,
		0x000000000FFFFFFF, 0x000000001FFFFFFF, 0x000000003FFFFFFF, 0x000000007FFFFFFF,
		0x00000000FFFFFFFF, 0x00000001FFFFFFFF, 0x00000003FFFFFFFF, 0x00000007FFFFFFFF,
		0x0000000FFFFFFFFF, 0x0000001FFFFFFFFF, 0x0000003FFFFFFFFF, 0x0000007FFFFFFFFF,
		0x000000FFFFFFFFFF, 0x000001FFFFFFFFFF, 0x000003FFFFFFFFFF, 0x000007FFFFFFFFFF,
		0x00000FFFFFFFFFFF, 0x00001FFFFFFFFFFF, 0x00003FFFFFFFFFFF, 0x00007FFFFFFFFFFF,
		0x0000FFFFFFFFFFFF, 0x0001FFFFFFFFFFFF, 0x0003FFFFFFFFFFFF, 0x0007FFFFFFFFFFFF,
		0x000FFFFFFFFFFFFF, 0x001FFFFFFFFFFFFF, 0x003FFFFFFFFFFFFF, 0x007FFFFFFFFFFFFF,
		0x00FFFFFFFFFFFFFF, 0x01FFFFFFFFFFFFFF, 0x03FFFFFFFFFFFFFF, 0x07FFFFFFFFFFFFFF,
		0x0FFFFFFFFFFFFFFF, 0x1FFFFFFFFFFFFFFF, 0x3FFFFFFFFFFFFFFF, 0x7FFFFFFFFFFFFFFF
	};
};

//////////////////////////////////////////////////////////////////////////////

template<size_t BitCount>
struct Epic::detail::HeapBitmap
{
	static_assert(BitCount > 0, "HeapBitmap: BitCount cannot be zero.");

	using StorageType = size_t;

	static constexpr size_t Entries = BitCount;
	static constexpr size_t BitsPerBlock = sizeof(StorageType) * CHAR_BIT;
	static constexpr size_t BlockCount = (BitCount + BitsPerBlock - 1) / BitsPerBlock;

	static constexpr StorageType AllZero = 0;
	static constexpr StorageType AllOne = ~AllZero;

	StorageType Blocks[BlockCount];

	// Reset all bits to 0
	inline void Reset() noexcept
	{
		for (size_t i = 0; i < BlockCount; ++i)
			Blocks[i] = AllZero;
	}

	// Set bit at location to 0
	inline void Unset(const size_t location) noexcept
	{
		Set(location, false);
	}

	// Set bits from start to start+count to 0
	inline void Unset(const size_t start, const size_t count) noexcept
	{
		Set(start, count, false);
	}

	// Set bit at location to value
	void Set(const size_t location, const bool value = true) noexcept
	{
		const size_t block = start / BitsPerBlock;
		const size_t lrem = start % BitsPerBlock;
		const size_t urem = (start + count) % BitsPerBlock;

		assert(block < BlockCount);
		
		if (value)
			Blocks[block] |= ~HeapBitmapMask<StorageType>::Masks[lrem] & HeapBitmapMask<StorageType>::Masks[urem];
		else
			Blocks[blocks] &= HeapBitmapMask<StorageType>::Masks[lrem] | ~HeapBitmapMask<StorageType>::Masks[urem];
	}

	// Set bits from start to start+count to value
	void Set(const size_t start, const size_t count, const bool value = true) noexcept
	{
		const size_t lbound = start / BitsPerBlock;
		const size_t ubound = (start + count) / BitsPerBlock;
		const size_t lrem = start % BitsPerBlock;
		const size_t urem = (start + count) % BitsPerBlock;

		assert(lbound < BlockCount);
		assert(ubound <= BlockCount);

		if (lbound == ubound)
		{
			// Spans a single block
			if (value)
				Blocks[lbound] |= ~HeapBitmapMask<StorageType>::Masks[lrem] & HeapBitmapMask<StorageType>::Masks[urem];
			else
				Blocks[lbound] &= HeapBitmapMask<StorageType>::Masks[lrem] | ~HeapBitmapMask<StorageType>::Masks[urem];
		}
		else
		{
			// Spans multiple block
			if (value)
			{
				Blocks[lbound] |= ~HeapBitmapMask<StorageType>::Masks[lrem];
				if (urem > 0) Blocks[ubound] |= HeapBitmapMask<StorageType>::Masks[urem];

				for (size_t i = lbound + 1; i < ubound; ++i)
					Blocks[i] = AllOne;
			}
			else
			{
				Blocks[lbound] &= HeapBitmapMask<StorageType>::Masks[lrem];
				if (urem > 0) Blocks[ubound] &= ~HeapBitmapMask<StorageType>::Masks[urem];

				for (size_t i = lbound + 1; i < ubound; ++i)
					Blocks[i] = 0;
			}
		}
	}

	// Find the first bit where 'length' bits are contiguously free.
	// The search will begin at the 'start' bit.
	size_t FindAvailable(const size_t length) const noexcept
	{
		size_t LBlock = 0;
		size_t UBlock = length / BitsPerBlock;

		size_t LMask = (LBlock == UBlock) ? ~HeapBitmapMask<StorageType>::Masks[length] : AllZero;
		size_t UMask = ~HeapBitmapMask<StorageType>::Masks[length % BitsPerBlock];
		
		const size_t last = Entries - length + 1;

		for (size_t i = 0; i < (Entries - length + 1); ++i)
		{
			bool good = true;

			// Whole block checks
			for (size_t j = LBlock + 1; j < UBlock; ++j)
			{
				if (Blocks[j] != 0)
				{
					good = false;
					break;
				}
			}

			// Edge block checks
			if (LBlock == UBlock)
			{
				size_t CMask = LMask | UMask;
				good &= ((CMask | Blocks[LBlock]) == CMask);
			}
			else
			{
				good &= ((LMask | Blocks[LBlock]) == LMask);
				good &= ((UMask | Blocks[UBlock]) == UMask);
			}

			// If all is well, free space has been found
			if (good) return i;

			// Update edge masks
			LMask = (LMask << 1) | 1;
			UMask = UMask << 1;

			// Check for edge block shifting
			if (LMask == AllOne)
			{
				LMask = AllZero;
				++LBlock;
			}

			if (UMask == AllZero)
			{
				UMask = AllOne;
				++UBlock;
			}
		}
		
		// Failed to locate an available span
		return Entries;
	}

	// Test whether or not 'count' bits are contiguously free from location 'start'
	bool HasAvailable(const size_t start, const size_t count) const noexcept
	{
		const size_t lbound = start / BitsPerBlock;
		const size_t ubound = (start + count) / BitsPerBlock;
		const size_t urem = (start + count) % BitsPerBlock;

		if (lbound >= BlockCount || ubound > BlockCount || (ubound == BlockCount && urem != 0))
			return false;

		const size_t lmask = HeapBitmapMask<StorageType>::Masks[start % BitsPerBlock];
		const size_t umask = HeapBitmapMask<StorageType>::Masks[urem];
		
		if (lbound == ubound)
		{
			// Spans a single block
			return (Blocks[lbound] & ~(~lmask ^ umask)) == 0;
		}
		else
		{
			// Spans multiple block
			for (size_t i = lbound + 1; i < ubound; ++i)
			{
				if (Blocks[i] != AllZero)
					return false;
			}

			if ((Blocks[lbound] & ~lmask) != AllZero)
				return false;

			if ((urem > 0) && ((Blocks[ubound] & umask) != AllZero))
				return false;
		}

		return true;
	}
};
