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
#include <Epic/STL/List.hpp>
#include <Epic/STL/Vector.hpp>
#include <Epic/NullMutex.hpp>
#include <algorithm>
#include <mutex>

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	template<class T>
	class AutoListDefaultStore;

	template<class T>
	class AutoListVectorStore;

	template<class CRTP, bool Sync = false, template<class> class Store = AutoListDefaultStore>
	class AutoList;

	enum AutoListIgnoreT { AutoListIgnore };
}

//////////////////////////////////////////////////////////////////////////////

// AutoList<CRTP, Sync, Store>
template<class CRTP, bool Sync, template<class> class Store>
class Epic::AutoList
{
	static_assert(std::is_class<std::remove_cv_t<CRTP>>::value, "AutoList can only store class types");

private:
	using Type = Epic::AutoList<CRTP, Sync, Store>;
	using Pointer = typename std::add_pointer<CRTP>::type;
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

	AutoList(const Type&)
	{
		std::lock_guard<MutexType> lock(s_Mutex);
		ListStore::Insert(Pointer(this));
	}

	AutoList(Type&& other)
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

			for(auto it = ListStore::SafeBegin(); it != ListStore::SafeEnd(); ++it)
				fn(*it);
		}
	}

	// Acquire an iterator to the beginning of the instance list. (Unsynchronized) 
	static inline auto InstancesBegin() noexcept
	{
		return ListStore::Begin();
	}

	// Acquire an iterator to one element past the end of the instance list. (Unsynchronized)
	static inline auto InstancesEnd() noexcept
	{
		return ListStore::End();
	}
};

// AutoList Static Initializers
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
	using Allocator = Epic::DefaultAllocatorFor<AllocatedType, Epic::eAllocatorFor::List>;

private:
	using ListType = Epic::STLList<T, Allocator>;
	
private:
	struct SafeIterator
	{
		using Iterator = typename ListType::iterator;
		using ConstIterator = typename ListType::const_iterator;

		Iterator itThis;
		Iterator itNext;
		ConstIterator itEnd;

		SafeIterator(const Iterator& iterThis, const ConstIterator& iterEnd)
			: itThis(iterThis), itEnd(iterEnd)
		{
			itNext = (itThis != itEnd) ? std::next(itThis) : itThis;
		}

		inline auto operator*() { return *itThis; }
		
		inline bool operator== (const SafeIterator& other) const noexcept
		{
			if (itThis == itEnd)
				return other.itThis == other.itEnd;

			return itThis == other.itThis;
		}
		
		inline bool operator!= (const SafeIterator& other) const noexcept
		{
			return !(this->operator== (other));
		}

		inline SafeIterator& operator++ () noexcept
		{
			itThis = itNext;
			
			if (itThis != itEnd)
				itNext = std::next(itThis);

			return *this;
		}
	};

private:
	static ListType s_Data;

public:
	static inline auto Begin() noexcept
	{
		return std::begin(s_Data);
	}

	static inline auto End() noexcept
	{
		return std::end(s_Data);
	}

	static inline auto SafeBegin() noexcept
	{
		return SafeIterator{ std::begin(s_Data), std::cend(s_Data) };
	}

	static inline auto SafeEnd() noexcept
	{
		return SafeIterator{ std::end(s_Data), std::cend(s_Data) };
	}

public:
	static void Insert(const Value& value)
	{
		s_Data.emplace_front(value);
	}

	static void Erase(const Value& value) noexcept
	{
		auto it = std::find(std::begin(s_Data), std::end(s_Data), value);
		if (it != std::end(s_Data))
			s_Data.erase(it);
	}
};

// AutoListDefaultStore Static Initializers
template<class T>
decltype(Epic::AutoListDefaultStore<T>::s_Data) Epic::AutoListDefaultStore<T>::s_Data;

//////////////////////////////////////////////////////////////////////////////

// AutoListVectorStore<T>
template<class T>
class Epic::AutoListVectorStore
{
	static_assert(std::is_nothrow_default_constructible<T>::value,
		"AutoListVectorStore's value type must be no-throw default constructible.");

public:
	using Type = AutoListVectorStore<T>;

	using Value = T;
	using AllocatedType = T;
	using Allocator = Epic::DefaultAllocatorFor<AllocatedType, Epic::eAllocatorFor::Vector>;

private:
	using ListType = Epic::STLVector<T, Allocator>;

private:
	struct SafeIterator
	{
		using Index = typename ListType::size_type;

		Index indexThis;
		Index lastSize;
		ListType* pList;
		
		SafeIterator(ListType* lst, Index n)
			: pList(lst), indexThis(n), lastSize(lst->size())
		{ }

		inline auto operator*() { return pList->at(indexThis); }

		inline bool operator== (const SafeIterator& other) const noexcept
		{
			if (indexThis >= pList->size())
				return other.indexThis >= other.pList->size();

			return indexThis == other.indexThis;
		}

		inline bool operator!= (const SafeIterator& other) const noexcept
		{
			return !(this->operator== (other));
		}

		inline SafeIterator& operator++ () noexcept
		{
			if (indexThis >= pList->size())
				return *this;

			if (lastSize == pList->size())
				++indexThis;

			lastSize = pList->size();

			return *this;
		}
	};

private:
	static ListType s_Data;

public:
	static inline auto Begin() noexcept
	{
		return std::begin(s_Data);
	}

	static inline auto End() noexcept
	{
		return std::end(s_Data);
	}

	static inline auto SafeBegin() noexcept
	{
		return SafeIterator{ &s_Data, 0 };
	}

	static inline auto SafeEnd() noexcept
	{
		return SafeIterator{ &s_Data, std::size(s_Data) };
	}

public:
	static void Insert(const Value& value)
	{
		s_Data.emplace_back(value);
	}

	static void Erase(const Value& value) noexcept
	{
		auto it = std::find(std::begin(s_Data), std::end(s_Data), value);
		if (it != std::end(s_Data))
			s_Data.erase(it);
	}
};

// AutoListVectorStore Static Initializers
template<class T>
decltype(Epic::AutoListVectorStore<T>::s_Data) Epic::AutoListVectorStore<T>::s_Data;
