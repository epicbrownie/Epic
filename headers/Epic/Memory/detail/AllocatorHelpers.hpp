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
#include <Epic/Memory/detail/AllocatorTraits.hpp>
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <algorithm>
#include <type_traits>

//////////////////////////////////////////////////////////////////////////////

namespace Epic::detail
{
	static constexpr size_t DefaultAlignment = alignof(std::max_align_t);
}

//////////////////////////////////////////////////////////////////////////////

namespace Epic::detail
{
	constexpr bool IsGoodAlignment(const size_t alignment) noexcept
	{
		return !(alignment == 0) &&				// alignment must be non-zero
			   !(alignment & (alignment - 1));	// alignment must be a power-of-two
	}

	constexpr size_t RoundToAligned(const size_t sz, const size_t alignment) noexcept
	{
		// Round sz up to the nearest multiple of alignment
		return ((sz + alignment - 1) / alignment) * alignment;
	}
}

//////////////////////////////////////////////////////////////////////////////

namespace Epic::detail
{
	//	TODO: 'if constexpr' should eventually remove MOST uses of these.
	
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

//////////////////////////////////////////////////////////////////////////////

namespace Epic::detail
{
	template<class T>
	struct Reallocator;

	template<class T>
	struct AlignedReallocator;
}

//////////////////////////////////////////////////////////////////////////////

template<class T, bool Enabled>
struct Epic::detail::AllocateIf
{
	static inline Blk apply(const T&, size_t sz) noexcept 
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

	static inline Blk apply(const T& alloc, size_t sz) noexcept
	{
		return alloc.Allocate(sz);
	}
};

template<class T, bool Enabled>
struct Epic::detail::AllocateAlignedIf
{
	static inline Blk apply(const T&, size_t sz, size_t alignment = T::Alignment) noexcept
	{
		return{ nullptr, 0 };
	}
};

template<class T>
struct Epic::detail::AllocateAlignedIf<T, true>
{
	static inline Blk apply(T& alloc, size_t sz, size_t alignment = T::Alignment) noexcept
	{
		return alloc.AllocateAligned(sz, alignment);
	}

	static inline Blk apply(const T& alloc, size_t sz, size_t alignment = T::Alignment) noexcept
	{
		return alloc.AllocateAligned(sz, alignment);
	}
};

template<class T, bool Enabled>
struct Epic::detail::ReallocateIf
{
	static inline bool apply(const T&, Blk&, size_t)
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

	static inline bool apply(const T& alloc, Blk& blk, size_t sz)
	{
		return alloc.Reallocate(blk, sz);
	}
};

template<class T, bool Enabled>
struct Epic::detail::ReallocateAlignedIf
{
	static inline bool apply(const T&, Blk&, size_t, size_t = T::Alignment)
	{
		return false;
	}
};

template<class T>
struct Epic::detail::ReallocateAlignedIf<T, true>
{
	static inline bool apply(T& alloc, Blk& blk, size_t sz, size_t alignment = T::Alignment)
	{
		return alloc.ReallocateAligned(blk, sz, alignment);
	}

	static inline bool apply(const T& alloc, Blk& blk, size_t sz, size_t alignment = T::Alignment)
	{
		return alloc.ReallocateAligned(blk, sz, alignment);
	}
};

template<class T, bool Enabled>
struct Epic::detail::AllocateAllIf
{
	static inline Blk apply(const T&) noexcept
	{
		return{ nullptr, 0 };
	}
};

template<class T>
struct Epic::detail::AllocateAllIf<T, true>
{
	static inline Blk apply(T& alloc) noexcept
	{
		return alloc.AllocateAll();
	}

	static inline Blk apply(const T& alloc) noexcept
	{
		return alloc.AllocateAll();
	}
};

template<class T, bool Enabled>
struct Epic::detail::AllocateAllAlignedIf
{
	static inline Blk apply(const T&, size_t alignment = T::Alignment) noexcept
	{
		return{ nullptr, 0 };
	}
};

template<class T>
struct Epic::detail::AllocateAllAlignedIf<T, true>
{
	static inline Blk apply(T& alloc, size_t alignment = T::Alignment) noexcept
	{
		return alloc.AllocateAllAligned(alignment);
	}

	static inline Blk apply(const T& alloc, size_t alignment = T::Alignment) noexcept
	{
		return alloc.AllocateAllAligned(alignment);
	}
};

template<class T, bool Enabled>
struct Epic::detail::DeallocateIf
{
	static inline void apply(const T&, const Blk&) 
	{ 

	}
};

template<class T>
struct Epic::detail::DeallocateIf<T, true>
{
	static inline void apply(T& alloc, const Blk& blk) 
	{ 
		alloc.Deallocate(blk); 
	}

	static inline void apply(const T& alloc, const Blk& blk)
	{
		alloc.Deallocate(blk);
	}
};

template<class T, bool Enabled>
struct Epic::detail::DeallocateAlignedIf
{
	static inline void apply(const T&, const Blk&)
	{

	}
};

template<class T>
struct Epic::detail::DeallocateAlignedIf<T, true>
{
	static inline void apply(T& alloc, const Blk& blk)
	{
		alloc.DeallocateAligned(blk);
	}

	static inline void apply(const T& alloc, const Blk& blk)
	{
		alloc.DeallocateAligned(blk);
	}
};

template<class T, bool Enabled>
struct Epic::detail::DeallocateAllIf
{
	static inline void apply(const T&) noexcept
	{

	}
};

template<class T>
struct Epic::detail::DeallocateAllIf<T, true>
{
	static inline void apply(T& alloc) noexcept
	{
		alloc.DeallocateAll();
	}

	static inline void apply(const T& alloc) noexcept
	{
		alloc.DeallocateAll();
	}
};

//////////////////////////////////////////////////////////////////////////////

template<class T>
struct Epic::detail::Reallocator
{
	static inline bool ReallocateViaCopy(T& alloc, Blk& __restrict blk, const size_t sz)
	{
		auto newblk = detail::AllocateIf<T>::apply(alloc, sz);
		if (!newblk) return false;

		if (blk)
		{
			std::memcpy(newblk.Ptr, blk.Ptr, std::min(sz, blk.Size));
			detail::DeallocateIf<T>::apply(alloc, blk);
		}

		blk = newblk;

		return true;
	}

	static inline bool ReallocateViaCopy(const T& alloc, Blk& __restrict blk, const size_t sz)
	{
		auto newblk = detail::AllocateIf<T>::apply(alloc, sz);
		if (!newblk) return false;

		if (blk)
		{
			std::memcpy(newblk.Ptr, blk.Ptr, std::min(sz, blk.Size));
			detail::DeallocateIf<T>::apply(alloc, blk);
		}

		blk = newblk;

		return true;
	}
};

template<class T>
struct Epic::detail::AlignedReallocator
{
	static inline bool ReallocateViaCopy(T& alloc, Blk& __restrict blk, const size_t sz, const size_t alignment = T::Alignment)
	{
		auto newblk = detail::AllocateAlignedIf<T>::apply(alloc, sz, alignment);
		if (!newblk) return false;

		if (blk)
		{
			std::memcpy(newblk.Ptr, blk.Ptr, std::min(sz, blk.Size));
			detail::DeallocateAlignedIf<T>::apply(alloc, blk);
		}

		blk = newblk;

		return true;
	}

	static inline bool ReallocateViaCopy(const T& alloc, Blk& __restrict blk, const size_t sz, const size_t alignment = T::Alignment)
	{
		auto newblk = detail::AllocateAlignedIf<T>::apply(alloc, sz, alignment);
		if (!newblk) return false;

		if (blk)
		{
			std::memcpy(newblk.Ptr, blk.Ptr, std::min(sz, blk.Size));
			detail::DeallocateAlignedIf<T>::apply(alloc, blk);
		}

		blk = newblk;

		return true;
	}
};
