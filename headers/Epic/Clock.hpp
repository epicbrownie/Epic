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

#include <chrono>

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	template<class Unit = std::chrono::microseconds, class ClockType = std::chrono::high_resolution_clock>
	class Clock;

	namespace detail
	{
		template<class ClockType>
		struct DefaultClock;
	}
}

//////////////////////////////////////////////////////////////////////////////

// Clock
template<class U, class C>
class Epic::Clock
{
public:
	using ClockType = C;
	using Unit = U;
	using TimeStamp = U;
	
private:
	using TimePoint = std::chrono::time_point<ClockType>;

public:
	static constexpr bool IsSteady = ClockType::is_steady;

public:
	inline Clock() noexcept
	{
		Reset();
	}

public:
	// Get the unit difference between two timestamps
	inline Unit Elapsed(const TimeStamp& start, const TimeStamp& end) const noexcept
	{
		return std::chrono::duration_cast<Unit>(end - start);
	}

public:
	// Get the current timestamp
	inline TimeStamp Now() const noexcept
	{
		return std::chrono::duration_cast<Unit>(ClockType::now() - m_Epoch);
	}

	// Get the timestamp from the last call to Update()
	inline Unit FrameTime() const noexcept
	{
		return m_FrameTime;
	}

	// Get the amount of time that passed between the last two calls to Update()
	inline Unit DeltaTime() const noexcept
	{
		return m_DeltaTime;
	}

	// Reset the internal epoch (Now() and GetFrameTime() are both relative to this internal epoch)
	inline void Reset() noexcept
	{
		m_FrameTime = m_DeltaTime = Unit{ 0 };
		m_Epoch = ClockType::now();
		m_PrevFrameTimePoint = m_FrameTimePoint = m_Epoch;
	}

	// Update timestamps
	inline void Update() noexcept
	{
		m_PrevFrameTimePoint = m_FrameTimePoint;
		m_FrameTimePoint = ClockType::now();

		m_FrameTime = std::chrono::duration_cast<Unit>(m_FrameTimePoint - m_Epoch);
		m_DeltaTime = std::chrono::duration_cast<Unit>(m_FrameTimePoint - m_PrevFrameTimePoint);
	}

private:
	TimePoint m_Epoch, m_FrameTimePoint, m_PrevFrameTimePoint;
	Unit m_FrameTime, m_DeltaTime;
};

//////////////////////////////////////////////////////////////////////////////

// Aliases
namespace Epic
{
	using MicroClock = Epic::Clock<std::chrono::microseconds, std::chrono::high_resolution_clock>;
	using MilliClock = Epic::Clock<std::chrono::milliseconds, std::chrono::high_resolution_clock>;
}

//////////////////////////////////////////////////////////////////////////////

// Externals
namespace Epic
{
	extern MilliClock StandardClock;
	extern MicroClock HighResolutionClock;
}

//////////////////////////////////////////////////////////////////////////////

// DefaultClock<ClockType>
template<class ClockType>
struct Epic::detail::DefaultClock
{
	static inline ClockType& Get() noexcept
	{
		return ClockType();
	}
};

// DefaultClock<StandardClock>
template<>
struct Epic::detail::DefaultClock<decltype(Epic::StandardClock)>
{
	static inline decltype(Epic::StandardClock)& Get() noexcept
	{
		return Epic::StandardClock;
	}
};

// DefaultClock<HighResolutionClock>
template<>
struct Epic::detail::DefaultClock<decltype(Epic::HighResolutionClock)>
{
	static inline decltype(Epic::HighResolutionClock)& Get() noexcept
	{
		return Epic::HighResolutionClock;
	}
};

//////////////////////////////////////////////////////////////////////////////

static_assert(decltype(Epic::HighResolutionClock)::IsSteady, "HighResolutionClock must use a monotonic clock");
static_assert(decltype(Epic::StandardClock)::IsSteady, "StandardClock must use a monotonic clock");
