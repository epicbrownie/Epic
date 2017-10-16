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

namespace Epic::EON
{
	template<class I, class T, class U, class IConverter = DefaultConverter>
	struct Adapter;
}

//////////////////////////////////////////////////////////////////////////////

// Adapter - TODO: Move to own file
template<class I, class T, class U, class IConverter>
struct Epic::EON::Adapter
{
	using Type = Epic::EON::Adapter<I, T, U, IConverter>;
	using IntermediateType = I;
	using ConverterType = IConverter;

	U T::* pDest;
	IConverter fnConvertI;

	explicit Adapter(U T::* dest, IConverter convert = IConverter())
		: pDest(dest), fnConvertI(convert)
	{ }
};

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

	template<class U, class Converter = DefaultConverter>
	Parser(const Selector& selector, U T::* pMember, Converter fnConvert = Converter())
	{
		Bind(selector, pMember, fnConvert);
	}

	template<class U, class Converter = DefaultConverter>
	Parser(eAttribute attr, U T::* pMember, Converter fnConvert = Converter())
	{
		Bind(attr, pMember, fnConvert);
	}

	template<class E, class U, class Converter = DefaultConverter>
	Parser(const Selector& selector, U T::* pMember, const Parser<E>& extractor, Converter fnConvert = Converter())
	{
		Bind(selector, pMember, extractor, fnConvert);
	}

	template<class U, class I, class IConverter, class UConverter = DefaultConverter>
	Parser(const Selector& selector, const Adapter<I, T, U, IConverter>& adapter, UConverter fnConvert = UConverter())
	{
		Bind(selector, adapter, fnConvert);
	}

	template<class E, class U, class I, class IConverter, class UConverter = DefaultConverter>
	Parser(const Selector& selector, const Adapter<I, T, U, IConverter>& adapter, const Parser<E>& extractor, UConverter fnConvert = UConverter())
	{
		Bind(selector, adapter, extractor, fnConvert);
	}

	template<class U, class Converter = DefaultConverter>
	Type& Bind(const Selector& selector, U T::* pMember, Converter fnConvert = Converter())
	{
		using Assigner = detail::MemberAssigner<T, U, Converter>;
		m_Bindings.emplace_back(Binding{ selector, Epic::MakeShared<Assigner>(pMember, fnConvert) });

		return *this;
	}

	template<class U, class Converter = DefaultConverter>
	Type& Bind(eAttribute attr, U T::* pMember, Converter fnConvert = Converter())
	{
		using Assigner = detail::MemberAttributeAssigner<T, U, Converter>;
		m_Bindings.emplace_back(Binding{ Selector(), Epic::MakeShared<Assigner>(attr, pMember, fnConvert) });

		return *this;
	}

	template<class E, class U, class Converter = DefaultConverter>
	Type& Bind(const Selector& selector, U T::* pMember, const Parser<E>& extractor, Converter fnConvert = Converter())
	{
		using Assigner = detail::MemberObjectAssigner<T, U, E, Converter>;
		m_Bindings.emplace_back(Binding{ selector, Epic::MakeShared<Assigner>(pMember, extractor, fnConvert) });

		return *this;
	}

	template<class U, class I, class IConverter = DefaultConverter, class UConverter = DefaultConverter>
	Type& Bind(const Selector& selector, const Adapter<I, T, U, IConverter>& adapter, UConverter fnConvertU = UConverter())
	{
		using Assigner = detail::MemberAdapter<I, T, U, IConverter, UConverter>;
		m_Bindings.emplace_back(Binding{ selector, Epic::MakeShared<Assigner>
		(
			adapter.pDest, 
			adapter.fnConvertI, 
			fnConvertU
		) });

		return *this;
	}

	template<class E, class U, class I, class IConverter = DefaultConverter, class UConverter = DefaultConverter>
	Type& Bind(const Selector& selector, const Adapter<I, T, U, IConverter>& adapter, const Parser<E>& extractor, UConverter fnConvertU = UConverter())
	{
		using Assigner = detail::MemberObjectAdapter<I, T, U, E, IConverter, UConverter>;
		m_Bindings.emplace_back(Binding{ selector, Epic::MakeShared<Assigner>
		(
			adapter.pDest, 
			extractor,
			adapter.fnConvertI, 
			fnConvertU
		) });

		return *this;
	}

	template<class U, class Converter = DefaultConverter>
	Type& operator() (const Selector& selector, U T::* pMember, Converter fnConvert = Converter())
	{
		return Bind(selector, pMember, fnConvert);
	}

	template<class U, class Converter = DefaultConverter>
	Type& operator() (eAttribute attr, U T::* pMember, Converter fnConvert = Converter())
	{
		return Bind(attr, pMember, fnConvert);
	}

	template<class E, class U, class Converter = DefaultConverter>
	Type& operator() (const Selector& selector, U T::* pMember, const Parser<E>& extractor, Converter fnConvert = Converter())
	{
		return Bind(selector, pMember, extractor, fnConvert);
	}

	template<class U, class I, class IConverter, class UConverter = DefaultConverter>
	Type& operator() (const Selector& selector, const Adapter<I, T, U, IConverter>& adapter, UConverter fnConvert = UConverter())
	{
		return Bind(selector, adapter, fnConvert);
	}

	template<class E, class U, class I, class IConverter, class UConverter = DefaultConverter>
	Type& operator() (const Selector& selector, const Adapter<I, T, U, IConverter>& adapter, const Parser<E>& extractor, UConverter fnConvert = UConverter())
	{
		return Bind(selector, adapter, extractor, fnConvert);
	}

	/////

	bool Assign(T& to, const EONVariant& scope, const EONObject& globalScope) const
	{
		static_assert(std::is_default_constructible_v<T>, "Extracted type must be default constructible.");

		T extracted;
		for (auto& bnd : m_Bindings)
			if (!bnd.second->Assign(extracted, bnd.first, scope, globalScope))
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

		template<class T, class U, class Converter = DefaultConverter>
		Parser<T> Bind(eAttribute attr, U T::* pMember, Converter fnConvert = Converter())
		{
			return Parser<T>(attr, pMember, fnConvert);
		}

		template<class T, class U, class I, class IConverter = DefaultConverter, class UConverter = DefaultConverter>
		Parser<T> Bind(const Selector& selector, const Adapter<I, T, U, IConverter>& adapter, UConverter fnConvert = UConverter())
		{
			return Parser<T>(selector, adapter, fnConvert);
		}

		template<class T, class E, class U, class I, class IConverter = DefaultConverter, class UConverter = DefaultConverter>
		Parser<T> Bind(const Selector& selector, const Adapter<I, T, U, IConverter>& adapter, const Parser<E>& extractor, UConverter fnConvert = UConverter())
		{
			return Parser<T>(selector, adapter, extractor, fnConvert);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////

// Adapt
namespace Epic::EON
{
	namespace
	{
		template<class I, class T, class U, class Converter = DefaultConverter>
		auto Adapt(U T::* pMember, Converter fnConvert = Converter()) noexcept
		{
			return Adapter<I, T, U, Converter>(pMember, fnConvert);
		}
	}
}
