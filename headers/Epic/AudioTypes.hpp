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

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	enum class eSpeakerMode
	{
		Default = 0,
		Mono,
		Stereo,
		Quad,
		Surround,
		FiveOne,
		SevenOne
	};

	enum class eSoundStatus
	{
		Invalid = 0,
		Playing,
		Sustaining,
		Stopped,
		Starting,
		Stopping
	};
}
