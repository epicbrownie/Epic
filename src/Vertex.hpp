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

#include <Epic/VertexAttribute.hpp>
#include <Epic/detail/VertexPosition.hpp>
#include <Epic/detail/VertexNormal.hpp>
#include <Epic/detail/VertexTexture.hpp>
#include <Epic/detail/VertexColor.hpp>
#include <Epic/detail/VertexSkin.hpp>
#include <Epic/TMP/VariadicContains.hpp>
#include <Epic/STL/Vector.hpp>
#include <cstdint>
#include <cstring>
#include <type_traits>

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	struct VertexFormat;

	namespace detail
	{
		struct VertexFormatNode;
	}

	template<class... AttributeTags>
	class Vertex;
}

//////////////////////////////////////////////////////////////////////////////


// VertexFormatNode
struct Epic::detail::VertexFormatNode
{
	const char* Semantic;					// The semantic name of the attribute
	const std::uintptr_t Offset;			// The byte-offset of the attribute data
	const size_t Components;				// The number of data components
											//   (eg: 3 for an xyz position attribute)
	const size_t Size;						// The size (in bytes) of the attribute data
	const Epic::eComponentType DataType;	// The data type for each data component
	const bool Normalize;					// Whether or not the attribute data should 
											//   be normalized automatically.
};

//////////////////////////////////////////////////////////////////////////////

// VertexFormat
struct Epic::VertexFormat
{
	size_t Stride;
	Epic::STLVector<Epic::detail::VertexFormatNode> Attributes;

	VertexFormat& operator= (const VertexFormat& other)
	{
		Stride = other.Stride;

		Attributes.clear();

		for (const auto& attr : other.Attributes)
			Attributes.emplace_back(detail::VertexFormatNode{ attr });

		return *this;
	}

	bool Contains(const Epic::detail::VertexFormatNode& AttrNode) const noexcept
	{
		for (auto& attr : Attributes)
		{
			if (std::strcmp(attr.Semantic, AttrNode.Semantic) == 0)
				return true;
		}

		return false;
	}
};

//////////////////////////////////////////////////////////////////////////////

// Vertex
template<class... AttributeTags>
class Epic::Vertex : public Epic::VertexAttribute<AttributeTags>...
{
public:
	using Type = Epic::Vertex<AttributeTags...>;

	template<class SearchTag>
	using HasAttribute = Epic::TMP::VariadicContains<SearchTag, AttributeTags...>;

public:
	constexpr Vertex() noexcept = default;
	Vertex(const Type&) noexcept = default;

public:
	constexpr Vertex(const Epic::VertexAttribute<AttributeTags>&... attributes)
		: Epic::VertexAttribute<AttributeTags>(attributes)... { }

	constexpr Vertex(Epic::VertexAttribute<AttributeTags>&&... attributes)
		: Epic::VertexAttribute<AttributeTags>(std::move(attributes))... { }

public:
	static Epic::VertexFormat GetFormat() noexcept
	{
		Epic::VertexFormat fmt;
		fmt.Stride = sizeof(Type);

		IterateAttributes(
			[&](const char* semantic, std::uintptr_t offset, size_t dataSize, size_t components, Epic::eComponentType cmpType, bool normalize)
			{
				fmt.Attributes.emplace_back(Epic::detail::VertexFormatNode
				{ 
					semantic, offset, components, dataSize, cmpType, normalize 
				});
			});

		return fmt;
	}

private: 
	// Relies on non-standard behavior
	template<typename Attribute>
	static constexpr std::uintptr_t OffsetOf() noexcept
	{
		return reinterpret_cast<std::uintptr_t>(&static_cast<Type*>(nullptr)->Attribute::Value());
	}

	template<typename Function>
	static void IterateAttributes(Function&& fn) noexcept
	{
		// Unpack expressions are only allowed in argument lists and initialization lists,
		// so this expression unpacks the function call expression into the initializer list
		// for an unused array (which the optimizer is nice enough to discard)
		char pass[] =
		{
			(fn(Epic::VertexAttribute<AttributeTags>::GetSemantic(),
				Type::OffsetOf<Epic::VertexAttribute<AttributeTags>>(),
				sizeof(typename Epic::VertexAttribute<AttributeTags>::ValueType),
				Epic::VertexAttribute<AttributeTags>::Components,
				Epic::VertexAttribute<AttributeTags>::DataType,
				Epic::VertexAttribute<AttributeTags>::Normalize
				), '\0'
			)...
		};

		(void)pass; // Suppress unused variable warnings
	}
};
