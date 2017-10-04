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
#include <Epic/EON/detail/Utility.hpp>
#include <Epic/EON/detail/Traits.hpp>
#include <Epic/EON/detail/Visitors.hpp>

//////////////////////////////////////////////////////////////////////////////

namespace Epic::EON::detail
{
	template<class T>
	struct Assigner;

	template<class T, class Converter>
	struct FreeAssigner;

	template<class T, class E, class Converter>
	struct FreeObjectAssigner;

	template<class T, class U, class Converter>
	struct MemberAssigner;

	template<class T, class U, class E, class Converter>
	struct MemberObjectAssigner;
}

//////////////////////////////////////////////////////////////////////////////

// Helpers
namespace Epic::EON::detail
{
	namespace
	{
		template<class T, class Converter>
		bool DoConvertAssign(T& to, const Selector& selector, Converter fnConvert, const EONVariant& scope)
		{
			auto vars = selector.Evaluate(&scope);
			if (std::empty(vars))
				return selector.IsOptional();

			for (const auto& var : vars)
				if (!std::visit(ConversionVisitor<T, Converter>(to, fnConvert), var.second->Data))
					return false;

			return true;
		}

		template<class T, class E, class Converter>
		bool DoScalarAssign(T& to, const Selector& selector, const Parser<E>& parser, Converter fnConvert, const EONVariant& scope)
		{
			auto vars = selector.Evaluate(&scope);
			if (std::empty(vars))
				return selector.IsOptional();

			for (const auto& var : vars)
			{
				const auto pAsObject = std::get_if<EONObject>(&var.second->Data);
				if (!pAsObject)
					return false;

				E extracted;

				if (!parser.Assign(extracted, *var.second))
					return false;

				if (!ConvertIf<E, T, Converter>::Apply(fnConvert, to, extracted))
					return false;
			}

			return true;
		}

		template<class T, class E, class Converter>
		bool DoArrayAssign(T& to, const Selector& selector, const Parser<E>& parser, Converter fnConvert, const EONVariant& scope)
		{
			auto vars = selector.Evaluate(&scope);
			if (std::empty(vars))
				return selector.IsOptional();

			for (const auto& var : vars)
			{
				const auto pAsArray = std::get_if<EONArray>(&var.second->Data);
				if (!pAsArray)
					return false;

				T items;

				for (const auto& vm : pAsArray->Members)
				{
					const auto pAsObject = std::get_if<EONObject>(&vm.Data);
					if (!pAsObject)
						return false;

					E extracted;
					if (!parser.Assign(extracted, vm))
						return false;

					typename T::value_type item;
					if (!ConvertIf<E, typename T::value_type, Converter>::Apply(fnConvert, item, extracted))
						return false;

					items.emplace_back(std::move(item));
				}

				to = std::move(items);
			}

			return true;
		}

		template<class T, class E, class Converter>
		bool DoSetAssign(T& to, const Selector& selector, const Parser<E>& parser, Converter fnConvert, const EONVariant& scope)
		{
			auto vars = selector.Evaluate(&scope);
			if (std::empty(vars))
				return selector.IsOptional();

			for (const auto& var : vars)
			{
				const auto pAsArray = std::get_if<EONArray>(&var.second->Data);
				if (!pAsArray)
					return false;

				T items;

				for (const auto& vm : pAsArray->Members)
				{
					const auto pAsObject = std::get_if<EONObject>(&vm.Data);
					if (!pAsObject)
						return false;

					E extracted;
					if (!parser.Assign(extracted, vm))
						return false;

					typename T::key_type item;
					if (!ConvertIf<E, typename T::key_type, Converter>::Apply(fnConvert, item, extracted))
						return false;

					items.emplace(std::move(item));
				}

				to = std::move(items);
			}

			return true;
		}

		template<class T, class E, class Converter>
		bool DoMapAssign(T& to, const Selector& selector, const Parser<E>& parser, Converter fnConvert, const EONVariant& scope)
		{
			auto vars = selector.Evaluate(&scope);
			if (std::empty(vars))
				return selector.IsOptional();

			for (const auto& var : vars)
			{
				const auto pAsObject = std::get_if<EONObject>(&var.second->Data);
				if (!pAsObject)
					return false;

				T items;

				for (const auto& vm : pAsObject->Members)
				{
					typename T::key_type key;
					if (!ConvertIf<decltype(vm.Name), typename T::key_type, Converter>::Apply(fnConvert, key, vm.Name))
						return false;

					const auto pItemAsObject = std::get_if<EONObject>(&vm.Value.Data);
					if (!pItemAsObject)
						return false;

					E extracted;
					if (!parser.Assign(extracted, vm.Value))
						return false;

					typename T::mapped_type item;
					if (!ConvertIf<E, typename T::mapped_type, Converter>::Apply(fnConvert, item, extracted))
						return false;

					items.emplace(std::move(key), std::move(item));
				}

				to = std::move(items);
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
	virtual bool Assign(T& to, const Selector& selector, const EONVariant& scope) const = 0;
};

// FreeAssigner
template<class T, class Converter>
struct Epic::EON::detail::FreeAssigner : public Assigner<T>
{
	Converter fnConvert;

	FreeAssigner() noexcept
		: fnConvert() { }

	explicit FreeAssigner(Converter convert)
		: fnConvert(convert) { }

	bool Assign(T& to, const Selector& selector, const EONVariant& scope) const override
	{
		return DoConvertAssign(to, selector, fnConvert, scope);
	}
};

// FreeObjectAssigner
template<class T, class E, class Converter>
struct Epic::EON::detail::FreeObjectAssigner : public Assigner<T>
{
public:
	Parser<E> Ext;
	Converter fnConvert;

private:
	using Traits = EONTraits<T>;

	struct FailTag { };
	struct ScalarTag { };
	struct ArrayTag { };
	struct SetTag { };
	struct MapTag { };

	using MakeAssignTag =
		std::conditional_t<!Traits::IsContainer, ScalarTag,
		std::conditional_t<Traits::IsVectorLike, ArrayTag,
		std::conditional_t<Traits::IsSetLike, SetTag,
		std::conditional_t<Traits::IsMapLike, MapTag,
		FailTag>>>>;

private:
	bool DoAssign(T&, const Selector&, const EONVariant&, FailTag) const
	{
		return false;
	}

	bool DoAssign(T& to, const Selector& selector, const EONVariant& scope, ScalarTag) const
	{
		static_assert(std::is_default_constructible_v<E>, "Extracted type must be default constructible.");

		return DoScalarAssign(to, selector, Ext, fnConvert, scope);
	}

	bool DoAssign(T& to, const Selector& selector, const EONVariant& scope, ArrayTag) const
	{
		static_assert(std::is_default_constructible_v<T>, "Array type must be default constructible.");
		static_assert(std::is_default_constructible_v<typename T::value_type>, "Value type must be default constructible.");
		static_assert(std::is_default_constructible_v<E>, "Extracted type must be default constructible.");

		return DoArrayAssign(to, selector, Ext, fnConvert, scope);
	}

	bool DoAssign(T& to, const Selector& selector, const EONVariant& scope, SetTag) const
	{
		static_assert(std::is_default_constructible_v<T>, "Set type must be default constructible.");
		static_assert(std::is_default_constructible_v<typename T::key_type>, "Key type must be default constructible.");
		static_assert(std::is_default_constructible_v<E>, "Extracted type must be default constructible.");

		return DoSetAssign(to, selector, Ext, fnConvert, scope);
	}

	bool DoAssign(T& to, const Selector& selector, const EONVariant& scope, MapTag) const
	{
		static_assert(std::is_default_constructible_v<T>, "Map type must be default constructible.");
		static_assert(std::is_default_constructible_v<typename T::key_type>, "Key type must be default constructible.");
		static_assert(std::is_default_constructible_v<typename T::mapped_type>, "Value type must be default constructible.");
		static_assert(std::is_default_constructible_v<E>, "Extracted type must be default constructible.");

		return DoMapAssign(to, selector, Ext, fnConvert, scope);
	}

public:
	explicit FreeObjectAssigner(const Parser<E>& extractor)
		: Ext(extractor), fnConvert() { }

	FreeObjectAssigner(const Parser<E>& extractor, Converter convert)
		: Ext(extractor), fnConvert(convert) { }

	bool Assign(T& to, const Selector& selector, const EONVariant& scope) const override
	{
		return DoAssign(to, selector, scope, MakeAssignTag());
	}
};

// MemberAssigner
template<class T, class U, class Converter>
struct Epic::EON::detail::MemberAssigner : public Assigner<T>
{
	U T::* pDest;
	Converter fnConvert;

	explicit MemberAssigner(U T::* dest)
		: pDest(dest), fnConvert() { }

	MemberAssigner(U T::* dest, Converter convert)
		: pDest(dest), fnConvert(convert) { }

	bool Assign(T& to, const Selector& selector, const EONVariant& scope) const override
	{
		return DoConvertAssign(to.*pDest, selector, fnConvert, scope);
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

	struct FailTag { };
	struct ScalarTag { };
	struct ArrayTag { };
	struct SetTag { };
	struct MapTag { };

	using MakeAssignTag =
		std::conditional_t<!Traits::IsContainer, ScalarTag,
		std::conditional_t<Traits::IsVectorLike, ArrayTag,
		std::conditional_t<Traits::IsSetLike, SetTag,
		std::conditional_t<Traits::IsMapLike, MapTag,
		FailTag>>>>;

private:
	bool DoAssign(T&, const Selector&, const EONVariant&, FailTag) const
	{
		return false;
	}

	bool DoAssign(T& to, const Selector& selector, const EONVariant& scope, ScalarTag) const
	{
		static_assert(std::is_default_constructible_v<E>, "Extracted type must be default constructible.");

		return DoScalarAssign(to.*pDest, selector, Ext, fnConvert, scope);
	}

	bool DoAssign(T& to, const Selector& selector, const EONVariant& scope, ArrayTag) const
	{
		static_assert(std::is_default_constructible_v<U>, "Array type must be default constructible.");
		static_assert(std::is_default_constructible_v<typename U::value_type>, "Value type must be default constructible.");
		static_assert(std::is_default_constructible_v<E>, "Extracted type must be default constructible.");

		return DoArrayAssign(to.*pDest, selector, Ext, fnConvert, scope);
	}

	bool DoAssign(T& to, const Selector& selector, const EONVariant& scope, SetTag) const
	{
		static_assert(std::is_default_constructible_v<U>, "Set type must be default constructible.");
		static_assert(std::is_default_constructible_v<typename U::key_type>, "Key type must be default constructible.");
		static_assert(std::is_default_constructible_v<E>, "Extracted type must be default constructible.");

		return DoSetAssign(to.*pDest, selector, Ext, fnConvert, scope);
	}

	bool DoAssign(T& to, const Selector& selector, const EONVariant& scope, MapTag) const
	{
		static_assert(std::is_default_constructible_v<U>, "Map type must be default constructible.");
		static_assert(std::is_default_constructible_v<typename U::key_type>, "Key type must be default constructible.");
		static_assert(std::is_default_constructible_v<typename U::mapped_type>, "Value type must be default constructible.");
		static_assert(std::is_default_constructible_v<E>, "Extracted type must be default constructible.");

		return DoMapAssign(to.*pDest, selector, Ext, fnConvert, scope);
	}

public:
	MemberObjectAssigner(U T::* dest, const Parser<E>& extractor)
		: pDest(dest), Ext(extractor), fnConvert() { }

	MemberObjectAssigner(U T::* dest, const Parser<E>& extractor, Converter convert)
		: pDest(dest), Ext(extractor), fnConvert(convert) { }

	bool Assign(T& to, const Selector& selector, const EONVariant& scope) const override
	{
		return DoAssign(to, selector, scope, MakeAssignTag());
	}
};
