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

#include <Epic/Core/OS.h>
#include <Epic/STL/UniquePtr.hpp>
#include <Epic/STL/Vector.hpp>
#include <cstdint>
#include <direct.h>
#include <intrin.h>
#include <memory>

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	struct CPUCaps;
	struct CPUSpecs;
	struct DriveCaps;
	struct DriveSpecs;
	struct MemorySpecs;
}

//////////////////////////////////////////////////////////////////////////////

// CPUCaps
struct Epic::CPUCaps
{
	uint64_t uiCPUSpeed;
};

//////////////////////////////////////////////////////////////////////////////

// CPUSpecs
struct Epic::CPUSpecs
{
	Epic::UniquePtr<CPUCaps[]> pCPUCaps;
	uint32_t uiCPUCount;

	inline CPUSpecs() = default;

	inline CPUSpecs(const CPUSpecs& other) noexcept
		: uiCPUCount{ other.uiCPUCount }
	{
		pCPUCaps = Epic::MakeUnique<Epic::CPUCaps[]>(uiCPUCount);

		for (uint32_t i = 0; i < uiCPUCount; ++i)
			pCPUCaps[i] = other.pCPUCaps[i];
	}

	inline CPUSpecs& operator = (const CPUSpecs& other) noexcept
	{
		if (this != &other)
		{
			uiCPUCount = other.uiCPUCount;
			pCPUCaps = Epic::MakeUnique<Epic::CPUCaps[]>(uiCPUCount);

			for (uint32_t i = 0; i < uiCPUCount; ++i)
				pCPUCaps[i] = other.pCPUCaps[i];
		}

		return *this;
	}
};

//////////////////////////////////////////////////////////////////////////////

// DriveCaps
struct Epic::DriveCaps
{
	uint64_t uiAvailableBytes, uiCapacityBytes;
	uint32_t uiDriveIndex;
	bool bIsAvailable, bIsReady;
	char chDriveLetter;
};

//////////////////////////////////////////////////////////////////////////////

// DriveSpecs
struct Epic::DriveSpecs
{
	uint32_t uiDriveCount, uiDefaultDrive;
	DriveCaps pDriveCaps[26];
};

//////////////////////////////////////////////////////////////////////////////

// MemorySpecs
struct Epic::MemorySpecs
{
	uint64_t uiTotalPhysicalMem, uiAvailPhysicalMem;
	uint64_t uiTotalVirtualMem, uiAvailVirtualMem;
	uint64_t uiTotalPagingMem, uiAvailPagingMem;
	uint64_t uiExtendedMem;
	uint32_t uiMemoryLoad;
};

//////////////////////////////////////////////////////////////////////////////

// Specification query functions
namespace Epic
{
	namespace
	{
		/// @fn DriveSpecs GetDriveSpecs() noexcept
		/// @brief	Gets drive specs
		///
		/// @return	The drive specs
		///
		Epic::DriveSpecs GetDriveSpecs() noexcept
		{
			Epic::DriveSpecs ds;
			ds.uiDriveCount = 0;

			// Set the current drive
			ds.uiDefaultDrive = _getdrive() - 1; // _getdrive() = [1, 26]
			unsigned long dwDriveMask = _getdrives();

			// Calculate the 26 possible drives
			for (unsigned int i = 0; i < 26; ++i)
			{
				// Parameters that exist for all drives, present or not
				ds.pDriveCaps[i].chDriveLetter = (char)('A' + i);
				ds.pDriveCaps[i].uiDriveIndex = i + 1;

				// Is this drive available?
				if ((dwDriveMask & 0x1) == 0)
				{
					ds.pDriveCaps[i].uiAvailableBytes = 0;
					ds.pDriveCaps[i].uiCapacityBytes = 0;
					ds.pDriveCaps[i].bIsAvailable = false;
					ds.pDriveCaps[i].bIsReady = false;
					dwDriveMask >>= 1;
					continue;
				}
				else
				{
					// The drive is present
					++ds.uiDriveCount;
				}

				// Shift our mask so we can detect our next drive
				dwDriveMask >>= 1;

				// Get free space on this drive
				struct _diskfree_t diskfree;
				unsigned int rv = 0;

				try
				{
					rv = _getdiskfree(i + 1, &diskfree);
				}
				catch (...)
				{
					rv = 1;
				}

				if (rv == 0)
				{
					// Convert it into bytes
					ds.pDriveCaps[i].uiAvailableBytes =
						static_cast<uint64_t>(diskfree.avail_clusters) *
						static_cast<uint64_t>(diskfree.sectors_per_cluster) *
						static_cast<uint64_t>(diskfree.bytes_per_sector);
					ds.pDriveCaps[i].uiCapacityBytes =
						static_cast<uint64_t>(diskfree.total_clusters) *
						static_cast<uint64_t>(diskfree.sectors_per_cluster) *
						static_cast<uint64_t>(diskfree.bytes_per_sector);
					ds.pDriveCaps[i].bIsAvailable = true;
					ds.pDriveCaps[i].bIsReady = true;
				}
				else
				{
					// Failed to get drive space, the drive is not ready
					ds.pDriveCaps[i].uiAvailableBytes = 0;
					ds.pDriveCaps[i].uiCapacityBytes = 0;
					ds.pDriveCaps[i].bIsAvailable = true;
					ds.pDriveCaps[i].bIsReady = false;
				}
			}

			return ds;
		}

		/// @fn MemorySpecs GetMemorySpecs()
		/// @brief	Gets memory specs
		///
		/// @return	The memory specs
		///
		Epic::MemorySpecs GetMemorySpecs() noexcept
		{
			Epic::MemorySpecs ms;

			// This is OS specific
			#ifdef WindowsOS
				MEMORYSTATUSEX status;
				status.dwLength = sizeof(status);
				GlobalMemoryStatusEx(&status);

				ms.uiTotalPhysicalMem = status.ullTotalPhys;
				ms.uiAvailPhysicalMem = status.ullAvailPhys;
				ms.uiTotalVirtualMem = status.ullTotalVirtual;
				ms.uiAvailVirtualMem = status.ullAvailVirtual;
				ms.uiTotalPagingMem = status.ullTotalPageFile;
				ms.uiAvailPagingMem = status.ullAvailPageFile;
				ms.uiExtendedMem = status.ullAvailExtendedVirtual;
				ms.uiMemoryLoad = status.dwMemoryLoad;
			#else
				#error GetMemorySpecs() is not implemented for this OS				
			#endif

			return ms;
		}

		/// @fn CPUSpecs GetCPUSpecs()
		/// @brief	Gets CPU specs
		///
		/// @return	The CPU specs
		///
		Epic::CPUSpecs GetCPUSpecs() noexcept
		{
			Epic::CPUSpecs cs;
			cs.uiCPUCount = 0;

			#ifdef WindowsOS
				HKEY hKey;
				DWORD dwSpeed;
				wchar_t szKey[256];

				DWORD dwCalculated = 0;		// The current key # to open
				Epic::STLVector<DWORD> lstSpeeds;

				// Keep trying to read processor speeds until we fail
				while (true)
				{
					// Get the key name
					#pragma warning (disable : 4996)
					_snwprintf(szKey, sizeof(szKey) / sizeof(wchar_t),
						L"HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\%d\\", dwCalculated);
					#pragma warning (default : 4996)

					// Open the key
					if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,
						szKey, 0, KEY_QUERY_VALUE, &hKey) != ERROR_SUCCESS)
					{
						// Failed to open the key.
						break;
					}

					// Read the value
					DWORD dwLen = 4;
					if (RegQueryValueEx(hKey, L"~MHz", NULL, NULL,
						(LPBYTE)&dwSpeed, &dwLen) != ERROR_SUCCESS)
					{
						// Failed to read speed.
						RegCloseKey(hKey);
						break;
					}

					// Success
					RegCloseKey(hKey);
					lstSpeeds.emplace_back(dwSpeed);
					++dwCalculated;
				}

				// Now create our caps
				cs.uiCPUCount = dwCalculated;
				cs.pCPUCaps = Epic::MakeUnique<Epic::CPUCaps[]>(dwCalculated);

				// Set the caps
				auto it = std::begin(lstSpeeds);
				for (DWORD c = 0; c < dwCalculated; ++c, ++it)
					cs.pCPUCaps.get()[c].uiCPUSpeed = *it;
			#else
				#error GetCPUSpecs() isn't implemented on this OS
			#endif

			return cs;
		}
	}
}
