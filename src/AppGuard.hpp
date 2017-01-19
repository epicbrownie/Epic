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

#include <Epic/STL/String.hpp>
#include <Epic/STL/UniquePtr.hpp>
#include <boost/interprocess/sync/named_semaphore.hpp>
#include <boost/asio.hpp>
#include <stdexcept>
#include <thread>

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	class AppGuard;

	struct SemaphoreCreationFailedException;
}

//////////////////////////////////////////////////////////////////////////////

struct Epic::SemaphoreCreationFailedException : public std::runtime_error
{
	using std::runtime_error::runtime_error;
};

//////////////////////////////////////////////////////////////////////////////

// AppGuard
class Epic::AppGuard
{
public:
	AppGuard() = delete;
	AppGuard(const AppGuard&) = delete;
	AppGuard(AppGuard&&) = delete;

public:
	AppGuard(const char* name, unsigned int allowedInstances = 1) 
	try : m_IOService{ },
			m_Signals{ m_IOService },
			m_IsLocked{ false },
			m_Semaphore{ boost::interprocess::open_or_create, name, allowedInstances }
	{
		if (m_IsLocked = m_Semaphore.try_wait())
		{
			m_Signals.add(SIGINT);
			m_Signals.add(SIGTERM);
			m_Signals.add(SIGABRT_COMPAT);
		
			m_Signals.async_wait(
				[&] (boost::system::error_code, int)
				{
					m_Semaphore.post();
				});

			m_pSignalListener = Epic::MakeUnique<std::thread>([&] { m_IOService.run(); });
		}
	}
	catch (boost::interprocess::interprocess_exception&)
	{
		throw Epic::SemaphoreCreationFailedException("Failed to construct named semaphore.");
	}
	
	~AppGuard()
	{
		if (m_IsLocked)
		{
			m_IOService.post([&] { m_Signals.cancel(); });

			if (m_pSignalListener)
			{
				m_pSignalListener->join();
				m_pSignalListener.reset();
			}
		}
	}

	AppGuard& operator = (const AppGuard&) = delete;
	AppGuard& operator = (AppGuard&&) = delete;

public:
	explicit constexpr operator bool() const
	{
		return m_IsLocked;
	}

private:
	boost::interprocess::named_semaphore m_Semaphore;
	boost::asio::io_service m_IOService;
	boost::asio::signal_set m_Signals;
	Epic::UniquePtr<std::thread> m_pSignalListener;
	bool m_IsLocked;
};

