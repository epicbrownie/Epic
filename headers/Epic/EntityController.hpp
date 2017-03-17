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
//
//    This simple ECS system was inspired by Sam Bloomberg's ECS system
//        available for download at: https://github.com/redxdev/ECS
//
//////////////////////////////////////////////////////////////////////////////

#pragma once

#include <Epic/EntityManager.hpp>
#include <Epic/Event.hpp>
#include <cassert>

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	class EntityController;
}

//////////////////////////////////////////////////////////////////////////////

// EntityController
class Epic::EntityController
{
public:
	using Type = Epic::EntityController;

private:
	Epic::EntityManager* m_pEntityManager;
	
public:
	explicit EntityController(Epic::EntityManager* pEntityManager) noexcept
		: m_pEntityManager{ pEntityManager }
	{
		assert(m_pEntityManager);

		m_pEntityManager->EntityCreated.Connect(this, &Type::OnEntityCreated);
		m_pEntityManager->EntityDestroyed.Connect(this, &Type::OnEntityDestroyed);
	}

	virtual ~EntityController()
	{
		m_pEntityManager->EntityCreated.DisconnectAll(this);
		m_pEntityManager->EntityDestroyed.DisconnectAll(this);
	}

public:
	constexpr Epic::EntityManager* GetEntityManager() const noexcept
	{
		return m_pEntityManager;
	}

public:
	virtual void Update() = 0;

protected:
	virtual void OnEntityCreated(Epic::Entity*) { }
	virtual void OnEntityDestroyed(Epic::Entity*) { }
};
