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

#include <type_traits>

//////////////////////////////////////////////////////////////////////////////

// Sequence<T>
namespace Epic::TMP
{
	template<typename T, T... Items>
	using Sequence = std::integer_sequence<T, Items...>;

	template<typename T, T N>
	using MakeSequence = std::make_integer_sequence<T, N>;
}

//////////////////////////////////////////////////////////////////////////////

// ForEach - Iterate over a Sequence<T>
namespace Epic::TMP
{
	template<class Seq>
	struct ForEach;

	template<typename T, T Item, T... Items>
	struct ForEach<Sequence<T, Item, Items...>>
	{
		template<class Function, class... Args>
		static void Apply(Function fn, Args&&... args)
		{
			fn(Item, std::forward<Args>(args)...);

			ForEach<Sequence<T, Items...>>
				::Apply(fn, std::forward<Args>(args)...);
		}
	};

	template<typename T, T Item>
	struct ForEach<Sequence<T, Item>>
	{
		template<class Function, class... Args>
		static void Apply(Function fn, Args&&... args)
		{
			fn(Item, std::forward<Args>(args)...);
		}
	};
}

//////////////////////////////////////////////////////////////////////////////

// ForEach2 - Iterate over 2 Sequence<T> simultaneously
namespace Epic::TMP
{
	template<class Seq1, class Seq2>
	struct ForEach2;

	template<typename T, T Item1, T Item2, T... Items1, T... Items2>
	struct ForEach2<Sequence<T, Item1, Items1...>, Sequence<T, Item2, Items2...>>
	{
		template<class Function, class... Args>
		static void Apply(Function fn, Args&&... args)
		{
			fn(Item1, Item2, std::forward<Args>(args)...);

			ForEach2<Sequence<T, Items1...>, Sequence<T, Items2...>>
				::Apply(fn, std::forward<Args>(args)...);
		}
	};

	template<typename T, T Item1, T Item2>
	struct ForEach2<Sequence<T, Item1>, Sequence<T, Item2>>
	{
		template<class Function, class... Args>
		static void Apply(Function fn, Args&&... args)
		{
			fn(Item1, Item2, std::forward<Args>(args)...);
		}
	};

	template<typename T, T... ExtraItems>
	struct ForEach2<Sequence<T, ExtraItems...>, Sequence<T>>
	{
		template<class Function, class... Args>
		static void Apply(Function, Args&&...)
		{
			/* The first sequence had too many items. */
		}
	};

	template<typename T, T... ExtraItems>
	struct ForEach2<Sequence<T>, Sequence<T, ExtraItems...>>
	{
		template<class Function, class... Args>
		static void Apply(Function, Args&&...)
		{
			/* The second sequence had too many items. */
		}
	};
}

//////////////////////////////////////////////////////////////////////////////

// SequenceContains - Determine whether Query is contained within a Sequence<T>
namespace Epic::TMP
{
	template<typename T, T Query, class Seq>
	struct SequenceContains : public std::false_type 
	{ };

	template<typename T, T Query, T Item, T... Items>
	struct SequenceContains<T, Query, Sequence<T, Item, Items...>>
		: public std::conditional_t<
			((Query == Item) || SequenceContains<T, Query, Sequence<T, Items...>>::value),
			std::true_type,
			std::false_type>
	{ };
}

//////////////////////////////////////////////////////////////////////////////

// IsSequenceUnique - Determine whether every T within a Sequence<T> is unique
namespace Epic::TMP
{
	template<class Seq>
	struct IsSequenceUnique : public std::true_type 
	{ };

	template<typename T, T Item, T... Items>
	struct IsSequenceUnique<Sequence<T, Item, Items...>>
		: public std::conditional_t<
			(!SequenceContains<T, Item, Sequence<T, Items...>>::value &&
				IsSequenceUnique<Sequence<T, Items...>>::value),
			std::true_type,
			std::false_type>
	{ };

	template<typename T, T Item, T Last>
	struct IsSequenceUnique<Sequence<T, Item, Last>>
		: public std::conditional_t<Item != Last, std::true_type, std::false_type>
	{ };
}
