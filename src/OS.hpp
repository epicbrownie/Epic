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

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	enum class eOS { Windows, Linux, Mac };
	enum class eArchitecture { x86, x64 };
}

//////////////////////////////////////////////////////////////////////////////

// Detect current Operating System target amongst supported systems
namespace Epic
{
	#if defined(_WIN64)
		constexpr eOS OS = eOS::Windows;
		constexpr eArchitecture Architecture = eArchitecture::x64;
		#define WindowsOS
	#elif defined(_WIN32)
		constexpr eOS OS = eOS::Windows;
		constexpr eArchitecture Architecture = eArchitecture::x86;
		#define WindowsOS
	#elif defined(WIN32)
		constexpr eOS OS = eOS::Windows;
		constexpr eArchitecture Architecture = eArchitecture::x86;
		#define WindowsOS
	#elif defined(__WIN32__)
		constexpr eOS OS = eOS::Windows;
		constexpr eArchitecture Architecture = eArchitecture::x86;
		#define WindowsOS
	#elif defined(__Win32__)
		constexpr eOS OS = eOS::Windows;
		constexpr eArchitecture Architecture = eArchitecture::x86;
		#define WindowsOS
	#elif defined(_WINDOWS)
		constexpr eOS OS = eOS::Windows;
		constexpr eArchitecture Architecture = eArchitecture::x86;
		#define WindowsOS
	#elif defined(__INTEL__) && defined(__MWERKS__)		// Windows 32-bit
		// Metrowerks CodeWarrior doesn't build anything other than Win32 on INTEL, no DOS
		constexpr eOS OS = eOS::Windows;
		constexpr eArchitecture Architecture = eArchitecture::x86;
		#define WindowsOS
	#endif

	// If not a DOS machine by now, may be Mac or Unix
	// cope with Metrowerks and Symantec (and MPW?)
	#ifndef MacOS
		#ifdef macintosh
			constexpr eOS OS = eOS::Mac;
			constexpr eArchitecture Architecture = eArchitecture::x86; // Unknown
			#define MacOS
			static_assert(false, "Mac is not supported at this time.");
		#endif
	#endif

	// Assume a unix system
	#if !defined(MacOS) && !defined(WindowsOS)
		constexpr eOS OS = eOS::Linux;
		constexpr eArchitecture Architecture = eArchitecture::x86; // Unknown
		#define LinuxOS
		static_assert(false, "Unix is not supported at this time.");
	#endif
}

//////////////////////////////////////////////////////////////////////////////

// Windows headers
#ifdef WindowsOS
	#define WIN32_LEAN_AND_MEAN

	#ifndef NOMINMAX
		#define NOMINMAX
	#endif

	#ifndef STRICT
		// some Windows headers define STRICT. In Visual C++ at least having it defined
		// affects how static member signatures are mangled, so we define it up front  
		#define STRICT
	#endif

	// Detect inline assembly support
	#ifdef _WIN64
		#define NO_ASM	// x64 compiler does not support ASM
	#endif

	#include <Windows.h>
	#include <WindowsX.h>
	#include <intrin.h>
	#include <objbase.h>

	#ifndef RI_MOUSE_HORIZONTAL_WHEEL
		#define RI_MOUSE_HORIZONTAL_WHEEL 0x0800
	#endif
#endif
