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
#include <Epic/Memory/AllocatorTraits.hpp>
#include <cstdint>
#include <cstddef>

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	namespace detail
	{
		static constexpr size_t DefaultAlignment = alignof(std::max_align_t);

		constexpr bool IsGoodAlignment(const size_t alignment) noexcept;
		constexpr size_t RoundToAligned(size_t sz, size_t alignment) noexcept;

		template<class T, bool Enabled = Epic::detail::CanAllocate<T>::value>
		struct AllocateIf;

		template<class T, bool Enabled = Epic::detail::CanAllocateAligned<T>::value>
		struct AllocateAlignedIf;

		template<class T, bool Enabled = Epic::detail::CanReallocate<T>::value>
		struct ReallocateIf;

		template<class T, bool Enabled = Epic::detail::CanReallocateAligned<T>::value>
		struct ReallocateAlignedIf;

		template<class T, bool Enabled = Epic::detail::CanAllocateAll<T>::value>
		struct AllocateAllIf;

		template<class T, bool Enabled = Epic::detail::CanAllocateAllAligned<T>::value>
		struct AllocateAllAlignedIf;

		template<class T, bool Enabled = Epic::detail::CanDeallocate<T>::value>
		struct DeallocateIf;

		template<class T, bool Enabled = Epic::detail::CanDeallocateAligned<T>::value>
		struct DeallocateAlignedIf;

		template<class T, bool Enabled = Epic::detail::CanDeallocateAll<T>::value>
		struct DeallocateAllIf;
	}
}

//////////////////////////////////////////////////////////////////////////////

constexpr bool Epic::detail::IsGoodAlignment(const size_t alignment) noexcept
{
	return !(alignment == 0) &&				// alignment must be non-zero
		   !(alignment & (alignment - 1));	// alignment must be a power-of-two
}

constexpr size_t Epic::detail::RoundToAligned(size_t sz, size_t alignment) noexcept
{
	// Round sz up to the nearest multiple of alignment
	return ((sz + alignment - 1) / alignment) * alignment;
}

//////////////////////////////////////////////////////////////////////////////

template<class T, bool Enabled>
struct Epic::detail::AllocateIf
{
	static inline Blk apply(T&, size_t sz) noexcept 
	{ 
		return{ nullptr, 0 }; 
	}
};

template<class T>
struct Epic::detail::AllocateIf<T, true>
{
	static inline Blk apply(T& alloc, size_t sz) noexcept
	{
		return alloc.Allocate(sz);
	}
};

template<class T, bool Enabled>
struct Epic::detail::AllocateAlignedIf
{
	static inline Blk apply(T&, size_t sz, size_t alignment) noexcept
	{
		return{ nullptr, 0 };
	}
};

template<class T>
struct Epic::detail::AllocateAlignedIf<T, true>
{
	static inline Blk apply(T& alloc, size_t sz, size_t alignment) noexcept
	{
		return alloc.AllocateAligned(sz, alignment);
	}
};

template<class T, bool Enabled>
struct Epic::detail::ReallocateIf
{
	static inline bool apply(T&, Blk&, size_t)
	{ 
		return false; 
	}
};

template<class T>
struct Epic::detail::ReallocateIf<T, true>
{
	static inline bool apply(T& alloc, Blk& blk, size_t sz) 
	{
		return alloc.Reallocate(blk, sz); 
	}
};

template<class T, bool Enabled>
struct Epic::detail::ReallocateAlignedIf
{
	static inline bool apply(T&, Blk&, size_t, size_t) 
	{ 
		return false; 
	}
};

template<class T>
struct Epic::detail::ReallocateAlignedIf<T, true>
{
	static inline bool apply(T& alloc, Blk& blk, size_t sz, size_t alignment) 
	{ 
		return alloc.ReallocateAligned(blk, sz, alignment); 
	}
};

template<class T, bool Enabled>
struct Epic::detail::AllocateAllIf
{
	static inline Blk apply(T&) noexcept
	{
		return{ nullptr, 0 };
	}
};

template<class T>
struct Epic::detail::AllocateAllIf<T, true>
{
	static inline Blk apply(T& alloc) noexcept
	{
		return alloc.Allocate();
	}
};

template<class T, bool Enabled>
struct Epic::detail::AllocateAllAlignedIf
{
	static inline Blk apply(T&, size_t alignment) noexcept
	{
		return{ nullptr, 0 };
	}
};

template<class T>
struct Epic::detail::AllocateAllAlignedIf<T, true>
{
	static inline Blk apply(T& alloc, size_t alignment) noexcept
	{
		return alloc.AllocateAllAligned(alignment);
	}
};

template<class T, bool Enabled>
struct Epic::detail::DeallocateIf
{
	static inline bool apply(T&, Blk&) 
	{ 
		return false; 
	}
};

template<class T>
struct Epic::detail::DeallocateIf<T, true>
{
	static inline bool apply(T& alloc, Blk& blk) 
	{ 
		return alloc.Deallocate(blk); 
	}
};

template<class T, bool Enabled>
struct Epic::detail::DeallocateAlignedIf
{
	static inline bool apply(T&, Blk&)
	{
		return false;
	}
};

template<class T>
struct Epic::detail::DeallocateAlignedIf<T, true>
{
	static inline bool apply(T& alloc, Blk& blk)
	{
		return alloc.DeallocateAligned(blk);
	}
};

template<class T, bool Enabled>
struct Epic::detail::DeallocateAllIf
{
	static inline bool apply(T&) noexcept
	{
		return false;
	}
};

template<class T>
struct Epic::detail::DeallocateAllIf<T, true>
{
	static inline bool apply(T& alloc) noexcept
	{
		return alloc.DeallocateAll();
	}
};
