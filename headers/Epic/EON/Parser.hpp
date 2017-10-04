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

	Parser() = default;

	template<class Converter = DefaultConverter>
	explicit Parser(const Selector& selector, Converter fnConvert = Converter())
	{
		Bind(selector, fnConvert);
	}

	template<class E, class Converter = DefaultConverter>
	Parser(const Selector& selector, const Parser<E>& extractor, Converter fnConvert = Converter())
	{
		Bind(selector, extractor, fnConvert);
	}

	template<class U, class Converter = DefaultConverter>
	Parser(const Selector& selector, U T::* pMember, Converter fnConvert = Converter())
	{
		Bind(selector, pMember, fnConvert);
	}

	template<class E, class U, class Converter = DefaultConverter>
	Parser(const Selector& selector, U T::* pMember, const Parser<E>& extractor, Converter fnConvert = Converter())
	{
		Bind(selector, pMember, extractor, fnConvert);
	}

	template<class Converter = DefaultConverter>
	Type& Bind(const Selector& selector, Converter fnConvert = Converter())
	{
		using Assigner = detail::FreeAssigner<T, Converter>;
		m_Bindings.emplace_back(Binding{ selector, Epic::MakeShared<Assigner>(fnConvert) });

		return *this;
	}

	template<class E, class Converter = DefaultConverter>
	Type& Bind(const Selector& selector, const Parser<E>& extractor, Converter fnConvert = Converter())
	{
		using Assigner = detail::FreeObjectAssigner<T, E, Converter>;
		m_Bindings.emplace_back(Binding{ selector, Epic::MakeShared<Assigner>(extractor, fnConvert) });

		return *this;
	}

	template<class U, class Converter = DefaultConverter>
	Type& Bind(const Selector& selector, U T::* pMember, Converter fnConvert = Converter())
	{
		using Assigner = detail::MemberAssigner<T, U, Converter>;
		m_Bindings.emplace_back(Binding{ selector, Epic::MakeShared<Assigner>(pMember, fnConvert) });

		return *this;
	}

	template<class E, class U, class Converter = DefaultConverter>
	Type& Bind(const Selector& selector, U T::* pMember, const Parser<E>& extractor, Converter fnConvert = Converter())
	{
		using Assigner = detail::MemberObjectAssigner<T, U, E, Converter>;
		m_Bindings.emplace_back(Binding{ selector, Epic::MakeShared<Assigner>(pMember, extractor, fnConvert) });

		return *this;
	}

	template<class Converter = DefaultConverter>
	Type& operator() (const Selector& selector, Converter fnConvert = Converter())
	{
		return Bind(selector, fnConvert);
	}

	template<class E, class Converter = DefaultConverter>
	Type& operator() (const Selector& selector, const Parser<E>& extractor, Converter fnConvert = Converter())
	{
		return Bind(selector, extractor, fnConvert);
	}

	template<class U, class Converter = DefaultConverter>
	Type& operator() (const Selector& selector, U T::* pMember, Converter fnConvert = Converter())
	{
		return Bind(selector, pMember, fnConvert);
	}

	template<class E, class U, class Converter = DefaultConverter>
	Type& operator() (const Selector& selector, U T::* pMember, const Parser<E>& extractor, Converter fnConvert = Converter())
	{
		return Bind(selector, pMember, extractor, fnConvert);
	}

	/////

	bool Assign(T& to, const EONVariant& scope) const
	{
		static_assert(std::is_default_constructible_v<T>, "Extracted type must be default constructible.");

		T extracted;
		for (auto& bnd : m_Bindings)
			if (!bnd.second->Assign(extracted, bnd.first, scope))
				return false;

		to = std::move(extracted);

		return true;
	}
};

//////////////////////////////////////////////////////////////////////////////

// Bind
namespace Epic::EON
{
	namespace
	{
		template<class T, class Converter = DefaultConverter>
		Parser<T> Bind(const Selector& selector, Converter fnConvert = Converter())
		{
			return Parser<T>(selector, fnConvert);
		}

		template<class T, class E, class Converter = DefaultConverter>
		Parser<T> Bind(const Selector& selector, const Parser<E>& extractor, Converter fnConvert = Converter())
		{
			return Parser<T>(selector, extractor, fnConvert);
		}

		template<class T, class U, class Converter = DefaultConverter>
		Parser<T> Bind(const Selector& selector, U T::* pMember, Converter fnConvert = Converter())
		{
			return Parser<T>(selector, pMember, fnConvert);
		}

		template<class T, class E, class U, class Converter = DefaultConverter>
		Parser<T> Bind(const Selector& selector, U T::* pMember, const Parser<E>& extractor, Converter fnConvert = Converter())
		{
			return Parser<T>(selector, pMember, extractor, fnConvert);
		}
	}
}
