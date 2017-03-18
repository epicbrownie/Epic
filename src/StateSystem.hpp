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

#include <Epic/State.hpp>
#include <Epic/StateTypes.hpp>
#include <Epic/StringHash.hpp>
#include <Epic/STL/Map.hpp>
#include <Epic/STL/Vector.hpp>
#include <Epic/STL/UniquePtr.hpp>
#include <Epic/detail/StateSystemFwd.hpp>

//////////////////////////////////////////////////////////////////////////////

// StateSystem
class Epic::StateSystem
{
public:
	using Type = Epic::StateSystem;

private:
	enum class eStateSystemCommand { Push, Pop, Change };

	struct StateSystemCommand
	{
		constexpr StateSystemCommand(eStateSystemCommand command, 
									 StateName target = InvalidStateName) noexcept
			: CommandType{ command }, Target{ target }
		{ }

		eStateSystemCommand CommandType;
		StateName Target;
	};

private:
	using StatePtr = Epic::UniquePtr<Epic::State>;
	using StateMap = Epic::STLUnorderedMap<Epic::StringHash, StatePtr>;
	using StateStack = Epic::STLVector<StatePtr::pointer>;
	using StateCommandBuffer = Epic::STLVector<StateSystemCommand>;

private:
	StateMap m_States;
	StateStack m_StateStack;
	StateCommandBuffer m_Commands;

public:
	inline StateSystem() noexcept { }

	StateSystem(const Type&) = delete;
	Type& operator= (const Type&) = delete;

public:
	template<class StateType, class... Args>
	StateType* CreateState(const StateName& name, Args&&... args) noexcept
	{
		if(name == InvalidStateName)
			return nullptr;

		auto pState = Epic::MakeImpl<Epic::State, StateType>(std::forward<Args>(args)...);
		auto pStatePtr = static_cast<StateType*>(pState.get());

		m_States[name] = std::move(pState);
		pStatePtr->m_pStateSystem = this;

		return pStatePtr;
	}

	State* GetState(const StateName& name) const noexcept
	{
		auto it = m_States.find(name);
		if (it == std::end(m_States))
			return nullptr;

		return (*it).second.get();
	}

	State* GetForeground() const noexcept
	{
		if (m_StateStack.empty())
			return nullptr;

		return m_StateStack.back();
	}

private:
	void _Push(const StateName& name)
	{
		auto pState = GetState(name);
		auto pForeground = GetForeground();
		m_StateStack.emplace_back(pState);

		if (pForeground)
			pForeground->LeaveForeground();

		pState->Enter();
	}

	void _Pop()
	{
		auto pState = GetForeground();
		if (!pState) return;

		pState->Leave();

		m_StateStack.pop_back();

		auto pForeground = GetForeground();
		if (pForeground)
			pForeground->EnterForeground();
	}

	void _Change(const StateName& name)
	{
		auto pState = GetState(name);

		if (m_StateStack.empty())
		{
			m_StateStack.emplace_back(pState);
			pState->Enter();
		}
		else
		{
			size_t stackCount = m_StateStack.size();

			// Stop all but the bottom state
			while (m_StateStack.size() > 1)
			{
				auto pForeground = m_StateStack.back();
				pForeground->Leave();
				m_StateStack.pop_back();
			}

			// Only pop the last state if it's not the target state
			if (pState != m_StateStack.back())
			{
				m_StateStack.back()->Leave();
				m_StateStack.clear();

				m_StateStack.emplace_back(pState);
				pState->Enter();
			}
			else if (stackCount > 1)
			{
				// The remaining state is the target state
				// and was previously in the background
				pState->EnterForeground();
			}
		}
	}

	void ProcessCommandQueue()
	{
		for (auto& cmd : m_Commands)
		{
			switch (cmd.CommandType)
			{
				case eStateSystemCommand::Change:
					_Change(cmd.Target);
					break;

				case eStateSystemCommand::Push:
					_Push(cmd.Target);
					break;

				case eStateSystemCommand::Pop:
					_Pop();
					break;

				default: break;
			}
		}
		
		m_Commands.clear();
	}

public:
	void Push(const StateName& name) noexcept
	{
		if (GetState(name) != nullptr)
			m_Commands.emplace_back(eStateSystemCommand::Push, name);
	}

	void Pop() noexcept
	{
		if (!m_Commands.empty() && 
			(m_Commands.back().CommandType == eStateSystemCommand::Push ||
			 m_Commands.back().CommandType == eStateSystemCommand::Change))
		{
			// This command would cancel out the previous command
			m_Commands.pop_back();
		}
		else
			m_Commands.emplace_back(eStateSystemCommand::Pop);
	}

	void ChangeTo(const StateName& name) noexcept
	{
		if (GetState(name) != nullptr)
		{
			// This command will cancel out previous commands
			m_Commands.clear();
			m_Commands.emplace_back(eStateSystemCommand::Change, name);
		}
	}

public:
	void Update()
	{
		ProcessCommandQueue();

		for (auto& pState : m_StateStack)
			pState->Update();
	}
};
