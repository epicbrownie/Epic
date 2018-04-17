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

#include <atomic>
#include <type_traits>

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	namespace detail
	{
		template<class T>
		class NullAtomicBase;

		template<class T>
		class NullAtomicIntegralBase;

		template<class T>
		class NullAtomicPointerBase;

		template<class T>
		struct NullAtomicBaseFor;
	}

	template<class T>
	class NullAtomic;
};

//////////////////////////////////////////////////////////////////////////////

/// NullAtomicBase<T>
template<class T>
class Epic::detail::NullAtomicBase
{
	static_assert(std::is_trivially_copyable<T>::value, "Only trivially copyable types can be used in NullAtomic.");

public:
	using Type = Epic::detail::NullAtomicBase<T>;
	using ValueType = T;

public:
	static constexpr bool is_always_lock_free = true;

private:
	ValueType _Value;

public:
	NullAtomicBase(void) noexcept { };
	constexpr NullAtomicBase(ValueType value) noexcept
		: _Value{ value }
	{ }

	NullAtomicBase(const Type&) = delete;
	NullAtomicBase(Type&&) = delete;
	
	Type& operator = (const Type&) = delete;

public:
	constexpr bool is_lock_free() const noexcept
	{
		return true;
	}

public:
	operator ValueType() const noexcept
	{
		return load();
	}

	ValueType operator = (ValueType value) noexcept
	{
		store(value);
		return value;
	}

public:
	void store(ValueType value, std::memory_order = std::memory_order_seq_cst) noexcept
	{
		_Value = std::move(value);
	}

	ValueType load(std::memory_order = std::memory_order_seq_cst) const noexcept
	{
		return _Value;
	}

	ValueType exchange(ValueType value, std::memory_order = std::memory_order_seq_cst) noexcept
	{
		ValueType tmp = std::move(value);
		std::swap(tmp, _Value);
		return tmp;
	}

	bool compare_exchange_weak(ValueType& expected, ValueType desired,
		std::memory_order order = std::memory_order_seq_cst) noexcept
	{
		return compare_exchange_strong(expected, desired, order, order);
	}

	bool compare_exchange_weak(ValueType& expected, ValueType desired,
		std::memory_order success, std::memory_order failure) noexcept
	{
		return compare_exchange_strong(expected, desired, success, failure);
	}

	bool compare_exchange_strong(ValueType& expected, ValueType desired,
		std::memory_order order = std::memory_order_seq_cst) noexcept
	{
		return compare_exchange_strong(expected, desired, order, order);
	}

	bool compare_exchange_strong(ValueType& expected, ValueType desired,
		std::memory_order, std::memory_order) noexcept
	{
		auto v = load();

		if (v == expected)
		{
			store(desired);
			return true;
		}
		else
		{
			expected = v;
			return false;
		}
	}
};

/// NullAtomicIntegralBase<T>
template<class T>
class Epic::detail::NullAtomicIntegralBase
	: public Epic::detail::NullAtomicBase<T>
{
public:
	using Type = Epic::detail::NullAtomicIntegralBase<T>;
	using Base = Epic::detail::NullAtomicBase<T>;
	using ValueType = typename Base::ValueType;

public:
	using Base::Base;

public:
	ValueType fetch_add(ValueType arg, std::memory_order order = std::memory_order_seq_cst)
	{
		return exchange(load() + arg, order);
	}

	ValueType fetch_sub(ValueType arg, std::memory_order order = std::memory_order_seq_cst)
	{
		return exchange(load() - arg, order);
	}

public:
	ValueType operator++ ()
	{
		return fetch_add(1) + 1;
	}

	ValueType operator++ (int)
	{
		return fetch_add(1);
	}

	ValueType operator-- ()
	{
		return fetch_sub(1) - 1;
	}

	ValueType operator-- (int)
	{
		return fetch_sub(1);
	}

	ValueType operator+= (ValueType arg)
	{
		return fetch_add(arg) + arg;
	}

	ValueType operator-= (ValueType arg)
	{
		return fetch_sub(arg) - arg;
	}
};

/// NullAtomicPointerBase<T>
template<class T>
class Epic::detail::NullAtomicPointerBase
	: public Epic::detail::NullAtomicBase<T>
{
public:
	using Type = Epic::detail::NullAtomicIntegralBase<T>;
	using Base = Epic::detail::NullAtomicBase<T>;
	using ValueType = typename Base::ValueType;

public:
	using Base::Base;

public:
	ValueType fetch_add(std::ptrdiff_t arg, std::memory_order order = std::memory_order_seq_cst)
	{
		return exchange(load() + arg, order);
	}

	ValueType fetch_sub(std::ptrdiff_t arg, std::memory_order order = std::memory_order_seq_cst)
	{
		return exchange(load() - arg, order);
	}

public:
	ValueType operator++ ()
	{
		return fetch_add(1) + 1;
	}

	ValueType operator++ (int)
	{
		return fetch_add(1);
	}

	ValueType operator-- ()
	{
		return fetch_sub(1) - 1;
	}

	ValueType operator-- (int)
	{
		return fetch_sub(1);
	}

	ValueType operator+= (std::ptrdiff_t arg)
	{
		return fetch_add(arg) + arg;
	}

	ValueType operator-= (std::ptrdiff_t arg)
	{
		return fetch_sub(arg) - arg;
	}
};

/// NullAtomicBaseFor<T>
template<class T>
struct Epic::detail::NullAtomicBaseFor
{
	using type = std::conditional_t<
		std::is_integral<T>::value,
		Epic::detail::NullAtomicIntegralBase<T>,
		Epic::detail::NullAtomicBase<T>>;
};

template<class T>
struct Epic::detail::NullAtomicBaseFor<T*>
{
	using type = Epic::detail::NullAtomicPointerBase<T*>;
};

//////////////////////////////////////////////////////////////////////////////

/// AtomicBase<T>
template<class T>
class Epic::NullAtomic : public Epic::detail::NullAtomicBaseFor<T>::type
{
public:
	using Type = Epic::NullAtomic<T>;
	using Base = typename Epic::detail::NullAtomicBaseFor<T>::type;

public:
	using Base::Base;
};
