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
#include <Epic/StringHash.hpp>
#include <Epic/detail/AudioParameterList.hpp>
#include <Epic/detail/FMODInclude.hpp>
#include <chrono>
#include <memory>

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	class Sound;
}

//////////////////////////////////////////////////////////////////////////////

// Sound
class Epic::Sound
{
public:
	using Type = Epic::Sound;

private:
	struct InstanceRelease
	{
		void operator() (FMOD::Studio::EventInstance* pInstance) const
		{
			if (pInstance)
				pInstance->release();
		}
	};

private:
	using InstancePtr = std::unique_ptr<FMOD::Studio::EventInstance, InstanceRelease>;

private:
	InstancePtr m_pInstance;
	FMOD::Studio::EventDescription* m_pDesc;

public:
	Epic::detail::AudioParameterList Parameters;

public:
	explicit Sound(FMOD::Studio::EventDescription* pDesc) noexcept
		: m_pDesc{ pDesc }, m_pInstance{ nullptr }
	{
		// Create instance
		FMOD::Studio::EventInstance* pInstance;
		FMODCHECK(m_pDesc->createInstance(&pInstance));
		m_pInstance.reset(pInstance);

		// Load parameters
		Parameters.Initialize(pDesc, pInstance);
	}

public:
	inline void Start() noexcept
	{
		if (m_pInstance)
			FMODCHECK(m_pInstance->start());
	}

	inline void Stop(bool stopImmediately = true) noexcept
	{
		if (m_pInstance)
		{
			FMODCHECK(m_pInstance->stop(stopImmediately
				? FMOD_STUDIO_STOP_IMMEDIATE
				: FMOD_STUDIO_STOP_ALLOWFADEOUT));
		}
	}

	inline bool IsPaused() const noexcept
	{
		bool value = true;

		if (m_pInstance)
			FMODCHECK(m_pInstance->getPaused(&value));

		return value;
	}

	inline void Pause() noexcept
	{
		if (m_pInstance)
			FMODCHECK(m_pInstance->setPaused(true));
	}

	inline void Unpause() noexcept
	{
		if (m_pInstance)
			FMODCHECK(m_pInstance->setPaused(false));
	}

	inline void TogglePause() noexcept
	{
		if (m_pInstance)
		{
			bool isPaused;
			FMODCHECK(m_pInstance->getPaused(&isPaused));
			FMODCHECK(m_pInstance->setPaused(!isPaused));
		}
	}

public:
	inline Epic::eSoundStatus GetStatus() const noexcept
	{
		if (m_pInstance)
		{
			FMOD_STUDIO_PLAYBACK_STATE status;
			FMODCHECK(m_pInstance->getPlaybackState(&status));

			switch (status)
			{
				case FMOD_STUDIO_PLAYBACK_PLAYING: return Epic::eSoundStatus::Playing;
				case FMOD_STUDIO_PLAYBACK_SUSTAINING: return Epic::eSoundStatus::Sustaining;
				case FMOD_STUDIO_PLAYBACK_STOPPED: return Epic::eSoundStatus::Stopped;
				case FMOD_STUDIO_PLAYBACK_STARTING:	return Epic::eSoundStatus::Starting;
				case FMOD_STUDIO_PLAYBACK_STOPPING:	return Epic::eSoundStatus::Stopping;
				default: break;
			}
		}

		return Epic::eSoundStatus::Invalid;
	}

	inline float GetPitch() const noexcept
	{
		if (m_pInstance)
		{
			float pitch = 0.0f;
			FMODCHECK(m_pInstance->getPitch(&pitch));
			return pitch;
		}

		return 0.0f;
	}
	
	inline float GetVolume() const noexcept
	{
		if (m_pInstance)
		{
			float volume = 0.0f;
			FMODCHECK(m_pInstance->getVolume(&volume));
			return volume;
		}

		return 0.0f;
	}
	
	inline std::chrono::milliseconds GetTimelinePosition() const noexcept
	{
		if (m_pInstance)
		{
			int pos = 0;
			FMODCHECK(m_pInstance->getTimelinePosition(&pos));
			return std::chrono::milliseconds(pos);
		}

		return std::chrono::milliseconds(0);
	}

	inline void SetPitch(float pitch) noexcept
	{
		if (m_pInstance)
			FMODCHECK(m_pInstance->setPitch(pitch));
	}

	inline void SetVolume(float volume) noexcept
	{
		if (m_pInstance)
			FMODCHECK(m_pInstance->setVolume(volume));
	}

	inline void SetTimelinePosition(const std::chrono::milliseconds& position) noexcept
	{
		if (m_pInstance)
		{
			FMODCHECK(m_pInstance->setTimelinePosition(
				static_cast<int>(position.count())));
		}
	}
};
