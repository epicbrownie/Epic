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

#include <Epic/detail/ReadConfig.hpp>
#include <Epic/Memory/GlobalAllocator.hpp>
#include <Epic/Memory/AffixAllocator.hpp>
#include <Epic/Memory/ForceAlignAllocator.hpp>
#include <Epic/Memory/AlignedMallocator.hpp>
#include <Epic/Memory/MemoryBlock.hpp>
#include <Epic/Memory/detail/AllocatorHelpers.hpp>
#include <type_traits>

//////////////////////////////////////////////////////////////////////////////

namespace Epic::detail
{
	template<class A, class Tag>
	struct AudA;

	struct AudPre;

	template<class Allocator, class Tag = detail::GlobalAllocatorTag>
	struct AllocAdapted;

	static constexpr size_t AudioAlignment = 16;
}

//////////////////////////////////////////////////////////////////////////////

struct Epic::detail::AudPre
{
	Epic::MemoryBlock::size_type Size;
};

//////////////////////////////////////////////////////////////////////////////

template<class A, class Tag>
struct Epic::detail::AudA
{
	using _aligned = Epic::ForceAlignAllocator<A, AudioAlignment>;
	using _affixed = Epic::AffixAllocator<_aligned, Epic::detail::AudPre>;

	using type = Epic::GlobalAllocator<_affixed, Tag>;
};

template<class A, class Tag, class OldTag>
struct Epic::detail::AudA<Epic::detail::GlobalAllocatorImpl<A, OldTag>, Tag>
{
	using _unwrapped = typename detail::UnwrapGlobal<A>::type;
	using _aligned = Epic::ForceAlignAllocator<_unwrapped, AudioAlignment>;
	using _affixed = Epic::AffixAllocator<_aligned, Epic::detail::AudPre>;

	using type = Epic::GlobalAllocator<_affixed, OldTag>;
};

//////////////////////////////////////////////////////////////////////////////

template<class A, class Tag>
struct Epic::detail::AllocAdapted : public Epic::detail::AudA<A, Tag>::type
{
	using Base = typename Epic::detail::AudA<A, Tag>::type;

	using Base::Base;
};

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	namespace detail
	{
		using AudioAllocatorPrefix = AudPre;

		template<class Tag = Epic::detail::GlobalAllocatorTag>
		using AudioAllocator =
			std::conditional_t<
				std::is_same<void, typename detail::GetConfigProperty<detail::eConfigProperty::AudioAllocator>::type>::value,
				Epic::detail::AllocAdapted<Epic::AlignedMallocator, Tag>,
				Epic::detail::AllocAdapted<typename detail::GetConfigProperty<detail::eConfigProperty::DefaultAllocator>::type, Tag>>;
	}
}
