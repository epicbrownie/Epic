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

#include <Epic/Memory/AffixAllocator.hpp>
#include <Epic/Memory/PatternGuard.hpp>
#include <Epic/TMP/Utility.hpp>
#include <type_traits>

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	enum class GuardMode;

	namespace detail
	{
		template<class A, GuardMode Mode, size_t Pattern, class DebugPolicy, class ReleasePolicy>
		struct CorruptionGuardInvoker;
	}
}

//////////////////////////////////////////////////////////////////////////////

enum class Epic::GuardMode 
{
	Before, After, Both
};

//////////////////////////////////////////////////////////////////////////////

template<class A, size_t Pattern, class DebugPolicy, class ReleasePolicy>
struct Epic::detail::CorruptionGuardInvoker<A, Epic::GuardMode::Before, Pattern, DebugPolicy, ReleasePolicy>
{
	using type = Epic::AffixAllocator<A, Epic::PatternGuard<Pattern, DebugPolicy, ReleasePolicy>>;
};

template<class A, size_t Pattern, class DebugPolicy, class ReleasePolicy>
struct Epic::detail::CorruptionGuardInvoker<A, Epic::GuardMode::After, Pattern, DebugPolicy, ReleasePolicy>
{
	using type = Epic::AffixAllocator<A, void, Epic::PatternGuard<Pattern, DebugPolicy, ReleasePolicy>>;
};

template<class A, size_t Pattern, class DebugPolicy, class ReleasePolicy>
struct Epic::detail::CorruptionGuardInvoker<A, Epic::GuardMode::Both, Pattern, DebugPolicy, ReleasePolicy>
{
	using type = Epic::AffixAllocator<A,
		Epic::PatternGuard<Pattern, DebugPolicy, ReleasePolicy>, 
		Epic::PatternGuard<Pattern, DebugPolicy, ReleasePolicy>>;
};

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	template<class Allocator,
		Epic::GuardMode Mode = Epic::GuardMode::After,
		class DebugResponsePolicy = Epic::GuardResponseAssert,
		class ReleaseResponsePolicy = Epic::GuardResponseCErr,
		size_t Pattern = Epic::DefaultGuardPattern::value>
	using CorruptionGuardedAllocator = typename detail::CorruptionGuardInvoker<Allocator, Mode, Pattern, DebugResponsePolicy, ReleaseResponsePolicy>::type;
}
