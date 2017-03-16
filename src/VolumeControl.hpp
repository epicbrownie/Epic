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

#include <Epic/detail/FMODInclude.hpp>
#include <algorithm>
#include <cassert>

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	class VolumeControl;
}

//////////////////////////////////////////////////////////////////////////////

// VolumeControl
class Epic::VolumeControl
{
public:
	using Type = Epic::VolumeControl;

private:
	FMOD::Studio::VCA* m_pVCA;

public:
	inline explicit VolumeControl(FMOD::Studio::VCA* pVCA) noexcept
		: m_pVCA{ pVCA }
	{
		assert(m_pVCA);
	}

	constexpr VolumeControl(const Type& other) noexcept = default;

public:
	inline Type& operator = (const Type& other) noexcept = default;
	
	inline Type& operator = (float value) noexcept
	{
		Set(value);
		return *this;
	}

	inline operator float() const noexcept
	{
		return Get();
	}

	inline explicit operator bool() const noexcept
	{
		return !IsMuted();
	}

public:
	inline bool IsMuted() const noexcept
	{
		return Get() == MinValue();
	}

	constexpr float MinValue() const
	{
		return 0.0f;
	}

	constexpr float MaxValue() const
	{
		return 1.0f;
	}

	inline float GetFinalVolume() const
	{
		float value;
		FMODCHECK(m_pVCA->getVolume(nullptr, &value));

		return value;
	}

	inline float Get() const noexcept
	{
		float value;
		FMODCHECK(m_pVCA->getVolume(&value));

		return value;
	}

	inline void Set(float value) noexcept
	{
		float volume = std::min(std::max(MinValue(), value), MaxValue());
		FMODCHECK(m_pVCA->setVolume(value));
	}
};
