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

#include <Epic/Core/Clock.hpp>
#include <Epic/Core/Event.hpp>
#include <Epic/AutoList.hpp>

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	namespace detail
	{
		class AutoTimer;
	}

	template<class ClockType = decltype(Epic::StandardClock)>
	class TaskTimer;

	template<class ClockType = decltype(Epic::StandardClock)>
	class PeriodicTimer;

	template<class ClockType = decltype(Epic::StandardClock)>
	class DiscreteTimer;
}

//////////////////////////////////////////////////////////////////////////////

// AutoTimer
class Epic::detail::AutoTimer : public Epic::AutoList<Epic::detail::AutoTimer, true>
{
public:
	using Type = Epic::detail::AutoTimer;
	using Base = Epic::AutoList<Epic::detail::AutoTimer, true>;

public:
	inline AutoTimer() noexcept { };

public:
	virtual void Update() = 0;
};

//////////////////////////////////////////////////////////////////////////////

// TaskTimer
template<class C>
class Epic::TaskTimer : public Epic::detail::AutoTimer
{
public:
	using Type = Epic::TaskTimer<C>;
	using Base = Epic::detail::AutoTimer;

public:
	using ClockType = C;
	using TimeStamp = typename ClockType::TimeStamp;
	using Duration = typename ClockType::Unit;

public:
	using TickDelegate = Epic::Event<void()>;

public:
	TickDelegate Tick;

private:
	const ClockType& m_Clock;
	TimeStamp m_Epoch;
	Duration m_Interval;
	bool m_IsTiming;

public:
	inline TaskTimer(const Duration& interval = Duration{ 0 }, 
					 const ClockType& clock = Epic::detail::DefaultClock<ClockType>::Get()) noexcept
		: Base(), m_Clock{ clock }, m_IsTiming{ false }, m_Interval{ interval }
	{ }

public:
	// Get the timer interval
	inline Duration GetInterval() const noexcept
	{
		return m_Interval;
	}

	// Set the timer interval
	inline void SetInterval(const Duration& interval) noexcept
	{
		m_Interval = interval;
	}

	// Returns whether or not the timer is currently timing
	inline bool IsTiming() const noexcept
	{
		return m_IsTiming;
	}

	// Start the timer
	inline void Start() noexcept
	{
		m_Epoch = m_Clock.Now();
		m_IsTiming = true;
	}

	// Stop the timer
	inline void Stop() noexcept
	{
		m_IsTiming = false;
	}

	// Update the timer
	void Update() noexcept final
	{
		if (m_IsTiming)
		{
			if (m_Clock.Elapsed(m_Epoch, m_Clock.Now()) > m_Interval)
			{
				Tick();
				Stop();
			}
		}
	}
};

//////////////////////////////////////////////////////////////////////////////

// PeriodicTimer
template<class C>
class Epic::PeriodicTimer : public Epic::detail::AutoTimer
{
public:
	using Type = Epic::PeriodicTimer<C>;
	using Base = Epic::detail::AutoTimer;

public:
	using ClockType = C;
	using TimeStamp = typename ClockType::TimeStamp;
	using Duration = typename ClockType::Unit;

public:
	using TickDelegate = Epic::Event<void()>;

public:
	TickDelegate Tick;

private:
	const ClockType& m_Clock;
	TimeStamp m_Epoch;
	Duration m_Interval;
	bool m_IsTiming;

public:
	inline PeriodicTimer(const Duration& interval = Duration{ 0 }, 
						 const ClockType& clock = Epic::detail::DefaultClock<ClockType>::Get()) noexcept
		: Base(), m_Clock{ clock }, m_IsTiming{ false }, m_Interval{ interval }
	{ }

public:
	// Get the timer interval
	inline Duration GetInterval() const noexcept
	{
		return m_Interval;
	}

	// Set the timer interval
	inline void SetInterval(const Duration& interval) noexcept
	{
		m_Interval = interval;
	}

	// Returns whether or not the timer is currently timing
	inline bool IsTiming() const noexcept
	{
		return m_IsTiming;
	}

	// Start the timer
	inline void Start() noexcept
	{
		m_Epoch = m_Clock.Now();
		m_IsTiming = true;
	}

	// Stop the timer
	inline void Stop() noexcept
	{
		m_IsTiming = false;
	}

	// Update the timer
	void Update() noexcept final
	{
		if (m_IsTiming)
		{
			auto delta = m_Clock.Elapsed(m_Epoch, m_Clock.Now());

			while (delta >= m_Interval)
			{
				m_Epoch += m_Interval;
				delta -= m_Interval;

				Tick();

				if (m_Interval == Duration{ 0 })
					break;
			}
		}
	}
};

//////////////////////////////////////////////////////////////////////////////

// DiscreteTimer
template<class C>
class Epic::DiscreteTimer
{
public:
	using Type = Epic::DiscreteTimer<C>;
	
public:
	using ClockType = C;
	using TimeStamp = typename ClockType::TimeStamp;
	using Duration = typename ClockType::Unit;

public:
	using TickDelegate = Epic::Event<void()>;

public:
	TickDelegate Tick;

private:
	const ClockType& m_Clock;
	TimeStamp m_Epoch;
	Duration m_Interval;
	bool m_IsPeriodic;
	bool m_IsTiming;

public:
	inline DiscreteTimer(bool isPeriodic = false, 
						 const Duration& interval = Duration{ 0 },
						 const ClockType& clock = Epic::detail::DefaultClock<ClockType>::Get()) noexcept
		: m_Clock{ clock }, m_IsTiming{ false }, m_IsPeriodic{ isPeriodic }, m_Interval{ interval }
	{ }

public:
	// Get whether or not this timer is periodic
	inline bool IsPeriodic() const noexcept
	{
		return m_IsPeriodic;
	}

	// Set whether or not this timer is periodic
	inline void SetIsPeriodic(bool isPeriodic) noexcept
	{
		m_IsPeriodic = isPeriodic;
	}

	// Get the timer interval
	inline Duration GetInterval() const noexcept
	{
		return m_Interval;
	}

	// Set the timer interval
	inline void SetInterval(const Duration& interval) noexcept
	{
		m_Interval = interval;
	}

	// Returns whether or not the timer is currently timing
	inline bool IsTiming() const noexcept
	{
		return m_IsTiming;
	}

	// Start the timer
	inline void Start() noexcept
	{
		m_Epoch = m_Clock.Now();
		m_IsTiming = true;
	}

	// Stop the timer
	inline void Stop() noexcept
	{
		m_IsTiming = false;
	}

	// Update the timer
	void Update() noexcept
	{
		if (m_IsTiming)
		{
			auto delta = m_Clock.Elapsed(m_Epoch, m_Clock.Now());
			
			if (m_IsPeriodic)
			{
				// Repeating (periodic) timer
				while (delta >= m_Interval)
				{
					m_Epoch += m_Interval;
					delta -= m_Interval;

					Tick();

					if (m_Interval == Duration{ 0 })
						break;
				}
			}
			else
			{
				// Non-repeating (task) timer
				if (delta >= m_Interval)
				{
					Tick();
					Stop();
				}
			}
		}
	}
};

//////////////////////////////////////////////////////////////////////////////

// Timers
namespace Epic::Timers
{
	namespace
	{
		inline void Update() noexcept
		{
			auto itFn = [&](auto pInstance) { pInstance->Update(); };
			Epic::detail::AutoTimer::IterateInstancesSafe(itFn);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////

// Timer Aliases
namespace Epic
{
	using StandardTaskTimer = Epic::TaskTimer<decltype(Epic::StandardClock)>;
	using StandardPeriodicTimer = Epic::PeriodicTimer<decltype(Epic::StandardClock)>;
	using StandardDiscreteTimer = Epic::DiscreteTimer<decltype(Epic::StandardClock)>;

	using HighResolutionTaskTimer = Epic::TaskTimer<decltype(Epic::HighResolutionClock)>;
	using HighResolutionPeriodicTimer = Epic::PeriodicTimer<decltype(Epic::HighResolutionClock)>;
	using HighResolutionDiscreteTimer = Epic::DiscreteTimer<decltype(Epic::HighResolutionClock)>;
}
