#include "AppLock.h"

Epic::AppLock::AppLock(const char* name, unsigned int allowedInstances)
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
	throw Epic::SemaphoreCreationFailed("Failed to construct named semaphore.");
}

Epic::AppLock::~AppLock()
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
