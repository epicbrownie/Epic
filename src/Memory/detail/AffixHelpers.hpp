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

#include <cstdint>
#include <type_traits>
#include <utility>

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
		::new (pWhere) Affix{ };
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

	explicit AffixBuffer(Affix* pObj) noexcept
		: _Buffer(std::move(*pObj))
	{
		pObj->~Affix();
	}

	void Restore(void* pWhere) noexcept
	{
		::new (pWhere) Affix{ std::move(_Buffer) };
	}
};

template<>
struct Epic::detail::AffixBuffer<void, true>
{
	static constexpr bool CanStore = true;

	explicit AffixBuffer(void*) noexcept { }
	void Restore(void*) noexcept { }
};
