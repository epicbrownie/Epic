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

#include <Epic/detail/EntityComponentIterator.hpp>
#include <Epic/detail/EntityComponentView.hpp>
#include <Epic/detail/EntityManagerFwd.hpp>
#include <Epic/Entity.hpp>
#include <Epic/Event.hpp>
#include <Epic/StringHash.hpp>
#include <Epic/STL/Vector.hpp>
#include <Epic/STL/Map.hpp>
#include <Epic/STL/UniquePtr.hpp>
#include <algorithm>
#include <functional>

//////////////////////////////////////////////////////////////////////////////

// EntityManager
class Epic::EntityManager
{
public:
	using Type = Epic::EntityManager;

public:
	constexpr static Epic::StringHash NoEntityName = Entity::NoEntityName;

private:
	using EntityPtr = Epic::UniquePtr<Entity>;
	using EntityList = Epic::STLVector<EntityPtr>;
	using EntityNameMap = Epic::STLUnorderedMap<Epic::StringHash, EntityPtr::pointer>;

private:
	EntityList m_Entities;
	EntityNameMap m_NameEntityMap;

public:
	EntityManager() noexcept { };

	~EntityManager() noexcept
	{
		Clear();
	}

private:
	inline void OnEntityCreated(EntityPtr::pointer pEntity) noexcept
	{
		this->EntityCreated(pEntity);
	}

	inline void OnEntityDestroyed(EntityPtr::pointer pEntity) noexcept
	{
		this->EntityDestroyed(pEntity);
	}

	void _DestroyEntity(EntityPtr::pointer pEntity) noexcept
	{
		m_NameEntityMap.erase(pEntity->GetName());

		auto it = std::find_if(
			std::begin(m_Entities),
			std::end(m_Entities),
			[&](const auto& p) { return p.get() == pEntity; });

		if (it != std::end(m_Entities))
			m_Entities.erase(it);
	}

public:
	inline size_t GetEntityCount() const noexcept
	{
		return m_Entities.size();
	}

	EntityPtr::pointer GetEntity(const size_t id) noexcept
	{
		auto it = std::find_if(std::begin(m_Entities), std::end(m_Entities),
			[&](const EntityPtr& pEntity) { return pEntity->GetID() == id; });

		return (it == std::end(m_Entities)) ? nullptr : (*it).get();
	}

	EntityPtr::pointer GetEntity(const Epic::StringHash& name) noexcept
	{
		auto it = m_NameEntityMap.find(name);
		if (it != std::end(m_NameEntityMap))
			return (*it).second;

		return nullptr;
	}

	inline EntityPtr::pointer GetEntityByIndex(const size_t index) noexcept
	{
		if (index >= 0 && index < m_Entities.size())
			return m_Entities[index].get();

		return nullptr;
	}

	EntityPtr::pointer CreateEntity(const Epic::StringHash& name = NoEntityName) noexcept
	{
		m_Entities.emplace_back(Epic::MakeUnique<Entity>(this, name, m_Entities.size()));
		EntityPtr::pointer pEntity = m_Entities.back().get();

		if (name != NoEntityName)
			m_NameEntityMap[name] = pEntity;

		OnEntityCreated(pEntity);

		return pEntity;
	}

	void DestroyEntity(EntityPtr::pointer pEntity, bool destroyNow = false) noexcept
	{
		if (!pEntity)
			return;

		if (pEntity->IsDestroyPending())
		{
			if (destroyNow)
				_DestroyEntity(pEntity);
		}
		else
		{
			pEntity->Destroy();
			OnEntityDestroyed(pEntity);

			if (destroyNow)
				_DestroyEntity(pEntity);
		}
	}

	inline void DestroyEntity(const Epic::StringHash& name, bool destroyNow = false) noexcept
	{
		DestroyEntity(GetEntity(name), destroyNow);
	}

	void Clear() noexcept
	{
		m_NameEntityMap.clear();

		for (auto& pEntity : m_Entities)
		{
			pEntity->Destroy();
			OnEntityDestroyed(pEntity.get());
		}

		m_Entities.clear();
	}

public:
	template<class... Components>
	Epic::detail::EntityComponentView<Components...> Each(bool includeDestroyed = false) noexcept
	{
		return Epic::detail::EntityComponentView<Components...>
		{
			{ this, 0, includeDestroyed },
			{ this, GetEntityCount(), includeDestroyed }
		};
	}

	template<class... Components>
	void Each(std::function<void(EntityPtr::pointer, Components&...)> fn, bool includeDestroyed = false)
	{
		if (includeDestroyed)
		{
			for (auto& pEntity : m_Entities)
			{
				if (pEntity->Has<Components...>())
					fn(pEntity.get(), pEntity->Get<Components>()...);
			}
		}
		else
		{
			for (auto& pEntity : m_Entities)
			{
				if (pEntity->Has<Components...>() && !pEntity->IsDestroyPending())
					fn(pEntity.get(), pEntity->Get<Components>()...);
			}
		}
	}

	Epic::detail::EntityComponentView<void> All(bool includeDestroyed = false) noexcept
	{
		return Epic::detail::EntityComponentView<void>
		{
			{ this, 0, includeDestroyed },
			{ this, GetEntityCount(), includeDestroyed }
		};
	}

	void All(std::function<void(EntityPtr::pointer)> fn, bool includeDestroyed = false)
	{
		if (includeDestroyed)
		{
			for (auto& pEntity : m_Entities)
				fn(pEntity.get());
		}
		else
		{
			for (auto& pEntity : m_Entities)
			{
				if (!pEntity->IsDestroyPending())
					fn(pEntity.get());
			}
		}
	}

public:
	void Update() noexcept
	{
		m_Entities.erase
		(
			std::remove_if
			(
				std::begin(m_Entities),
				std::end(m_Entities),
				[&](auto& pEntity)
				{
					if (pEntity->IsDestroyPending())
					{
						m_NameEntityMap.erase(pEntity->GetName());
						return true;
					}
				}
			),
			std::end(m_Entities)
		);
	}

private:
	using EntityChangedDelegate = Epic::Event<void(EntityPtr::pointer)>;

public:
	EntityChangedDelegate EntityCreated;
	EntityChangedDelegate EntityDestroyed;
};

