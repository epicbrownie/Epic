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

#include <Epic/StateTypes.hpp>
#include <Epic/detail/StateSystemFwd.hpp>

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	class State;
}

//////////////////////////////////////////////////////////////////////////////

// State
class Epic::State
{
public:
	using Type = Epic::State;
	
private:
	friend class Epic::StateSystem;

private:
	// This member will be set by StateSystem owner
	Epic::StateSystem* m_pStateSystem;

public:
	constexpr State() noexcept : m_pStateSystem{ nullptr } { }

	State(const Type&) = delete;
	Type& operator= (const Type&) = delete;

	State(Type&&) = default;
	Type& operator= (Type&&) = default;
	
	virtual ~State() { }

public:
	constexpr Epic::StateSystem* GetStateSystem() const noexcept
	{
		return m_pStateSystem;
	}

public:
	virtual void Enter() { }
	virtual void Leave() { }

	virtual void EnterForeground() { }
	virtual void LeaveForeground() { }

	virtual void Update() = 0;
};
