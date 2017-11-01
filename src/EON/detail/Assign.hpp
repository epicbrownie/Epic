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
#include <Epic/EON/Attribute.hpp>
#include <Epic/EON/detail/ParserFwd.hpp>
#include <Epic/EON/detail/Utility.hpp>
#include <Epic/EON/detail/Tags.hpp>
#include <Epic/EON/detail/Traits.hpp>
#include <Epic/EON/detail/Visitors.hpp>

//////////////////////////////////////////////////////////////////////////////

namespace Epic::EON::detail
{
	template<class T>
	struct Assigner;

	template<class T, class U, class Converter>
	struct MemberAssigner;

	template<class T, class U, class E, class Converter>
	struct MemberObjectAssigner;

	template<class T, class U, class Converter>
	struct MemberAttributeAssigner;
}

//////////////////////////////////////////////////////////////////////////////

// Helpers
namespace Epic::EON::detail
{
	namespace
	{
		template<class T, class Converter>
		bool DoConvertAssign(T& to, Selector selector, Converter fnConvert, const EONVariant& scope, const EONObject& globalScope)
		{
			auto vars = selector.Evaluate(&scope);
			if (std::empty(vars))
				return selector.IsOptional();

			for (const auto& var : vars)
				if (!std::visit(ConversionVisitor<T, Converter>(to, std::move(fnConvert), globalScope), var.second->Data))
					return false;

			return true;
		}

		template<class T, class E, class Converter>
		bool DoScalarAssign(T& to, Selector selector, Parser<E> parser, Converter fnConvert, 
							const EONVariant& scope, const EONObject& globalScope)
		{
			static_assert(std::is_default_constructible_v<E>, "Parsed type must be default constructible.");

			auto vars = selector.Evaluate(&scope);
			if (std::empty(vars))
				return selector.IsOptional();

			for (const auto& var : vars)
			{
				const auto pAsObject = std::get_if<EONObject>(&var.second->Data);
				if (!pAsObject)
					return false;

				E extracted;

				if (!parser.Assign(extracted, *var.second, globalScope))
					return false;

				if (!ConvertIf(std::move(fnConvert), to, std::move(extracted)))
					return false;
			}

			return true;
		}

		template<class T, class E, class Converter>
		bool DoArrayAssign(T& to, Selector selector, Parser<E> parser, Converter fnConvert, 
						   const EONVariant& scope, const EONObject& globalScope)
		{
			using Item = typename T::value_type;

			static_assert(std::is_default_constructible_v<E>, "Parsed type must be default constructible.");
			static_assert(std::is_default_constructible_v<Item>, "Array value type must be default constructible.");

			auto vars = selector.Evaluate(&scope);
			if (std::empty(vars))
				return selector.IsOptional();

			for (const auto& var : vars)
			{
				const auto pAsArray = std::get_if<EONArray>(&var.second->Data);
				if (!pAsArray)
					return false;

				for (const auto& vm : pAsArray->Members)
				{
					const auto pAsObject = std::get_if<EONObject>(&vm.Data);
					if (!pAsObject)
						return false;

					E extracted;
					if (!parser.Assign(extracted, vm, globalScope))
						return false;

					Item& item = to.emplace_back();

					if (!ConvertIf(std::move(fnConvert), item, std::move(extracted)))
						return false;
				}
			}

			return true;
		}

		template<class T, class E, class Converter>
		bool DoSetAssign(T& to, Selector selector, Parser<E> parser, Converter fnConvert, 
						 const EONVariant& scope, const EONObject& globalScope)
		{
			using Item = typename T::key_type;

			static_assert(std::is_default_constructible_v<E>, "Parsed type must be default constructible.");
			static_assert(std::is_default_constructible_v<Item>, "Set key type must be default constructible.");

			auto vars = selector.Evaluate(&scope);
			if (std::empty(vars))
				return selector.IsOptional();

			for (const auto& var : vars)
			{
				const auto pAsArray = std::get_if<EONArray>(&var.second->Data);
				if (!pAsArray)
					return false;

				for (const auto& vm : pAsArray->Members)
				{
					const auto pAsObject = std::get_if<EONObject>(&vm.Data);
					if (!pAsObject)
						return false;

					E extracted;
					if (!parser.Assign(extracted, vm, globalScope))
						return false;

					Item item;
					
					if (!ConvertIf(std::move(fnConvert), item, std::move(extracted)))
						return false;

					to.emplace(std::move(item));
				}
			}

			return true;
		}

		template<class T, class E, class Converter>
		bool DoMapAssign(T& to, Selector selector, Parser<E> parser, Converter fnConvert, 
						 const EONVariant& scope, const EONObject& globalScope)
		{
			using Key = typename T::key_type;
			using Item = typename T::mapped_type;

			static_assert(std::is_default_constructible_v<E>, "Parsed type must be default constructible.");
			static_assert(std::is_default_constructible_v<Key>, "Map key type must be default constructible.");
			static_assert(std::is_default_constructible_v<Item>, "Map mapped type must be default constructible.");

			auto vars = selector.Evaluate(&scope);
			if (std::empty(vars))
				return selector.IsOptional();

			for (const auto& var : vars)
			{
				const auto pAsObject = std::get_if<EONObject>(&var.second->Data);
				if (!pAsObject)
					return false;

				for (const auto& vm : pAsObject->Members)
				{
					const auto pItemAsObject = std::get_if<EONObject>(&vm.Value.Data);
					if (!pItemAsObject)
						return false;

					Key key;
					if (!ConvertIf(fnConvert, key, vm.Name))
						return false;

					E extracted;
					if (!parser.Assign(extracted, vm.Value, globalScope))
						return false;

					Item item;
					if (!ConvertIf(std::move(fnConvert), item, std::move(extracted)))
						return false;

					to.emplace(std::move(key), std::move(item));
				}
			}

			return true;
		}
	}
}

//////////////////////////////////////////////////////////////////////////////

// Assigner
template<class T>
struct Epic::EON::detail::Assigner
{
	virtual bool Assign(T& to, Selector selector, const EONVariant& scope, const EONObject& globalScope) const = 0;
};

// MemberAssigner
template<class T, class U, class Converter>
struct Epic::EON::detail::MemberAssigner : public Assigner<T>
{
	U T::* pDest;
	Converter fnConvert;

	MemberAssigner(U T::* dest, Converter convertFn)
		: pDest(dest), fnConvert(std::move(convertFn)) { }

	bool Assign(T& to, Selector selector, const EONVariant& scope, const EONObject& globalScope) const override
	{
		return DoConvertAssign(to.*pDest, std::move(selector), fnConvert, scope, globalScope);
	}
};

// MemberObjectAssigner
template<class T, class U, class E, class Converter>
struct Epic::EON::detail::MemberObjectAssigner : public Assigner<T>
{
public:
	U T::* pDest;
	Parser<E> Ext;
	Converter fnConvert;

private:
	using Traits = EONTraits<T>;

	using MakeAssignTag =
		std::conditional_t<!Traits::IsContainer, ScalarTag,
		std::conditional_t<Traits::IsVectorLike, ArrayTag,
		std::conditional_t<Traits::IsSetLike, SetTag,
		std::conditional_t<Traits::IsMapLike, MapTag,
		FailTag>>>>;

private:
	bool DoAssign(T&, Selector, const EONVariant&, const EONObject&, FailTag) const
	{
		return false;
	}

	bool DoAssign(T& to, Selector selector, const EONVariant& scope, 
				  const EONObject& globalScope, ScalarTag) const
	{
		return DoScalarAssign(to.*pDest, std::move(selector), Ext, fnConvert, scope, globalScope);
	}

	bool DoAssign(T& to, Selector selector, const EONVariant& scope, const EONObject& globalScope, ArrayTag) const
	{
		return DoArrayAssign(to.*pDest, std::move(selector), Ext, fnConvert, scope, globalScope);
	}

	bool DoAssign(T& to, Selector selector, const EONVariant& scope, const EONObject& globalScope, SetTag) const
	{
		return DoSetAssign(to.*pDest, std::move(selector), Ext, fnConvert, scope, globalScope);
	}

	bool DoAssign(T& to, Selector selector, const EONVariant& scope, const EONObject& globalScope, MapTag) const
	{
		return DoMapAssign(to.*pDest, std::move(selector), Ext, fnConvert, scope, globalScope);
	}

public:
	MemberObjectAssigner(U T::* dest, Parser<E> extractor, Converter convert)
		: pDest(dest), Ext(std::move(extractor)), fnConvert(std::move(convert)) { }

	bool Assign(T& to, Selector selector, const EONVariant& scope, const EONObject& globalScope) const override
	{
		return DoAssign(to, std::move(selector), scope, globalScope, MakeAssignTag());
	}
};

// MemberAttributeAssigner
template<class T, class U, class Converter>
struct Epic::EON::detail::MemberAttributeAssigner : public Assigner<T>
{
	U T::* pDest;
	eAttribute Attribute;
	Converter fnConvert;

	MemberAttributeAssigner(eAttribute attr, U T::* dest, Converter convertFn)
		: Attribute(attr), pDest(dest), fnConvert(std::move(convertFn)) { }

	bool Assign(T& to, Selector, const EONVariant& scope, const EONObject& globalScope) const override
	{
		auto [isValid, pVariable, index] = GetAttributes(&scope, globalScope);

		if (!isValid) return false;

		switch (Attribute)
		{
			case eAttribute::Name:
				if (!ConvertIf(fnConvert, to.*pDest, pVariable->Name))
					return false;
				break;

			case eAttribute::Type:
				if (!ConvertIf(fnConvert, to.*pDest, std::visit(TypeNameVisitor(), scope.Data)))
					return false;
				break;

			case eAttribute::Index:
				if (!ConvertIf(fnConvert, to.*pDest, std::move(index)))
					return false;
				break;

			case eAttribute::Parent:
				if (!ConvertIf(fnConvert, to.*pDest, pVariable->Parent))
					return false;
				break;

			default:
				return false;
		}

		return true;
	}
};
