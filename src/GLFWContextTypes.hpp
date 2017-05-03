//////////////////////////////////////////////////////////////////////////////
//
//            Copyright (c) 2017 Ronnie Brohn (EpicBrownie)      
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

#include <cstdint>

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	struct GLFWContextSettings;
}

//////////////////////////////////////////////////////////////////////////////

// GLFWContextSettings
struct Epic::GLFWContextSettings
{
	GLFWContextSettings() noexcept
		: WaitForRefresh{ true },
		  RedBits{ 8 }, GreenBits{ 8 }, BlueBits{ 8 }, AlphaBits{ 8 },
		  DepthBits{ 24 }, StencilBits{ 8 },
		  MultiSamples{ 0 },
		  VersionMajor{ 1 }, VersionMinor{ 0 }
	{ }

	bool WaitForRefresh;								// Default: true
	uint8_t RedBits, GreenBits, BlueBits, AlphaBits;	// Default: 8
	uint8_t DepthBits;									// Default: 24
	uint8_t StencilBits;								// Default: 8
	uint16_t MultiSamples;								// Default: 0
	int32_t VersionMajor;								// Default: 1
	int32_t VersionMinor;								// Default: 0
};
