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

#include <Epic/detail/EntityManagerFwd.hpp>
#include <Epic/Entity.hpp>
#include <cassert>

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	class EntitySystem;
}

//////////////////////////////////////////////////////////////////////////////

// EntitySystem
class Epic::EntitySystem
{
public:
	using Type = Epic::EntitySystem;

private:
	friend class Epic::EntityManager;

private:
	Epic::EntityManager* m_pEntityManager;

public:
	EntitySystem(Epic::EntityManager* pEntityManager) noexcept 
		: m_pEntityManager{ pEntityManager }
	{
		assert(m_pEntityManager);
	}

	virtual ~EntitySystem() { }

public:
	Epic::EntityManager* GetEntityManager() noexcept
	{
		return m_pEntityManager;
	}

	const Epic::EntityManager* GetEntityManager() const noexcept
	{
		return m_pEntityManager;
	}

private:
	void OnEntityCreated(Epic::Entity* pEntity)
	{ 
		pEntity->ComponentAttached.Connect(this, &Type::EntityComponentAttached);
		pEntity->ComponentDetached.Connect(this, &Type::EntityComponentDetached);

		EntityCreated(pEntity);
	}

	void OnEntityDestroyed(Epic::Entity* pEntity)
	{ 
		pEntity->ComponentAttached.DisconnectAll(this);
		pEntity->ComponentDetached.DisconnectAll(this);

		EntityDestroyed(pEntity);
	}

public:
	virtual void Update() = 0;

protected:
	virtual void InitialUpdate() { };

	virtual void EntityCreated(Epic::Entity*) { }
	virtual void EntityDestroyed(Epic::Entity*) { }
	virtual void EntityComponentAttached(Epic::Entity* pEntity, Epic::EntityComponentID id) { }
	virtual void EntityComponentDetached(Epic::Entity* pEntity, Epic::EntityComponentID id) { }
};
