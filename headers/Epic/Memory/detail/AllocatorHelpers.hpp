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
	constexpr bool IsGoodAlignment(const size_t alignment) noexcept;
	constexpr size_t RoundToAligned(size_t sz, size_t alignment) noexcept;
}

//////////////////////////////////////////////////////////////////////////////

namespace Epic::detail
{
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

namespace Epic::detail
{
	template<class Affix>
	struct AffixSize;

	template<class Affix>
	struct AffixConstructor;

	template<class Affix, bool Enabled = std::is_same<Affix, void>::value || std::is_move_constructible<Affix>::value>
	struct AffixBuffer;
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
	static inline Blk apply(const T&, size_t sz, size_t alignment) noexcept
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

	static inline Blk apply(const T& alloc, size_t sz, size_t alignment) noexcept
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
	static inline bool apply(const T&, Blk&, size_t, size_t)
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

	static inline bool apply(const T& alloc, Blk& blk, size_t sz, size_t alignment)
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
		return alloc.Allocate();
	}

	static inline Blk apply(const T& alloc) noexcept
	{
		return alloc.Allocate();
	}
};

template<class T, bool Enabled>
struct Epic::detail::AllocateAllAlignedIf
{
	static inline Blk apply(const T&, size_t alignment) noexcept
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

	static inline Blk apply(const T& alloc, size_t alignment) noexcept
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
	static_assert(Epic::detail::CanAllocate<T>::value, "Reallocator requires that the allocator can perform unaligned allocations.");

	static inline bool apply(T& alloc, Blk& blk, size_t sz)
	{
		auto newblk = alloc.Allocate(sz);
		if (!newblk) return false;

		if (blk)
		{
			std::memcpy(newblk.Ptr, blk.Ptr, std::min(sz, blk.Size));
			detail::DeallocateIf<T>::apply(alloc, blk);
		}

		blk = newblk;

		return true;
	}

	static inline bool apply(const T& alloc, Blk& blk, size_t sz)
	{
		auto newblk = alloc.Allocate(sz);
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
	static_assert(Epic::detail::CanAllocateAligned<T>::value, "AlignedReallocator requires that the allocator can perform aligned allocations.");

	static inline bool apply(T& alloc, Blk& blk, size_t sz, size_t alignment)
	{
		auto newblk = alloc.AllocateAligned(sz, alignment);
		if (!newblk) return false;

		if (blk)
		{
			std::memcpy(newblk.Ptr, blk.Ptr, std::min(sz, blk.Size));
			detail::DeallocateAlignedIf<T>::apply(alloc, blk);
		}

		blk = newblk;

		return true;
	}

	static inline bool apply(const T& alloc, Blk& blk, size_t sz, size_t alignment)
	{
		auto newblk = alloc.AllocateAligned(sz, alignment);
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

//////////////////////////////////////////////////////////////////////////////

template<class Affix>
struct Epic::detail::AffixSize
{
	static constexpr size_t value = sizeof(Affix);
};

template<>
struct Epic::detail::AffixSize<void>
{
	static constexpr size_t value = 0;
};

template<class Affix>
struct Epic::detail::AffixConstructor
{
	static void apply(void* pWhere)
		noexcept(std::is_nothrow_default_constructible<Affix>::value)
	{
		new (pWhere) Affix{ };
	}
};

template<>
struct Epic::detail::AffixConstructor<void>
{
	static void apply(void*) noexcept { }
};

template<class Affix>
struct Epic::detail::AffixBuffer<Affix, false>
{
	static constexpr bool CanStore = false;
};

template<class Affix>
struct Epic::detail::AffixBuffer<Affix, true>
{
	static constexpr bool CanStore = true;

	Affix _Buffer;

	AffixBuffer(Affix* pObj) noexcept
		: _Buffer(std::move(*pObj)) 
	{
		pObj->~Affix();
	}

	void Restore(void* pWhere) noexcept
	{
		new (pWhere) Affix{ std::move(_Buffer) };
	}
};

template<>
struct Epic::detail::AffixBuffer<void, true>
{
	static constexpr bool CanStore = true;

	AffixBuffer(void*) noexcept { }
	void Restore(void*) noexcept { }
};
