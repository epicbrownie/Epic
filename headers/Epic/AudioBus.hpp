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

#include <Epic/AudioTypes.hpp>
#include <Epic/detail/FMODInclude.hpp>
#include <algorithm>
#include <cassert>

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	class AudioBus;
}

//////////////////////////////////////////////////////////////////////////////

// AudioBus
class Epic::AudioBus
{
public:
	using Type = Epic::AudioBus;

private:
	FMOD::Studio::Bus* m_pBus;

public:
	inline explicit AudioBus(FMOD::Studio::Bus* pBus) noexcept
		: m_pBus{ pBus }
	{
		assert(m_pBus);
	}

	constexpr AudioBus(const Type& other) noexcept = default;

public:	
	inline bool IsMuted() const noexcept
	{
		bool value;
		FMODCHECK(m_pBus->getMute(&value));
		
		return value;
	}

	inline void Mute() noexcept
	{
		FMODCHECK(m_pBus->setMute(true));
	}

	inline void Unmute() noexcept
	{
		FMODCHECK(m_pBus->setMute(false));
	}

	inline void ToggleMute() noexcept
	{
		bool isMute;
		FMODCHECK(m_pBus->getMute(&isMute));
		FMODCHECK(m_pBus->setMute(!isMute));
	}

public:
	inline bool IsPaused() const noexcept
	{
		bool value;
		FMODCHECK(m_pBus->getPaused(&value));

		return value;
	}

	inline void Pause() noexcept
	{
		FMODCHECK(m_pBus->setPaused(true));
	}

	inline void Unpause() noexcept
	{
		FMODCHECK(m_pBus->setPaused(false));
	}

	inline void TogglePause() noexcept
	{
		bool isPaused;
		FMODCHECK(m_pBus->getPaused(&isPaused));
		FMODCHECK(m_pBus->setPaused(!isPaused));
	}

public:
	inline void StopAll(bool stopImmediately = true) noexcept
	{
		FMODCHECK(m_pBus->stopAllEvents(stopImmediately
				? FMOD_STUDIO_STOP_IMMEDIATE
				: FMOD_STUDIO_STOP_ALLOWFADEOUT));
	}

public:
	inline float GetVolume() const noexcept
	{
		float value;
		FMODCHECK(m_pBus->getVolume(&value));

		return value;
	}

	inline void SetVolume(float volume) noexcept
	{
		float value = std::min(std::max(0.0f, volume), 1.0f);
		FMODCHECK(m_pBus->setVolume(value));
	}
};
