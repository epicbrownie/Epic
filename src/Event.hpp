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

#include <Epic/StringHash.hpp>
#include <Epic/Memory/Default.hpp>
#include <Epic/STL/Vector.hpp>
#include <Epic/TMP/Sequence.hpp>
#include <boost/preprocessor.hpp>
#include <cstdint>
#include <functional>
#include <tuple>

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	namespace detail
	{
		template<class R, class... Args>
		class EventBase;
	}

	template<typename Signature>
	class Event;

	template<typename Signature>
	class PolledEvent;
}

//////////////////////////////////////////////////////////////////////////////

// The following macros are used with the boost preprocessor library to allow for automatic parameter 
// binding when binding member function pointers to events.  Set BMFP_COUNT to a number that is 
// less-than or equal to the number of std::placeholders defined for your system.

#define BMFP_COUNT 20
#define BMFP_PH_DECL(z, n, text) BOOST_PP_COMMA() BOOST_PP_CAT(std::placeholders::_, BOOST_PP_ADD(n, 1))
#define BMFP_N_DECL(z, n, text)																	\
	template<class T, class This>																\
	struct BMFP< ## n ##, T, This>																\
	{																							\
		static inline DelegateType Bind(const This* pThis, R(T::* fn)(Args...) const) noexcept	\
		{  return std::bind(fn, pThis BOOST_PP_REPEAT(n, BMFP_PH_DECL, ~));	 }					\
		static inline DelegateType Bind(This* pThis, R(T::* fn)(Args...)) noexcept				\
		{  return std::bind(fn, pThis BOOST_PP_REPEAT(n, BMFP_PH_DECL, ~));	 }					\
	};

//////////////////////////////////////////////////////////////////////////////

// EventBase
template<class R, class... Args>
class Epic::detail::EventBase
{
public:
	using Type = Epic::detail::EventBase<R, Args...>;
	using DelegateType = std::function<R(Args...)>;

protected:
	struct EventHandler
	{
		using HandleType = uint32_t;
		using InstanceType = intptr_t;

		HandleType handle;
		InstanceType instance;
	};

	struct MutateQueueEntry
	{
		enum eMutateCommand
		{ 
			Subscribe, 
			Unsubscribe, 
			UnsubscribeHandle, 
			UnsubscribeInstance, 
			UnsubscribeAll 
		};

		MutateQueueEntry(eMutateCommand cmd)
			: command{ cmd }, handler{ 0, 0 }, delegate{} 
		{ }

		MutateQueueEntry(eMutateCommand cmd, const EventHandler& hand, const DelegateType fn)
			: command{ cmd }, handler{ hand }, delegate{ fn } 
		{ }

		DelegateType delegate;
		EventHandler handler;
		eMutateCommand command;
	};

protected:
	using Listener = std::pair<EventHandler, DelegateType>;
	using ListenerList = Epic::STLVector<Listener>;
	using MutateQueue = Epic::SmallVector<MutateQueueEntry, 2>;

protected:
	ListenerList m_Listeners;
	MutateQueue m_MutateQueue;
	bool m_IsSuspended;

public:
	inline EventBase() noexcept
		: m_IsSuspended(false)
	{ }

	inline EventBase(Type& other) noexcept
		: m_IsSuspended(false),
		  m_Listeners(other.m_Listeners),
		  m_MutateQueue(other.m_MutateQueue)
	{ }

	inline EventBase(Type&& other) noexcept
		: m_IsSuspended(std::move(other.m_IsSuspended)),
		  m_Listeners(std::move(other.m_Listeners)),
		  m_MutateQueue(std::move(other.m_MutateQueue))
	{ }

public:
	Type& operator = (const Type& other) = delete;
	Type& operator = (Type&& other) = delete;

public:
	explicit inline operator bool() const noexcept
	{
		return !m_Listeners.empty();
	}

protected:
	// Subscribe listener without instance or handle
	// If the listener list is currently being processed, the subscription will be queued
	// and processed during the next Flush operation
	inline void Subscribe(const DelegateType& delegate) noexcept
	{
		Subscribe(delegate, 0, 0);
	}

	// Subscribe listener with instance (null handle)
	inline void Subscribe(const DelegateType& delegate, const typename EventHandler::InstanceType instance) noexcept
	{
		Subscribe(delegate, instance, 0);
	}

	// Subscribe listener with handle (null instance)
	inline void Subscribe(const DelegateType& delegate, const typename EventHandler::HandleType handle) noexcept
	{
		Subscribe(delegate, 0, handle);
	}

	// Subscribe listener with instance and handle
	void Subscribe(const DelegateType& delegate, 
				   const typename EventHandler::InstanceType instance, 
				   const typename EventHandler::HandleType handle) noexcept
	{
		if (!m_IsSuspended)
		{
			if ((instance == 0 && handle == 0) || !HasInstanceAndHandle(instance, handle))
				m_Listeners.emplace_back(EventHandler{ handle, instance }, delegate);
		}
		else
		{
			m_MutateQueue.emplace_back(MutateQueueEntry::Subscribe, EventHandler{ handle, instance }, delegate);
		}
	}

	// Unsubscribe listener with instance (null handle)
	inline void Unsubscribe(const typename EventHandler::InstanceType instance) noexcept
	{
		Unsubscribe(instance, 0);
	}

	// Unsubscribe all listeners with handle
	void Unsubscribe(const typename EventHandler::HandleType handle) noexcept
	{
		if (!m_IsSuspended)
		{
			// Multiple listeners can match this handle; unsubscribe all of them.
			m_Listeners.erase(std::remove_if(
				std::begin(m_Listeners),
				std::end(m_Listeners),
				[&](const auto& o) { return o.first.handle == handle; }
			), std::end(m_Listeners));
		}
		else
		{
			// Queue the unsubscription
			m_MutateQueue.emplace_back(MutateQueueEntry::UnsubscribeHandle, EventHandler{ handle, 0 }, DelegateType());
		}
	}

	// Unsubscribe one listener with instance and handle
	void Unsubscribe(const typename EventHandler::InstanceType instance, const typename EventHandler::HandleType handle) noexcept
	{
		if (!m_IsSuspended)
		{
			// There can only be one listener that has both this instance and this handle
			auto it = std::find_if(std::begin(m_Listeners), std::end(m_Listeners),
				[&](const auto& o) { return o.first.instance == instance && o.first.handle == handle; });

			if (it != std::end(m_Listeners))
				m_Listeners.erase(it);
		}
		else
		{
			// Queue the unsubscription
			m_MutateQueue.emplace_back(MutateQueueEntry::Unsubscribe, EventHandler{ handle, instance }, DelegateType());
		}
	}

	// Unsubscribe all listeners with instance
	void UnsubscribeAll(const typename EventHandler::InstanceType instance) noexcept
	{
		if (!m_IsSuspended)
		{
			// Unsubscribe all listeners that match this instance
			m_Listeners.erase(std::remove_if(
				std::begin(m_Listeners),
				std::end(m_Listeners),
				[&](const auto& o) { return o.first.instance == instance; }
			), std::end(m_Listeners));
		}
		else
		{
			// Queue the unsubscription
			m_MutateQueue.emplace_back(MutateQueueEntry::UnsubscribeInstance, EventHandler{ 0, instance }, DelegateType());
		}
	}

	// Unsubscribe all listeners
	void UnsubscribeAll() noexcept
	{
		if (!m_IsSuspended)
		{
			// Unsubscribe every listener
			m_Listeners.clear();
		}
		else
		{
			// Queue the unsubscription
			m_MutateQueue.emplace_back(MutateQueueEntry::UnsubscribeAll);
		}
	}

	// Iterate through listeners to determine if one can be found with the provided instance and handle
	inline bool HasInstanceAndHandle(const typename EventHandler::InstanceType instance, const typename EventHandler::HandleType handle) noexcept
	{
		return std::find_if(std::begin(m_Listeners), std::end(m_Listeners),
			[&](const auto& o)
			{
				return o.first.instance == instance && o.first.handle == handle;
			}) != std::end(m_Listeners);
	}

	// Suspend or resume subscribing/unsubscribing new listeners freely
	inline void Suspend(bool shouldSuspend) noexcept
	{
		m_IsSuspended = shouldSuspend;
	}

	// Process mutate queue
	void Flush() noexcept
	{
		Suspend(false);

		while (!m_MutateQueue.empty())
		{
			// Get the next queue item
			auto entry = std::move(m_MutateQueue.back());
			m_MutateQueue.pop_back();

			switch (entry.command)
			{
			default:
				break;

			case MutateQueueEntry::Subscribe:
				Subscribe(entry.delegate, entry.handler.instance, entry.handler.handle);
				break;

			case MutateQueueEntry::Unsubscribe:
				Unsubscribe(entry.handler.instance, entry.handler.handle);
				break;

			case MutateQueueEntry::UnsubscribeHandle:
				Unsubscribe(entry.handler.handle);
				break;

			case MutateQueueEntry::UnsubscribeInstance:
				UnsubscribeAll(entry.handler.instance);
				break;

			case MutateQueueEntry::UnsubscribeAll:
				UnsubscribeAll();
				break;
			}
		}
	}

public:
	// Returns the number of listeners subscribed to this event
	inline auto GetListenerCount() const noexcept
	{
		return std::size(m_Listeners);
	}

public:
	// Connect a function handler
	inline void Connect(const DelegateType& delegate) noexcept
	{
		Subscribe(delegate);
	}

	// Connect a function handler with a handle
	inline void Connect(const DelegateType& delegate, Epic::StringHash handle) noexcept
	{
		Subscribe(delegate, typename EventHandler::HandleType(handle));
	}

	// Connect a function object handler
	template<class Function>
	inline void Connect(const Function& fn) noexcept
	{
		Subscribe(DelegateType(fn));
	}

	// Connect a function object handler with a handle
	template<class Function>
	inline void Connect(const Function& fn, Epic::StringHash handle) noexcept
	{
		Subscribe(DelegateType(fn), typename EventHandler::HandleType(handle));
	}

	// Connect a static function pointer handler
	inline void Connect(R(*fn)(Args...)) noexcept
	{
		Subscribe(DelegateType(fn), reinterpret_cast<EventHandler::InstanceType>(fn));
	}

	// Connect a static function pointer handler with a handle
	inline void Connect(R(*fn)(Args...), Epic::StringHash handle) noexcept
	{
		Subscribe(DelegateType(fn), 
				  reinterpret_cast<EventHandler::InstanceType>(fn), 
				  typename EventHandler::HandleType(handle));
	}

	// Connect a member function pointer handler
	template<class T, class This>
	inline void Connect(This* pThis, R(T::* fn)(Args...)) noexcept
	{
		Subscribe(BMFP<sizeof...(Args), T, This>::Bind(pThis, fn),
				  reinterpret_cast<EventHandler::InstanceType>(pThis));
	}

	// Connect a const member function pointer handler
	template<class T, class This>
	inline void Connect(const This* pThis, R(T::* fn)(Args...) const) noexcept
	{
		Subscribe(BMFP<sizeof...(Args), T, This>::Bind(pThis, fn),
				  reinterpret_cast<EventHandler::InstanceType>(pThis));
	}

	// Connect a member function pointer handler with a handle
	template<class T, class This>
	inline void Connect(This* pThis, R(T::* fn)(Args...), Epic::StringHash handle) noexcept
	{
		Subscribe(BMFP<sizeof...(Args), T, This>::Bind(pThis, fn),
				  reinterpret_cast<EventHandler::InstanceType>(pThis),
				  typename EventHandler::HandleType>(handle));
	}

	// Connect a const member function pointer handler with a handle
	template<class T, class This>
	inline void Connect(const This* pThis, R(T::* fn)(Args...) const, Epic::StringHash handle) noexcept
	{
		Subscribe(BMFP<sizeof...(Args), T, This>::Bind(pThis, fn),
				  reinterpret_cast<EventHandler::InstanceType>(pThis),
				  typename EventHandler::HandleType>(handle));
	}

	// Disconnect all handlers with the supplied handle
	template<size_t N>
	inline void Disconnect(const char(&cstr)[N]) noexcept
	{
		Unsubscribe(typename EventHandler::HandleType(Epic::Hash(cstr)));
	}

	// Disconnect all handlers with the supplied handle
	inline void Disconnect(Epic::StringHash handle) noexcept
	{
		Unsubscribe(typename EventHandler::HandleType(handle));
	}

	// Disconnect a static function pointer handler (this will NOT disconnect listeners that provided a handle)
	inline void Disconnect(R(*fn)(Args...)) noexcept
	{
		Unsubscribe(reinterpret_cast<EventHandler::InstanceType>(fn));
	}

	// Disconnect a static function pointer handler with a handle
	inline void Disconnect(R(*fn)(Args...), Epic::StringHash handle) noexcept
	{
		Unsubscribe(reinterpret_cast<EventHandler::InstanceType>(fn), typename EventHandler::HandleType(handle));
	}

	// Disconnect a member function pointer handler (this will NOT disconnect listeners that provided a handle)
	template<class This>
	inline void Disconnect(const This* pThis) noexcept
	{
		Unsubscribe(reinterpret_cast<EventHandler::InstanceType>(pThis));
	}

	// Disconnect a member function pointer handler with a handle
	template<class This>
	inline void Disconnect(const This* pThis, Epic::StringHash handle) noexcept
	{
		Unsubscribe(reinterpret_cast<EventHandler::InstanceType>(pThis),
					typename EventHandler::HandleType(handle));
	}

	// Disconnect all static function pointer handlers with this address (even listeners that provided a handle)
	inline void DisconnectAll(R(*fn)(Args...)) noexcept
	{
		UnsubscribeAll(reinterpret_cast<EventHandler::InstanceType>(fn));
	}

	// Disconnect all member function pointer handlers with this instance address (even listeners that provided a handle)
	template<class This>
	inline void DisconnectAll(const This* pThis) noexcept
	{
		UnsubscribeAll(reinterpret_cast<EventHandler::InstanceType>(pThis));
	}

	// Disconnect all listeners
	inline void DisconnectAll() noexcept
	{
		UnsubscribeAll();
	}

public:
	// Alias: Connect a standard function handler
	inline Type& operator += (const DelegateType& delegate) noexcept
	{
		Connect(delegate);
		return *this;
	}

	// Alias: Connect a function object handler
	template<class Function>
	inline Type& operator += (const Function& fn) noexcept
	{
		Connect<Function>(fn);
		return *this;
	}

	// Alias: Connect a static function pointer handler
	inline Type& operator += (R(*fn)(Args...)) noexcept
	{
		Connect(fn);
		return *this;
	}

	// Alias: Disconnect all handlers with the supplied handle
	template<size_t N>
	inline Type& operator -= (const char(&cstr)[N]) noexcept
	{
		Disconnect(typename EventHandler::HandleType(Epic::Hash(cstr)));
		return *this;
	}

	// Alias: Disconnect all handlers with the supplied handle
	inline Type& operator -= (Epic::StringHash handle) noexcept
	{
		Disconnect(handle);
		return *this;
	}

	// Alias: Disconnect a static function pointer handler (this will NOT disconnect listeners that provided a handle)
	inline Type& operator -= (R(*fn)(Args...)) noexcept
	{
		Disconnect(fn);
		return *this;
	}

	// Alias: Disconnect a member function pointer handler (this will NOT disconnect listeners that provided a handle)
	template<class This>
	inline Type& operator -= (const This* pThis) noexcept
	{
		Disconnect(pThis);
		return *this;
	}

private:
	template<size_t argc, class T, class This>
	struct BMFP
	{
		static inline DelegateType Bind(const This* /* pThis */, R(T::* /* fn */)(Args...) const) noexcept
		{
			static_assert(false, "Unable to bind member function pointer!  Are you trying to bind more than BMFP_COUNT parameters?");
		}

		static inline DelegateType Bind(This* /* pThis */, R(T::* /* fn */)(Args...)) noexcept
		{
			static_assert(false, "Unable to bind member function pointer!  Are you trying to bind more than BMFP_COUNT parameters?");
		}
	};

	BOOST_PP_REPEAT(BMFP_COUNT, BMFP_N_DECL, ~);
};

//////////////////////////////////////////////////////////////////////////////

// PolledEvent<Signature>
template<class R, class... Args>
class Epic::PolledEvent<R(Args...)> : public Epic::detail::EventBase<R, Args...>
{
	using Type = Epic::PolledEvent<R(Args...)>;
	using Base = Epic::detail::EventBase<R, Args...>;

public:
	using Base::Base;

private:
	template<class EventType>
	struct ScopeSuspend
	{
		ScopeSuspend(EventType& evt)
			: _event(evt)
		{
			_event.Suspend(true);
		}

		~ScopeSuspend()
		{
			_event.Flush();
		}

		EventType& _event;
	};

	template<class EventType>
	friend struct ScopeSuspend;

private:
	template<class S>
	friend class Event;

protected:
	using Invocation = std::tuple<Args...>;
	using InvocationQueue = Epic::STLVector<Invocation>;

private:
	InvocationQueue m_Invocations;

public:
	// Buffer an invocation of the event
	inline void operator() (Args... args) noexcept
	{
		m_Invocations.emplace_back(std::forward<Args>(args)...);
	}

	// Buffer an invocation of the event
	inline void Invoke(Args... args) noexcept
	{
		this->operator() (std::forward<Args>(args)...);
	}

private:
	template<size_t... Is>
	void DoInvoke(Invocation& invocation, std::integer_sequence<size_t, Is...>)
	{
		for (auto& listener : m_Listeners)
			listener.second(std::get<Is>(invocation)...);
	}

	template<class Return>
	inline Return ForwardedInvoke(Args... args) noexcept
	{
		Invoke(std::forward<Args>(args)...);
		return Return();
	}

public:
	// Invoke all pending event invocations
	void Poll()
	{
		ScopeSuspend<Type> _suspend(*this);

		for(auto& argPack : m_Invocations)
			DoInvoke(argPack, Epic::TMP::MakeSequence<size_t, sizeof...(Args)>());
	}
};

//////////////////////////////////////////////////////////////////////////////

// Event<Signature>
template<class R, class... Args>
class Epic::Event<R(Args...)> : public Epic::detail::EventBase<R, Args...>
{
	using Type = Epic::Event<R(Args...)>;
	using Base = Epic::detail::EventBase<R, Args...>;

public:
	using Base::Base;

private:
	template<typename EventType>
	struct ScopeSuspend
	{
		ScopeSuspend(EventType& evt) 
			: _event(evt) 
		{ 
			_event.Suspend(true); 
		}

		~ScopeSuspend() 
		{ 
			_event.Flush(); 
		}

		EventType& _event;
	};

	template<typename EventType>
	friend struct ScopeSuspend;

public:
	// Import base Connect overloads
	using Base::Connect;

	// Connect an event as a handler to this event
	template<class Return>
	inline void Connect(Event<Return(Args...)>& event) noexcept
	{
		Base::Connect(&event, &Event<Return(Args...)>::ForwardedInvoke<R>);
	}

	// Connect an event as a handler to this event with a handle
	template<class Return>
	inline void Connect(Event<Return(Args...)>& event, Epic::StringHash handle) noexcept
	{
		Base::Connect(&event, &Event<Return(Args...)>::ForwardedInvoke<R>, handle);
	}

	// Connect a polled event as a handler to this event
	template<class Return>
	inline void Connect(PolledEvent<Return(Args...)>& event) noexcept
	{
		Base::Connect(&event, &PolledEvent<Return(Args...)>::ForwardedInvoke<R>);
	}
	
	// Connect a polled event as a handler to this event with a handle
	template<class Return>
	inline void Connect(PolledEvent<Return(Args...)>& event, Epic::StringHash handle) noexcept
	{
		Base::Connect(&event, &PolledEvent<Return(Args...)>::ForwardedInvoke<R>, handle);
	}

	// Import base Disconnect overloads
	using Base::Disconnect;

	// Disconnect an event handler
	template<class Return>
	inline void Disconnect(const Event<Return(Args...)>& event) noexcept
	{
		Base::Disconnect(&event);
	}

	// Disconnect an event handler with a handle
	template<class Return>
	inline void Disconnect(const Event<Return(Args...)>& event, Epic::StringHash handle) noexcept
	{
		Base::Disconnect(&event, handle);
	}

	// Disconnect a polled event handler
	template<class Return>
	inline void Disconnect(const PolledEvent<Return(Args...)>& event) noexcept
	{
		Base::Disconnect(&event);
	}

	// Disconnect a polled event handler with a handle
	template<class Return>
	inline void Disconnect(const PolledEvent<Return(Args...)>& event, Epic::StringHash handle) noexcept
	{
		Base::Disconnect(&event, handle);
	}

public:
	// Import Connect/Disconnect operators
	using Base::operator +=;
	using Base::operator -=;

	// Alias: Connect an event as a handler to this event
	template<class Return>
	inline Type& operator += (Event<Return(Args...)>& event) noexcept
	{
		Connect(event);
		return *this;
	}

	// Alias: Connect a polled event as a handler to this event
	template<class Return>
	inline Type& operator += (PolledEvent<Return(Args...)>& event) noexcept
	{
		Connect(event);
		return *this;
	}

	// Alias: Disconnect an event as a handler to this event
	template<class Return>
	inline Type& operator -= (Event<Return(Args...)>& event) noexcept
	{
		Disconnect(event);
		return *this;
	}

	// Alias: Disconnect a polled event as a handler to this event
	template<class Return>
	inline Type& operator -= (PolledEvent<Return(Args...)>& event) noexcept
	{
		Disconnect(event);
		return *this;
	}

public:
	// Invoke the event (handler return values are ignored)
	void operator() (Args... args)
	{
		ScopeSuspend<Type> _suspend(*this);
		
		for (auto& listener : m_Listeners)
			listener.second(std::forward<Args>(args)...);
	}

	// Invoke the event (handler return values are ignored)
	inline void Invoke(Args... args)
	{
		this->operator() (std::forward<Args>(args)...);
	}

	// Invoke the event and store listener return values into an accumulator
	template<class Accumulator = Epic::STLVector<R>>
	Accumulator InvokeAccumulate(Args... args)
	{
		ScopeSuspend<Type> _suspend(*this);
		Accumulator accum;

		for (auto& listener : m_Listeners)
			accum.emplace_back(listener.second(std::forward<Args>(args)...));

		return accum;
	}

	// Invoke the event and store listener return values into the supplied iterator
	template<class OutIter>
	void InvokeAccumulate(OutIter dest, Args... args)
	{
		ScopeSuspend<Type> _suspend(*this);

		for (auto& listener : m_Listeners)
			*dest++ = listener.second(std::forward<Args>(args)...);
	}

	// Listeners will be invoked and their return value fed to the predicate.
	// This loop will continue until the predicate evaluates to true.
	// Returns true if predicate ever evaluated to true.
	template<class Predicate>
	bool InvokeUntil(Predicate& predicate, Args... args)
	{
		ScopeSuspend<Type> _suspend(*this);

		for (auto& listener : m_Listeners)
		{
			if (predicate(listener.second(std::forward<Args>(args)...)))
				return true;
		}

		return false;
	}

	// Invoke the event until a listener returns the parameter value
	// This function returns whether or not a delegate returned the parameter value
	template<typename RV = std::enable_if_t<!std::is_void<R>::value, R>>
	bool InvokeUntil(const RV& value, Args... args)
	{
		ScopeSuspend<Type> _suspend(*this);

		for (auto& listener : m_Listeners)
		{
			if (value == listener.second(std::forward<Args>(args)...))
				return true;
		}

		return false;
	}

	// Listeners will be invoked and their return value fed to the predicate.
	// This loop will continue while the predicate evaluates to true.
	// Returns true if all listeners were invoked and predicate always evaluated to true.
	template<class Predicate>
	bool InvokeWhile(Predicate& predicate, Args... args)
	{
		ScopeSuspend<Type> _suspend(*this);

		for (auto& listener : m_Listeners)
		{
			if (!predicate(listener.second(std::forward<Args>(args)...)))
				return false;
		}

		return true;
	}

	// Invoke the event while delegates return the parameter value
	// This function returns true if all listeners returned 'value'
	template<typename RV = std::enable_if_t<!std::is_void<R>::value, R>>
	bool InvokeWhile(const RV& value, Args... args)
	{
		ScopeSuspend<Type> _suspend(*this);

		for (auto& listener : m_Listeners)
		{
			if (value != listener.second(std::forward<Args>(args)...))
				return false;
		}

		return true;
	}

private:
	template<class Return>
	inline Return ForwardedInvoke(Args... args) noexcept
	{
		Invoke(std::forward<Args>(args)...);
		return Return();
	}
};

//////////////////////////////////////////////////////////////////////////////

// Undefine the preprocessor macros (clean up)
#undef BMFP_N_DECL
#undef BMFP_PH_DECL
#undef BMFP_COUNT
