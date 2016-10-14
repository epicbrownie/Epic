//////////////////////////////////////////////////////////////////////////////
//
//            Copyright (c) 2016 Ronnie Brohn (EpicBrownie)      
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

#include <Epic/Memory/detail/AllocatorTraits.hpp>
#include <Epic/Memory/detail/AllocatorHelpers.hpp>
#include <Epic/Memory/MemoryBlock.hpp>
#include <Epic/NullAtomic.hpp>
#include <algorithm>
#include <atomic>
#include <cassert>
#include <cstdint>
#include <type_traits>

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	namespace detail
	{
		template<class AllocatorTemplate, bool IsShared, class NodeAllocator>
		class CascadingAllocatorBase;

		template<class AllocatorTemplate, bool IsShared, class NodeAllocator>
		class CascadingAllocatorImpl;

		template<class Allocator>
		struct CascadingAllocatorNode;

		template<class Allocator>
		struct CalcCascadingAllocatorNodeSize;
	}
}

//////////////////////////////////////////////////////////////////////////////

/// CascadingAllocatorNode<Allocator>
template<class Allocator>
struct Epic::detail::CascadingAllocatorNode 
{
	using Type = Epic::detail::CascadingAllocatorNode<Allocator>;
	
	CascadingAllocatorNode() 
		noexcept(std::is_nothrow_default_constructible<Allocator>::value)
		: m_Allocator{ }, m_pNext{ nullptr }, m_AllocatedSize{ 0 } 
	{ }

	explicit CascadingAllocatorNode(size_t sz)
		noexcept(std::is_nothrow_default_constructible<Allocator>::value)
		: m_Allocator{ }, m_pNext{ nullptr }, m_AllocatedSize{ sz }
	{ }

	CascadingAllocatorNode(const CascadingAllocatorNode<Allocator>&) = delete;

	template<typename = std::enable_if_t<std::is_move_constructible<Allocator>::value>>
	CascadingAllocatorNode(Type&& obj)
		noexcept(std::is_nothrow_move_constructible<Allocator>::value)
		: m_Allocator{ std::move(obj.m_Allocator) }, m_pNext{ nullptr }, m_AllocatedSize{ 0 }
	{
		std::swap(m_pNext, obj.m_pNext);
		std::swap(m_AllocatedSize, obj.m_AllocatedSize);
	}

	CascadingAllocatorNode& operator = (const CascadingAllocatorNode<Allocator>&) = delete;
	CascadingAllocatorNode& operator = (CascadingAllocatorNode<Allocator>&&) = delete;

	Type* m_pNext;
	size_t m_AllocatedSize;
	Allocator m_Allocator;
};

//////////////////////////////////////////////////////////////////////////////

/// CalcCascadingAllocatorNodeSize<Allocator>
template<class Allocator>
struct Epic::detail::CalcCascadingAllocatorNodeSize
{ 
	static constexpr size_t value = sizeof(CascadingAllocatorNode<Allocator>);
};

//////////////////////////////////////////////////////////////////////////////

/// CascadingAllocatorBase<A, IsShared, NodeA>
template<class A, bool IsShared, class NodeA>
class Epic::detail::CascadingAllocatorBase
{
	static_assert(std::is_default_constructible<A>::value, "The template allocator must be default-constructible.");
	static_assert(std::is_default_constructible<NodeA>::value, "The node allocator must be default-constructible.");
	static_assert(!IsShared || (IsShared && NodeA::IsShareable), "The node allocator must be shareable.");

public:
	using Type = Epic::detail::CascadingAllocatorBase<A, IsShared, NodeA>;

protected:
	using NodeType = CascadingAllocatorNode<A>;
	using NodeAllocatorType = NodeA;
	using NodePtr = std::conditional_t<IsShared, std::atomic<NodeType*>, Epic::NullAtomic<NodeType*>>;

private:
	static constexpr bool UseAllocate = CanAllocate<NodeA>::value;
	static constexpr size_t NodeSize = CalcCascadingAllocatorNodeSize<A>::value;
	
	static_assert(NodeSize <= NodeA::MaxAllocSize, "This node allocator's maximum allocation size is too low to hold the allocator nodes.");
	static_assert(NodeSize >= NodeA::MinAllocSize, "This node allocator's minimum allocation size is too high to hold the allocator nodes.");

private:
	NodePtr m_pAllocNodes;
	NodeAllocatorType m_NodeAllocator;
	
public:
	constexpr CascadingAllocatorBase()
		noexcept(std::is_nothrow_default_constructible<NodeA>::value)
		: m_NodeAllocator{ }, m_pAllocNodes{ nullptr }
	{ }

	constexpr CascadingAllocatorBase(const Type& obj) = delete;

	template<typename = std::enable_if_t<std::is_move_constructible<NodeA>::value>>
	constexpr CascadingAllocatorBase(Type&& obj)
		noexcept(std::is_nothrow_move_constructible<NodeA>::value)
		: m_NodeAllocator{ std::move(obj.m_NodeAllocator) }, 
		  m_pAllocNodes{ obj.m_pAllocNodes }
	{
		obj.m_pAllocNodes = nullptr;
	}

	CascadingAllocatorBase& operator = (const Type& obj) = delete;
	CascadingAllocatorBase& operator = (Type&& obj) = delete;

	~CascadingAllocatorBase()
	{
		DestroyNodes();
	}

protected:
	NodeType* FindOwner(const Blk& blk) const noexcept
	{
		auto pNode = GetNodeList();

		while (pNode)
		{
			if (pNode->m_Allocator.Owns(blk))
				return pNode;

			pNode = pNode->m_pNext;
		}

		return nullptr;
	}

	NodeType* GetNodeList() const noexcept
	{
		return m_pAllocNodes.load();
	}

	NodeType* CreateNode() noexcept
	{
		Blk blk;

		// Allocate a block to place the new node		
		if (UseAllocate)
			blk = AllocateIf<NodeA>::apply(m_NodeAllocator, NodeSize);
		else
			blk = AllocateAlignedIf<NodeA>::apply(m_NodeAllocator, NodeSize, NodeA::Alignment);

		// Verify the block
		if (!blk) 
			return nullptr;

		// Construct a new node into the allocated block
		auto pNode = ::new (blk.Ptr) NodeType{ blk.Size };

		// Atomically add the new node to the allocator node list
		// Critical Section
		//   Unless a profiler reveals a notable performance bottleneck here, the default memory 
		//   orderings will be used over release/relaxed for simplicity. 
		pNode->m_pNext = m_pAllocNodes.load();
		while (!m_pAllocNodes.compare_exchange_weak(pNode->m_pNext, pNode));

		return pNode;
	}

	void DestroyNodes()
	{
		/* Must not be called while thread safety is still required. */
		
		NodeType* pNode;

		while ((pNode = m_pAllocNodes.load(std::memory_order_acquire)) != nullptr)
		{
			auto pNextNode = pNode->m_pNext;
			auto nodesz = pNode->m_AllocatedSize;

			pNode->~CascadingAllocatorNode();

			if (!CanDeallocateAll<NodeA>::value)
			{
				Blk blk{ pNode, nodesz };

				if (UseAllocate)
					DeallocateIf<NodeA>::apply(m_NodeAllocator, blk);
				else
					DeallocateAlignedIf<NodeA>::apply(m_NodeAllocator, blk);
			}

			m_pAllocNodes.store(pNextNode, std::memory_order_release);
		}

		DeallocateAllIf<NodeA>::apply(m_NodeAllocator);
	}

	void DeallocateAllInNodes() noexcept
	{
		auto pNode = GetNodeList();

		while (pNode)
		{
			DeallocateAllIf<A>::apply(pNode->m_Allocator);
			pNode = pNode->m_pNext;
		}
	}
};

/// CascadingAllocatorBase<A, IsShared, void>
template<class A, bool IsShared>
class Epic::detail::CascadingAllocatorBase<A, IsShared, void>
{
	static_assert(std::is_default_constructible<A>::value, "The template allocator must be default-constructible.");
	static_assert(std::is_move_constructible<A>::value, "The template allocator must be move-constructible when not using a node allocator.");

public:
	using Type = Epic::detail::CascadingAllocatorBase<A, IsShared, void>;

protected:
	using NodeType = CascadingAllocatorNode<A>;
	using NodeAllocatorType = void;
	using NodePtr = std::conditional_t<IsShared, std::atomic<NodeType*>, Epic::NullAtomic<NodeType*>>;

private:
	static constexpr bool UseAllocate = CanAllocate<A>::value;
	static constexpr size_t NodeSize = CalcCascadingAllocatorNodeSize<A>::value;
	
	static_assert(NodeSize <= A::MaxAllocSize, "This allocator's maximum allocation size is too low to hold the allocator nodes.");
	static_assert(NodeSize >= A::MinAllocSize, "This allocator's minimum allocation size is too high to hold the allocator nodes.");

private:
	NodePtr m_pAllocNodes;
	
public:
	constexpr CascadingAllocatorBase() noexcept
		: m_pAllocNodes{ nullptr } { }

	constexpr CascadingAllocatorBase(const Type& obj) = delete;

	constexpr CascadingAllocatorBase(Type&& obj) noexcept
		: m_pAllocNodes{ nullptr }
	{
		auto pAllocNodes = obj.m_pAllocNodes.load(std::memory_order_relaxed);
		while (!obj.m_pAllocNodes.compare_exchange_weak(pAllocNodes, nullptr));

		m_pAllocNodes.store(pAllocNodes);
	}

	CascadingAllocatorBase& operator = (const Type& obj) = delete;
	CascadingAllocatorBase& operator = (Type&& obj) noexcept = delete;

	~CascadingAllocatorBase()
	{
		DestroyNodes();
	}

protected:
	NodeType* FindOwner(const Blk& blk) const noexcept
	{
		auto pNode = GetNodeList();

		while (pNode)
		{
			if (pNode->m_Allocator.Owns(blk))
				return pNode;

			pNode = pNode->m_pNext;
		}

		return nullptr;
	}

	NodeType* GetNodeList() const noexcept
	{
		return m_pAllocNodes.load();
	}

	NodeType* CreateNode() noexcept
	{
		// Create a node on the stack and use it to allocate 
		// a storage block for the node.
		NodeType node;
		Blk blk;

		if (UseAllocate)
			blk = AllocateIf<A>::apply(node.m_Allocator, NodeSize);
		else
			blk = AllocateAlignedIf<A>::apply(node.m_Allocator, NodeSize, A::Alignment);

		// Verify the block
		if (!blk) return nullptr;

		// Prepare the node for transfer
		node.m_AllocatedSize = blk.Size;

		// Construct a new node into the allocated block
		auto pNode = ::new (blk.Ptr) NodeType{ std::move(node) };

		// Atomically add the new node to the allocator node list
		pNode->m_pNext = m_pAllocNodes.load();
		while (!m_pAllocNodes.compare_exchange_weak(pNode->m_pNext, pNode));

		return pNode;
	}

	void DestroyNodes()
	{
		/* Must not be called while thread safety is still required. */

		NodeType* pNode;

		while ((pNode = m_pAllocNodes.load(std::memory_order_acquire)) != nullptr)
		{
			auto pNextNode = pNode->m_pNext;

			// Move the node to the stack
			NodeType node{ std::move(*pNode) };

			// Free the node block
			Blk blk{ pNode, node.m_AllocatedSize };
			pNode->~CascadingAllocatorNode();

			if (UseAllocate)
				DeallocateIf<A>::apply(node.m_Allocator, blk);
			else
				DeallocateAlignedIf<A>::apply(node.m_Allocator, blk);

			m_pAllocNodes.store(pNextNode, std::memory_order_release);
		}
	}

	void DeallocateAllInNodes() noexcept
	{
		auto pNode = GetNodeList();

		while (pNode)
		{
			DeallocateAllIf<A>::apply(pNode->m_Allocator);
			pNode = pNode->m_pNext;
		}
	}
};

//////////////////////////////////////////////////////////////////////////////

/// CascadingAllocatorImpl<A, IsShared, NodeA>
template<class A, bool IsShared, class NodeA>
class Epic::detail::CascadingAllocatorImpl : public Epic::detail::CascadingAllocatorBase<A, IsShared, NodeA>
{
public:
	static_assert(!IsShared || (IsShared && A::IsShareable), "The template allocator must be shareable.");

public:
	using Type = Epic::detail::CascadingAllocatorImpl<A, IsShared, NodeA>;
	using Base = Epic::detail::CascadingAllocatorBase<A, IsShared, NodeA>;
	using AllocatorType = A;
	using NodeAllocatorType = NodeA;
	
public:
	static constexpr size_t Alignment = A::Alignment;
	static constexpr size_t MinAllocSize = A::MinAllocSize;
	static constexpr size_t MaxAllocSize = A::MaxAllocSize;
	static constexpr bool IsShareable = IsShared;

public:
	constexpr CascadingAllocatorImpl()
		noexcept(std::is_nothrow_default_constructible<Base>::value) = default;

	constexpr CascadingAllocatorImpl(const Type& obj) = delete;

	template<typename = std::enable_if_t<std::is_move_constructible<Base>::value>>
	constexpr CascadingAllocatorImpl(Type&& obj)
		noexcept(std::is_nothrow_move_constructible<Base>::value)
		: Base(std::move(obj))
	{ }

	CascadingAllocatorImpl& operator = (const Type& obj) = delete;
	CascadingAllocatorImpl& operator = (Type&& obj) = delete;
	
private:
	Blk TryAllocate(size_t sz) noexcept
	{
		if (!detail::CanAllocate<A>::value)
			return{ nullptr, 0 };

		Blk result;
		auto pNode = GetNodeList();

		while (pNode)
		{
			if (result = detail::AllocateIf<A>::apply(pNode->m_Allocator, sz))
				return result;

			pNode = pNode->m_pNext;
		}

		return{ nullptr, 0 };
	}

	Blk TryAllocateAligned(size_t sz, size_t alignment) noexcept
	{
		if (!detail::CanAllocateAligned<A>::value)
			return{ nullptr, 0 };

		Blk result;
		auto pNode = GetNodeList();

		while (pNode)
		{
			if (result = detail::AllocateAlignedIf<A>::apply(pNode->m_Allocator, sz, alignment))
				return result;

			pNode = pNode->m_pNext;
		}

		return{ nullptr, 0 };
	}

public:
	/* Returns whether or not this allocator is responsible for the block Blk. */
	inline bool Owns(const Blk& blk) const noexcept
	{
		return blk && (FindOwner(blk) != nullptr);
	}

public:
	/* Returns a block of uninitialized memory. */
	template<typename = std::enable_if_t<detail::CanAllocate<A>::value>>
	Blk Allocate(size_t sz) noexcept
	{
		// Verify that the requested size is within our allowed bounds
		if (sz == 0 || sz < MinAllocSize || sz > MaxAllocSize)
			return{ nullptr, 0 };

		// Allocate the block
		Blk result = TryAllocate(sz);

		if (!result)
		{
			CreateNode();
			result = TryAllocate(sz);
		}

		return result;
	}

	/* Returns a block of uninitialized memory (aligned to alignment). */
	template<typename = std::enable_if_t<detail::CanAllocateAligned<A>::value>>
	Blk AllocateAligned(size_t sz, size_t alignment = Alignment) noexcept
	{
		// Verify that the alignment is acceptable
		if (!Epic::detail::IsGoodAlignment(alignment))
			return{ nullptr, 0 };

		// Verify that the requested size is within our allowed bounds
		if (sz == 0 || sz < MinAllocSize || sz > MaxAllocSize)
			return{ nullptr, 0 };

		// Allocate the block
		Blk result = TryAllocateAligned(sz, alignment);

		if (!result)
		{
			CreateNode();
			result = TryAllocateAligned(sz, alignment);
		}

		return result;
	}

	/* Attempts to reallocate the memory of blk to the new size sz. */
	template<typename = std::enable_if_t<detail::CanAllocate<A>::value>>
	bool Reallocate(Blk& blk, size_t sz)
	{
		// If the block isn't valid, delegate to Allocate
		if (!blk)
		{
			blk = detail::AllocateIf<Type>::apply(*this, sz);
			return (bool)blk;
		}

		// If the requested size is zero, delegate to Deallocate
		if (sz == 0)
		{
			if (detail::CanDeallocate<Type>::value)
			{
				detail::DeallocateIf<Type>::apply(*this, blk);
				blk = { nullptr, 0 };
			}

			return detail::CanDeallocate<Type>::value;
		}

		// Verify that the requested size is within our allowed bounds
		if (sz < MinAllocSize || sz > MaxAllocSize)
			return false;

		// First, attempt to reallocate it via the owning allocator
		if (detail::CanReallocate<A>::value)
		{
			auto pNode = FindOwner(blk);
			assert(pNode && "CascadingAllocator::Reallocate - Attempted to reallocate a block that was not allocated through this allocator");

			if (detail::ReallocateIf<A>::apply(pNode->m_Allocator, blk, sz))
				return true;
		}

		// Now attempt to reallocate using a helper.
		// This could result in the allocation being moved to another node.
		return detail::Reallocator<Type>::ReallocateViaCopy(*this, blk, sz);
	}

	/* Attempts to reallocate the memory of blk (aligned to alignment) to the new size sz. */
	template<typename = std::enable_if_t<detail::CanAllocateAligned<A>::value>>
	bool ReallocateAligned(Blk& blk, size_t sz, size_t alignment = A::Alignment)
	{
		// Verify that the alignment is acceptable
		if (!Epic::detail::IsGoodAlignment(alignment))
			return false;

		// If the block isn't valid, delegate to AllocateAligned
		if (!blk)
		{
			blk = detail::AllocateAlignedIf<Type>::apply(*this, sz, alignment);
			return (bool)blk;
		}

		// If the requested size is zero, delegate to DeallocateAligned
		if (sz == 0)
		{
			if (detail::CanDeallocateAligned<Type>::value)
			{
				detail::DeallocateAlignedIf<Type>::apply(*this, blk);
				blk = { nullptr, 0 };
			}

			return detail::CanDeallocateAligned<Type>::value;
		}

		// Verify that the requested size is within our allowed bounds
		if (sz < MinAllocSize || sz > MaxAllocSize)
			return false;

		// First, attempt to reallocate it via the owning allocator
		if (detail::CanReallocateAligned<A>::value)
		{
			auto pNode = FindOwner(blk);
			assert(pNode && "CascadingAllocator::ReallocateAligned - Attempted to reallocate a block that was not allocated through this allocator");

			if (detail::ReallocateAlignedIf<A>::apply(pNode->m_Allocator, blk, sz, alignment))
				return true;
		}

		// Now attempt to reallocate using a helper.
		// This could result in the allocation being moved to another node.
		return detail::AlignedReallocator<Type>::ReallocateViaCopy(*this, blk, sz, alignment);
	}

public:
	/* Frees the memory for blk. */
	template<typename = std::enable_if_t<detail::CanDeallocate<A>::value>>
	void Deallocate(const Blk& blk)
	{
		if (!blk) return;

		auto pNode = FindOwner(blk);
		assert(pNode && "CascadingAllocator::Deallocate - Attempted to deallocate a block that was not allocated by this allocator");

		pNode->m_Allocator.Deallocate(blk);
	}

	/* Frees the memory for blk. */
	template<typename = std::enable_if_t<detail::CanDeallocateAligned<A>::value>>
	void DeallocateAligned(const Blk& blk)
	{
		if (!blk) return;

		auto pNode = FindOwner(blk);
		assert(pNode && "CascadingAllocator::DeallocateAligned - Attempted to deallocate a block that was not allocated by this allocator");

		pNode->m_Allocator.DeallocateAligned(blk);
	}

	/* Frees all of the allocated memory in all allocator nodes.
	   If this allocator is not shared, the allocator chain will also be destroyed. */
	template<typename = std::enable_if_t<detail::CanDeallocateAll<A>::value>>
	void DeallocateAll() noexcept
	{
		DeallocateAllInNodes();
		
		if(!IsShared)
			DestroyNodes();
	}
};

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	template<class AllocatorTemplate>
	using CascadingAllocatorNodeSize = detail::CalcCascadingAllocatorNodeSize<AllocatorTemplate>;
}

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	template<class AllocatorTemplate, class NodeAllocator = void>
	using CascadingAllocator = Epic::detail::CascadingAllocatorImpl<AllocatorTemplate, false, NodeAllocator>;

	template<class AllocatorTemplate, class NodeAllocator = void>
	using SharedCascadingAllocator = Epic::detail::CascadingAllocatorImpl<AllocatorTemplate, true, NodeAllocator>;
}
