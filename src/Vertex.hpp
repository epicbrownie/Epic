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

#include <Epic/VertexComponent.hpp>
#include <Epic/detail/VertexPosition.hpp>
#include <Epic/detail/VertexNormal.hpp>
#include <Epic/detail/VertexTexture.hpp>
#include <Epic/detail/VertexColor.hpp>
#include <Epic/TMP/VariadicContains.hpp>
#include <Epic/STL/Vector.hpp>
#include <cstdint>
#include <type_traits>

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	struct VertexFormat;

	namespace detail
	{
		struct VertexFormatNode;
	}

	template<class... ComponentTags>
	class Vertex;
}

//////////////////////////////////////////////////////////////////////////////

// VertexFormat
struct Epic::VertexFormat
{
	size_t Stride;
	Epic::STLVector<Epic::detail::VertexFormatNode> Components;
};

//////////////////////////////////////////////////////////////////////////////

// VertexFormatNode
struct Epic::detail::VertexFormatNode
{
	const char* Name;				// The name of the component
	const std::uintptr_t Offset;	// The byte-offset of the component
	const size_t Components;		// The number of data components 
									//  (eg: 3 for an xyz position component)
	const size_t Size;				// The size (in bytes) of the component
};

//////////////////////////////////////////////////////////////////////////////

// Vertex
template<class... ComponentTags>
class Epic::Vertex : public Epic::VertexComponent<ComponentTags>...
{
public:
	using Type = Epic::Vertex<ComponentTags...>;

	template<class SearchTag>
	using HasComponent = Epic::TMP::VariadicContains<SearchTag, ComponentTags...>;

public:
	constexpr Vertex() noexcept = default;
	Vertex(const Type&) noexcept = default;

public:
	static Epic::VertexFormat GetFormat() noexcept
	{
		Epic::VertexFormat fmt;
		fmt.Stride = sizeof(Type);

		IterateComponents(
			[&](const char* name, std::uintptr_t offset, size_t dataSize, size_t components)
			{
				fmt.Components.emplace_back(Epic::detail::VertexFormatNode{ name, offset, components, dataSize });
			});

		return fmt;
	}

private: 
	// Relies on non-standard behavior
	template<typename Component>
	static constexpr std::uintptr_t OffsetOf() noexcept
	{
		return reinterpret_cast<std::uintptr_t>(&static_cast<Type*>(nullptr)->Component::Value());
	}

	template<typename Function>
	static void IterateComponents(Function&& fn) noexcept
	{
		// Unpack expressions are only allowed in argument lists and initialization lists,
		// so this expression unpacks the function call expression into the initializer list
		// for an unused array (which the optimizer is nice enough to discard)
		char pass[] =
		{
			(fn(Epic::VertexComponent<ComponentTags>::GetName(),
				Type::OffsetOf<Epic::VertexComponent<ComponentTags>>(),
				sizeof(typename Epic::VertexComponent<ComponentTags>::ValueType),
				Epic::VertexComponent<ComponentTags>::Components
				), '\0'
			)...
		};

		(void)pass; // Suppress unused variable warnings
	}
};
