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

#include <Epic/Memory/Default.hpp>
#include <Epic/STL/ForwardList.hpp>
#include <Epic/NullMutex.hpp>
#include <algorithm>
#include <mutex>

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	template<class T>
	class AutoListDefaultStore;

	template<class T, bool Sync = false, template<class> class Store = AutoListDefaultStore>
	class AutoList;

	struct AutoListIgnoreT { };
}

//////////////////////////////////////////////////////////////////////////////

// AutoList<T, Sync, Store>
template<class T, bool Sync, template<class> class Store>
class Epic::AutoList
{
	static_assert(std::is_class<std::remove_cv_t<T>>::value, "AutoList can only store class types");

private:
	using Pointer = typename std::add_pointer<T>::type;
	using ListStore = Store<Pointer>;
	using MutexType = std::conditional_t<Sync, std::recursive_mutex, Epic::NullMutex>;

private:
	static MutexType s_Mutex;

protected:
	AutoList()
	{
		std::lock_guard<MutexType> lock(s_Mutex);
		ListStore::Insert(Pointer(this));
	}

	AutoList(const AutoList<T, Sync, Store>&)
	{
		std::lock_guard<MutexType> lock(s_Mutex);
		ListStore::Insert(Pointer(this));
	}

	AutoList(AutoList<T, Sync, Store>&& other)
	{
		std::lock_guard<MutexType> lock(s_Mutex);
		ListStore::Erase(Pointer(&other));
		ListStore::Insert(Pointer(this));
	}

	explicit AutoList(AutoListIgnoreT) noexcept { }

	~AutoList()
	{
		std::lock_guard<MutexType> lock(s_Mutex);
		ListStore::Erase(Pointer(this));
	}

public:
	// Iterate over the instance list, calling 'fn(pInstance)' for each instance pointer in the list.
	template<typename Function>
	static inline void IterateInstances(Function fn)
	{
		{	/* CS */
			std::lock_guard<MutexType> lock(s_Mutex);

			for (auto it = ListStore::Begin(); it != ListStore::End(); it++)
				fn(*it);
		}
	}

	// Iterate over the instance list, calling 'fn(pInstance)' for each instance pointer in the list.
	// The instance may be safely removed from the AutoList without disrupting iteration.
	template<typename Function>
	static void IterateInstancesSafe(Function fn)
	{
		{	/* CS */
			std::lock_guard<MutexType> lock(s_Mutex);

			auto it = ListStore::Begin();
			while (it != ListStore::End())
			{
				const auto itNext = ListStore::Next(it);
				fn(*it);
				it = itNext;
			}
		}
	}

	// Acquire an iterator to the beginning of the instance list.
	static inline auto InstancesBegin() noexcept
	{
		return ListStore::Begin();
	}

	// Acquire an iterator to one element past the end of the instance list.
	static inline auto InstancesEnd() noexcept
	{
		return ListStore::End();
	}
};

// Static Initializers
template<class T, bool Sync, template<class> class Store>
decltype(Epic::AutoList<T, Sync, Store>::s_Mutex) Epic::AutoList<T, Sync, Store>::s_Mutex;


//////////////////////////////////////////////////////////////////////////////

// AutoListDefaultStore<T>
template<class T>
class Epic::AutoListDefaultStore
{
	static_assert(std::is_nothrow_default_constructible<T>::value,
		"AutoListDefaultStore's value type must be no-throw default constructible.");

public:
	using Type = AutoListDefaultStore<T>;

	using Value = T;
	using AllocatedType = T;
	using Allocator = Epic::DefaultAllocatorFor<AllocatedType, Epic::eAllocatorFor::ForwardList>;

private:
	using ListType = Epic::STLForwardList<T, Allocator>;
	
private:
	static ListType s_Data;

public:
	static inline auto Begin() noexcept
	{
		return std::begin(s_Data);
	}

	static inline auto Next(decltype(Begin()) it) noexcept
	{
		return std::next(it);
	}

	static inline auto End() noexcept
	{
		return std::end(s_Data);
	}

public:
	static void Insert(const Value& value)
	{
		s_Data.emplace_front(value);
	}

	static void Erase(const Value& value) noexcept
	{
		auto it = Begin();
		auto itPrev = s_Data.cbefore_begin();

		for (; it != End(); itPrev = it++)
		{
			if (*it == value)
			{
				s_Data.erase_after(itPrev);
				return;
			}
		}
	}
};

// Static Initializers
template<class T>
decltype(Epic::AutoListDefaultStore<T>::s_Data) Epic::AutoListDefaultStore<T>::s_Data;

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	// Pass this to the constructor of AutoList to instruct it to NOT add the 
	// current instance to the AutoList
	AutoListIgnoreT AutoListIgnore;
}
