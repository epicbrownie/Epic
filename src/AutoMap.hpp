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
#include <Epic/STL/Map.hpp>
#include <Epic/NullMutex.hpp>
#include <algorithm>
#include <mutex>

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	template<class K, class V>
	class AutoMapDefaultStore;

	template<class T, class Key = Epic::StringHash, bool Sync = false, template<class, class> class Store = AutoMapDefaultStore>
	class AutoMap;

	struct AutoMapIgnoreT{ };
}

//////////////////////////////////////////////////////////////////////////////

// AutoMap<T, K, Store>
template<class T, class K, bool Sync, template<class, class> class Store>
class Epic::AutoMap
{
	static_assert(std::is_class<std::remove_cv_t<T>>::value, "AutoMap can only store class types");

private:
	using Key = K;
	using Pointer = typename std::add_pointer<T>::type;
	using MapStore = Store<Key, Pointer>;
	using MutexType = std::conditional_t<Sync, std::recursive_mutex, Epic::NullMutex>;

private:
	static MutexType s_Mutex;

protected:
	AutoMap() = delete;
	
	explicit AutoMap(const Key& key)
	{
		std::lock_guard<MutexType> lock(s_Mutex);
		MapStore::Insert(key, Pointer(this));
	}

	AutoMap(const AutoMap<T, K, Sync, Store>&) = delete;

	AutoMap(const AutoMap<T, K, Sync, Store>&, const Key& key)
	{
		std::lock_guard<MutexType> lock(s_Mutex);
		MapStore::Insert(key, Pointer(this));
	}

	AutoMap(AutoMap<T, K, Sync, Store>&&) = delete;

	AutoMap(AutoMap<T, K, Sync, Store>&& other, const Key& key)
	{
		std::lock_guard<MutexType> lock(s_Mutex);
		MapStore::EraseValue(Pointer(&other));
		MapStore::Insert(key, Pointer(this));
	}

	explicit AutoMap(AutoMapIgnoreT) noexcept { }

	~AutoMap() noexcept
	{
		std::lock_guard<MutexType> lock(s_Mutex);
		MapStore::EraseValue(Pointer(this));
	}

public:
	// Retrieve an instance pointer from the AutoMap identified by 'key'
	static Pointer GetInstance(const Key& key)
	{
		std::lock_guard<MutexType> lock(s_Mutex);
		return MapStore::Get(key);
	}
};

template<class T, class K, bool Sync, template<class, class> class Store>
decltype(Epic::AutoMap<T, K, Sync, Store>::s_Mutex) Epic::AutoMap<T, K, Sync, Store>::s_Mutex;

//////////////////////////////////////////////////////////////////////////////

// AutoMapDefaultStore<K, V>
template<class K, class V>
class Epic::AutoMapDefaultStore
{
	static_assert(std::is_nothrow_default_constructible<V>::value,
		"AutoMapDefaultStore's value type must be no-throw default constructible.");

public:
	using Type = AutoMapDefaultStore<K, V>;

	using Key = K;
	using Value = V;
	using AllocatedType = std::pair<const Key, Value>;
	using Allocator = Epic::DefaultAllocatorFor<AllocatedType, Epic::eAllocatorFor::Map>;

private:
	using MapType = Epic::STLMap<Key, Value, std::less<Key>, AllocatedType, Allocator>;

private:
	static MapType s_Data;

public:
	static inline Value Get(const Key& key) noexcept
	{
		auto it = s_Data.find(key);
		return (it == std::end(s_Data)) ? Value() : (*it).second;
	}

public:
	static void Insert(const Key& key, const Value& value)
	{
		s_Data.emplace(key, value);
	}

	static void Erase(const Key& key) noexcept
	{
		s_Data.erase(key);
	}

	static void EraseValue(const Value& value) noexcept
	{
		auto it = std::find_if(std::begin(s_Data), std::end(s_Data),
			[&](const auto& pair)
		{
			return pair.second == value;
		});

		if (it != std::end(s_Data))
			s_Data.erase(it);
	}
};

// Static Initializers
template<class K, class V>
decltype(Epic::AutoMapDefaultStore<K, V>::s_Data) Epic::AutoMapDefaultStore<K, V>::s_Data;

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	// Pass this to the constructor of AutoMap to instruct it to NOT add the 
	// current instance to the AutoMap
	AutoMapIgnoreT AutoMapIgnore;
}
