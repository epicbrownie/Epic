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
#include <cstdint>
#include <utility>
#include <type_traits>

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	namespace detail
	{
		template<class T> struct CanAllocate;
		template<class T> struct CanAllocateAligned;
		template<class T> struct CanReallocate;
		template<class T> struct CanReallocateAligned;
		template<class T> struct CanAllocateAll;
		template<class T> struct CanAllocateAllAligned;
		template<class T> struct CanDeallocate;
		template<class T> struct CanDeallocateAligned;
		template<class T> struct CanDeallocateAll;
	}
}

//////////////////////////////////////////////////////////////////////////////

/// CanAllocate<T>
template<class T>
struct Epic::detail::CanAllocate
{
private:
	template<class U> static auto HasAllocate(int)
		-> decltype(std::declval<U>().Allocate(size_t())) { };
	template<class> static auto HasAllocate(...) -> std::false_type { };

public:
	static constexpr bool value =
		std::is_same< decltype(HasAllocate<T>(0)), Blk >::value;
};
		

/// CanAllocateAligned<T>
template<class T>
struct Epic::detail::CanAllocateAligned
{
private:
	template<class U> static auto HasAllocateAligned(int)
		-> decltype(std::declval<U>().AllocateAligned(size_t(), size_t())) { };
	template<class> static auto HasAllocateAligned(...) -> std::false_type { };

public:
	static constexpr bool value =
		std::is_same< decltype(HasAllocateAligned<T>(0)), Blk >::value;
};


/// CanReallocate<T>
template<class T>
struct Epic::detail::CanReallocate
{
private:
	template<class U> static auto HasReallocate(int)
		-> decltype(std::declval<U>().Reallocate(Blk(), size_t())) { };
	template<class> static auto HasReallocate(...) -> std::false_type { };

public:
	static constexpr bool value =
		std::is_same< decltype(HasReallocate<T>(0)), bool >::value;
};


/// CanReallocateAligned<T>
template<class T>
struct Epic::detail::CanReallocateAligned
{
private:
	template<class U> static auto HasReallocateAligned(int)
		-> decltype(std::declval<U>().ReallocateAligned(Blk(), size_t(), size_t())) { };
	template<class> static auto HasReallocateAligned(...) -> std::false_type { };

public:
	static constexpr bool value =
		std::is_same< decltype(HasReallocateAligned<T>(0)), bool >::value;
};


/// CanAllocateAll<T>
template<class T>
struct Epic::detail::CanAllocateAll
{
private:
	template<class U> static auto HasAllocateAll(int)
		-> decltype(std::declval<U>().AllocateAll()) { };
	template<class> static auto HasAllocateAll(...) -> std::false_type { };

public:
	static constexpr bool value =
		std::is_same< decltype(HasAllocateAll<T>(0)), Blk >::value;
};


/// CanAllocateAllAligned<T>
template<class T>
struct Epic::detail::CanAllocateAllAligned
{
private:
	template<class U> static auto HasAllocateAllAligned(int)
		-> decltype(std::declval<U>().AllocateAllAligned(size_t())) { };
	template<class> static auto HasAllocateAllAligned(...) -> std::false_type { };

public:
	static constexpr bool value =
		std::is_same< decltype(HasAllocateAllAligned<T>(0)), Blk >::value;
};


/// CanDeallocate<T>
template<class T>
struct Epic::detail::CanDeallocate
{
private:
	template<class U> static auto HasDeallocate(int)
		-> decltype(std::declval<U>().Deallocate(Blk())) { };
	template<class> static auto HasDeallocate(...) -> std::false_type { };

public:
	static constexpr bool value =
		std::is_same< decltype(HasDeallocate<T>(0)), void >::value;
};


/// CanDeallocateAligned<T>
template<class T>
struct Epic::detail::CanDeallocateAligned
{
private:
	template<class U> static auto HasDeallocateAligned(int)
		-> decltype(std::declval<U>().DeallocateAligned(Blk())) { };
	template<class> static auto HasDeallocateAligned(...) -> std::false_type { };

public:
	static constexpr bool value =
		std::is_same< decltype(HasDeallocateAligned<T>(0)), void >::value;
};


/// CanDeallocateAll<T>
template<class T>
struct Epic::detail::CanDeallocateAll
{
private:
	template<class U> static auto HasDeallocateAll(int)
		-> decltype(std::declval<U>().DeallocateAll()) { };
	template<class> static auto HasDeallocateAll(...) -> std::false_type { };

public:
	static constexpr bool value = std::is_same< decltype(HasDeallocateAll<T>(0)), void >::value;
};
