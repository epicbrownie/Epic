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

#include <Epic/EON/Attribute.hpp>
#include <Epic/EON/Selector.hpp>
#include <Epic/EON/detail/ParserFwd.hpp>
#include <Epic/EON/detail/Assign.hpp>
#include <Epic/EON/detail/Utility.hpp>
#include <Epic/EON/detail/Visitors.hpp>
#include <Epic/STL/SharedPtr.hpp>
#include <Epic/STL/Vector.hpp>
#include <Epic/TMP/TypeTraits.hpp>
#include <string_view>
#include <utility>

//////////////////////////////////////////////////////////////////////////////

// Parser
template<class T>
struct Epic::EON::Parser
{
	using Type = Epic::EON::Parser<T>;

	using AssignerPtr = Epic::SharedPtr<detail::Assigner<T>>;
	using Binding = std::pair<Selector, AssignerPtr>;

	Epic::STLVector<Binding> m_Bindings;

	Parser() noexcept = default;
	Parser(const Type&) = default;
	Parser(Type&&) = default;

	template<class U, class Converter = DefaultConverter>
	Parser(Selector selector, U T::* pMember, Converter fnConvert = Converter())
	{
		Bind(std::move(selector), pMember, std::move(fnConvert));
	}

	template<class U, class Converter = DefaultConverter>
	Parser(eAttribute attr, U T::* pMember, Converter fnConvert = Converter())
	{
		Bind(attr, pMember, std::move(fnConvert));
	}

	template<class E, class U, class Converter = DefaultConverter>
	Parser(Selector selector, U T::* pMember, Parser<E> parser, Converter fnConvert = Converter())
	{
		Bind(std::move(selector), pMember, std::move(parser), std::move(fnConvert));
	}

	template<class U, class Converter = DefaultConverter>
	Type& Bind(Selector selector, U T::* pMember, Converter fnConvert = Converter())
	{
		using Assigner = detail::MemberAssigner<T, U, Converter>;
		
		m_Bindings.emplace_back(
			std::move(selector), 
			Epic::MakeShared<Assigner>(pMember, std::move(fnConvert))
		);

		return *this;
	}

	template<class U, class Converter = DefaultConverter>
	Type& Bind(eAttribute attr, U T::* pMember, Converter fnConvert = Converter())
	{
		using Assigner = detail::MemberAttributeAssigner<T, U, Converter>;
		
		m_Bindings.emplace_back(
			Selector(), 
			Epic::MakeShared<Assigner>(attr, pMember, std::move(fnConvert))
		);

		return *this;
	}

	template<class E, class U, class Converter = DefaultConverter>
	Type& Bind(Selector selector, U T::* pMember, Parser<E> parser, Converter fnConvert = Converter())
	{
		using Assigner = detail::MemberObjectAssigner<T, U, E, Converter>;
		
		m_Bindings.emplace_back(
			std::move(selector),
			Epic::MakeShared<Assigner>(pMember, std::move(parser), std::move(fnConvert))
		);

		return *this;
	}

	template<class U, class Converter = DefaultConverter>
	Type& operator() (Selector selector, U T::* pMember, Converter fnConvert = Converter())
	{
		return Bind(std::move(selector), pMember, std::move(fnConvert));
	}

	template<class U, class Converter = DefaultConverter>
	Type& operator() (eAttribute attr, U T::* pMember, Converter fnConvert = Converter())
	{
		return Bind(attr, pMember, std::move(fnConvert));
	}

	template<class E, class U, class Converter = DefaultConverter>
	Type& operator() (Selector selector, U T::* pMember, Parser<E> parser, Converter fnConvert = Converter())
	{
		return Bind(std::move(selector), pMember, std::move(parser), std::move(fnConvert));
	}

	/////

	bool Assign(T& to, const EONVariant& scope, const EONObject& globalScope) const
	{
		static_assert(std::is_default_constructible_v<T>, "Extracted type must be default constructible.");

		for (auto& bnd : m_Bindings)
			if (!bnd.second->Assign(to, bnd.first, scope, globalScope))
				return false;

		return true;
	}
};

//////////////////////////////////////////////////////////////////////////////

// Bind
namespace Epic::EON
{
	namespace
	{
		template<class T, class U, class Converter = DefaultConverter>
		Parser<T> Bind(Selector selector, U T::* pMember, Converter fnConvert = Converter())
		{
			return Parser<T>(std::move(selector), pMember, std::move(fnConvert));
		}

		template<class T, class E, class U, class Converter = DefaultConverter>
		Parser<T> Bind(Selector selector, U T::* pMember, Parser<E> parser, Converter fnConvert = Converter())
		{
			return Parser<T>(std::move(selector), pMember, std::move(parser), std::move(fnConvert));
		}

		template<class T, class U, class Converter = DefaultConverter>
		Parser<T> Bind(eAttribute attr, U T::* pMember, Converter fnConvert = Converter())
		{
			return Parser<T>(attr, pMember, std::move(fnConvert));
		}
	}
}
