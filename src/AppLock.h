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
	class AppLock;

	struct SemaphoreCreationFailed;
}

//////////////////////////////////////////////////////////////////////////////

// AppLock
class Epic::AppLock
{
public:
	AppLock() = delete;
	AppLock(const char* name, unsigned int allowedInstances = 1);

	AppLock(const AppLock&) = delete;
	AppLock(AppLock&&) = delete;

	~AppLock();

	AppLock& operator = (const AppLock&) = delete;
	AppLock& operator = (AppLock&&) = delete;

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

//////////////////////////////////////////////////////////////////////////////

struct Epic::SemaphoreCreationFailed : public std::runtime_error
{
	explicit SemaphoreCreationFailed(const Epic::STLString<>& what) 
		: std::runtime_error{ what.c_str() } { };
	
	explicit SemaphoreCreationFailed(const char* what) 
		: std::runtime_error{ what } { };
};
