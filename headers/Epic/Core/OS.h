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

// Detect current Operating System target amongst supported systems
#ifndef WindowsOS
	#if defined(_WIN64)
		#define _Win64									// Windows 64-bit
		#define WindowsOS
	#elif defined(_WIN32)
		#define _Win32									// Windows 32-bit
		#define WindowsOS
	#elif defined(WIN32)
		#define _Win32									// Windows 32-bit
		#define WindowsOS
	#elif defined(__WIN32__)							// Windows 32-bit
		#define _Win32
		#define WindowsOS
	#elif defined(__Win32__)							// Windows 32-bit
		#define _Win32
		#define WindowsOS
	#elif defined(_WINDOWS)								// Windows
		#define WindowsOS
	#elif defined(__INTEL__) && defined(__MWERKS__)		// Windows 32-bit
		// Metrowerks CodeWarrior doesn't build anything other than Win32 on INTEL, no DOS
		#define WindowsOS
		#define _Win32
	#endif
#else
	#if defined __Win32__ || defined _WIN32
		#ifndef _Win32
			#define _Win32
		#endif
	#endif
#endif

// If not a DOS machine by now, may be Mac or Unix
// cope with Metrowerks and Symantec (and MPW?)
#ifndef MacOS
	#ifdef macintosh
		#define _Macintosh
		static_assert(false, "Mac is not supported at this time.");
	#endif
#endif

// Assume a unix system
#if !defined(MacOS) && !defined(WindowsOS)
	#define _Unix
	static_assert(false, "Unix is not supported at this time.");
#endif

//////////////////////////////////////////////////////////////////////////////

// Operating system consts
#ifdef WindowsOS
	static constexpr bool _IsWindows = true;
	static constexpr bool _IsUnix = false;
	static constexpr bool _IsMac = false;
#elif defined(MacOS)
	static_assert(false, "Dont forget to define _DEBUG in debug builds");
	static constexpr bool _IsWindows = false;
	static constexpr bool _IsUnix = false;
	static constexpr bool _IsMac = true;
#else
	static_assert(false, "Dont forget to define _DEBUG in debug builds");
	static constexpr bool _IsWindows = false;
	static constexpr bool _IsUnix = true;
	static constexpr bool _IsMac = false;
#endif

//////////////////////////////////////////////////////////////////////////////

// Windows headers
#ifdef WindowsOS
	#define WIN32_LEAN_AND_MEAN
	#define NOMINMAX

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

	#ifndef RI_MOUSE_HORIZONTAL_WHEEL
	#define RI_MOUSE_HORIZONTAL_WHEEL 0x0800
	#endif
#endif
