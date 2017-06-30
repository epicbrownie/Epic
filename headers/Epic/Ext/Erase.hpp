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

#include <algorithm>
#include <deque>
#include <forward_list>
#include <list>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <vector>

//////////////////////////////////////////////////////////////////////////////

namespace Epic::Ext
{
	/// Generically remove element x from container c
	template <typename Container, typename X>
	void erase(Container& c, const X& x);

	/// Generically remove all elements in container c which satisfy predicate p
	template <typename Container, typename Predicate>
	void erase_if(Container& c, Predicate p);

	/// Implementation
	namespace detail
	{
		// Dispatch tags
		struct vectorlike_tag { };		// Method #1 - Erase using erase-remove idiom
		struct listlike_tag { };		// Method #2 - Erase using remove()
		struct associative_tag { };		// Method #3 - Erase using erase()

		// Container dispatch tagging
		template<typename C>
		struct container_traits;

		// erase/erase_if implementations
		template <typename Container, typename X>
		void erase_helper(Container& c, const X& x, vectorlike_tag);

		template <typename Container, typename X>
		void erase_helper(Container& c, const X& x, listlike_tag);

		template <typename Container, typename X>
		void erase_helper(Container& c, const X& x, associative_tag);

		template <typename Container, typename Predicate>
		void erase_if_helper(Container& c, Predicate p, vectorlike_tag);

		template <typename Container, typename Predicate>
		void erase_if_helper(Container& c, Predicate p, listlike_tag);

		template <typename Container, typename Predicate>
		void erase_if_helper(Container& c, Predicate p, associative_tag);
	}
}

//////////////////////////////////////////////////////////////////////////////

#pragma region Container Tags

template<typename T, typename A>
struct Epic::Ext::detail::container_traits<std::vector<T, A>>
{
	using category = vectorlike_tag;
};

template <typename T, typename A>
struct Epic::Ext::detail::container_traits<std::deque<T, A>>
{
	using category = vectorlike_tag;
};


template <typename T, typename A>
struct Epic::Ext::detail::container_traits<std::list<T, A>>
{
	using category = listlike_tag;
};

template <typename T, typename A>
struct Epic::Ext::detail::container_traits<std::forward_list<T, A>>
{
	using category = listlike_tag;
};


template <typename T, typename C, typename A>
struct Epic::Ext::detail::container_traits<std::set<T, C, A>>
{
	using category = associative_tag;
};

template <typename T, typename C, typename A>
struct Epic::Ext::detail::container_traits<std::multiset<T, C, A>>
{
	using category = associative_tag;
};

template <typename T, typename C, typename A>
struct Epic::Ext::detail::container_traits<std::unordered_set<T, C, A>>
{
	using category = associative_tag;
};

template <typename T, typename C, typename A>
struct Epic::Ext::detail::container_traits<std::unordered_multiset<T, C, A>>
{
	using category = associative_tag;
};

template <typename K, typename V, typename C, typename A>
struct Epic::Ext::detail::container_traits<std::map<K, V, C, A>>
{
	using category = associative_tag;
};

template <typename K, typename V, typename C, typename A>
struct Epic::Ext::detail::container_traits<std::multimap<K, V, C, A>>
{
	using category = associative_tag;
};

template <typename K, typename V, typename C, typename A>
struct Epic::Ext::detail::container_traits<std::unordered_map<K, V, C, A>>
{
	using category = associative_tag;
};

template <typename K, typename V, typename C, typename A>
struct Epic::Ext::detail::container_traits<std::unordered_multimap<K, V, C, A>>
{
	using category = associative_tag;
};

#pragma endregion

//////////////////////////////////////////////////////////////////////////////

#pragma region Dispatched Erase/Erase-If Helpers

template <typename Container, typename X>
void Epic::Ext::detail::erase_helper(Container& c, const X& x, Epic::Ext::detail::vectorlike_tag)
{
	c.erase(std::remove(c.begin(), c.end(), x), c.end());
}

template <typename Container, typename X>
void Epic::Ext::detail::erase_helper(Container& c, const X& x, Epic::Ext::detail::listlike_tag)
{
	c.remove(x);
}

template <typename Container, typename X>
void Epic::Ext::detail::erase_helper(Container& c, const X& x, Epic::Ext::detail::associative_tag)
{
	c.erase(x);
}

template <typename Container, typename Predicate>
void Epic::Ext::detail::erase_if_helper(Container& c, Predicate p, Epic::Ext::detail::vectorlike_tag)
{
	c.erase(std::remove_if(c.begin(), c.end(), p), c.end());
}

template <typename Container, typename Predicate>
void Epic::Ext::detail::erase_if_helper(Container& c, Predicate p, Epic::Ext::detail::listlike_tag)
{
	c.remove_if(p);
}

template <typename Container, typename Predicate>
void Epic::Ext::detail::erase_if_helper(Container& c, Predicate p, Epic::Ext::detail::associative_tag)
{
	for (auto i = c.begin(); i != c.end();)
	{
		if (p(*i))
			c.erase(i++);
		else
			++i;
	}
}

#pragma endregion

//////////////////////////////////////////////////////////////////////////////

template <typename Container, typename X>
void Epic::Ext::erase(Container& c, const X& x)
{
	Epic::Ext::detail::erase_helper(c, x, typename Epic::Ext::detail::container_traits<Container>::category());
}

template <typename Container, typename Predicate>
void Epic::Ext::erase_if(Container& c, Predicate p)
{
	Epic::Ext::detail::erase_if_helper(c, p, typename Epic::Ext::detail::container_traits<Container>::category());
}