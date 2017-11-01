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

#include <Epic/EON/Parser.hpp>
#include <Epic/EON/ResultPolicy.hpp>
#include <Epic/EON/Selector.hpp>
#include <Epic/EON/detail/Tags.hpp>
#include <Epic/EON/detail/Traits.hpp>
#include <Epic/EON/detail/Utility.hpp>
#include <Epic/EON/detail/Visitors.hpp>
#include <algorithm>
#include <cassert>
#include <optional>
#include <type_traits>

//////////////////////////////////////////////////////////////////////////////

namespace Epic::EON
{
	class Extractor;
}

//////////////////////////////////////////////////////////////////////////////

// Extractor
class Epic::EON::Extractor
{
public:
	using Type = Epic::EON::Extractor;

protected:
	const EONObject& m_Scope;

public:
	Extractor() = delete;
	Extractor(const EONObject& scope) noexcept : m_Scope{ scope } { }
	Extractor(const Type&) noexcept = default;
	Extractor(Type&&) noexcept = default;
	
	Extractor& operator= (const Type&) = delete;
	Extractor& operator= (Type&&) = delete;

private:
	template<class T>
	struct ExtractHelper
	{
	private:
		using Traits = detail::EONTraits<T>;

		using MakeAssignTag =
			std::conditional_t<!Traits::IsContainer, detail::ScalarTag,
			std::conditional_t<Traits::IsVectorLike, detail::ArrayTag,
			std::conditional_t<Traits::IsSetLike, detail::SetTag,
			std::conditional_t<Traits::IsMapLike, detail::MapTag,
			detail::FailTag>>>>;

		using MakeAttrAssignTag =
			std::conditional_t<!Traits::IsContainer, detail::ScalarTag,
			std::conditional_t<Traits::IsVectorLike, detail::ArrayTag,
			std::conditional_t<Traits::IsSetLike, detail::SetTag,
			detail::FailTag>>>;
		
		// DoExtract - Fail
		template<class Converter = DefaultConverter>
		static bool DoExtract(T&, Converter, const Selector::MatchList&, const EONObject&, detail::FailTag) noexcept
		{
			return false;
		}

		// DoExtract - Parser<E> - Fail
		template<class E, class Converter = DefaultConverter>
		static bool DoExtract(T&, Parser<E>, Converter, const Selector::MatchList&, const EONObject&, detail::FailTag) noexcept
		{
			return false;
		}

		// DoExtract - Attribute - Fail
		template<class Converter = DefaultConverter>
		static bool DoExtract(T&, eAttribute, Converter, const Selector::MatchList&, const EONObject&, detail::FailTag) noexcept
		{
			return false;
		}

		// DoExtract - Scalar
		template<class Converter = DefaultConverter>
		static bool DoExtract(T& to, Converter fnConvert, const Selector::MatchList& vars, 
							  const EONObject& scope, detail::ScalarTag)
		{
			for (const auto& var : vars)
			{
				if (!std::visit(detail::ConversionVisitor<T, Converter>(to, std::move(fnConvert), scope), var.second->Data))
					return false;
			}

			return true;
		}

		// DoExtract - Parser<E> - Scalar
		template<class E, class Converter = DefaultConverter>
		static bool DoExtract(T& to, Parser<E> parser, Converter fnConvert, 
							  const Selector::MatchList& vars, const EONObject& scope, detail::ScalarTag)
		{
			static_assert(std::is_default_constructible_v<E>, "Parsed type must be default constructible.");

			for (const auto& var : vars)
			{
				E extracted;

				if (!parser.Assign(extracted, *var.second, scope))
					return false;

				if (!detail::ConvertIf(std::move(fnConvert), to, std::move(extracted)))
					return false;
			}

			return true;
		}

		// DoExtract - Attribute - Scalar
		template<class Converter = DefaultConverter>
		static bool DoExtract(T& to, eAttribute attr, Converter fnConvert, const Selector::MatchList& vars, 
							  const EONObject& scope, detail::ScalarTag)
		{
			for (const auto& var : vars)
			{
				auto [isValid, pVariable, index] = detail::GetAttributes(var.second, scope);

				if (!isValid) return false;

				switch (attr)
				{
					case eAttribute::Name:
						if (!detail::ConvertIf(std::move(fnConvert), to, pVariable->Name))
							return false;
						break;

					case eAttribute::Type:
						if (!detail::ConvertIf(std::move(fnConvert), to, std::visit(detail::TypeNameVisitor(), var.second->Data)))
							return false;
						break;

					case eAttribute::Index:
						if (!detail::ConvertIf(std::move(fnConvert), to, std::move(index)))
							return false;
						break;

					case eAttribute::Parent:
						if (!detail::ConvertIf(std::move(fnConvert), to, pVariable->Parent))
							return false;
						break;

					default:
						return false;
				}
			}

			return true;
		}

		// DoExtract - Array
		template<class Converter = DefaultConverter>
		static bool DoExtract(T& to, Converter fnConvert, const Selector::MatchList& vars, 
							  const EONObject& scope, detail::ArrayTag)
		{
			using Item = typename T::value_type;

			static_assert(std::is_default_constructible_v<Item>, "Value type must be default constructible.");

			for (const auto& var : vars)
			{
				Item& item = to.emplace_back();

				if (!std::visit(detail::ConversionVisitor<Item, Converter>(item, std::move(fnConvert), scope), var.second->Data))
					return false;
			}

			return true;
		}

		// DoExtract - Array
		template<class E, class Converter = DefaultConverter>
		static bool DoExtract(T& to, Parser<E> parser, Converter fnConvert, 
							  const Selector::MatchList& vars, const EONObject& scope, detail::ArrayTag)
		{
			using Item = typename T::value_type;

			static_assert(std::is_default_constructible_v<E>, "Parsed type must be default constructible.");
			static_assert(std::is_default_constructible_v<Item>, "Value type must be default constructible.");

			for (const auto& var : vars)
			{
				E extracted;
				if (!parser.Assign(extracted, *var.second, scope))
					return false;

				Item& item = to.emplace_back();

				if (!detail::ConvertIf(std::move(fnConvert), item, std::move(extracted)))
					return false;
			}

			return true;
		}

		// DoExtract - Attribute - Array
		template<class Converter = DefaultConverter>
		static bool DoExtract(T& to, eAttribute attr, Converter fnConvert, const Selector::MatchList& vars, 
							  const EONObject& scope, detail::ArrayTag)
		{
			using Item = typename T::value_type;

			static_assert(std::is_default_constructible_v<Item>, "Value type must be default constructible.");

			for (const auto& var : vars)
			{
				Item& item = to.emplace_back();

				auto [isValid, pVariable, index] = detail::GetAttributes(var.second, scope);

				if (!isValid) return false;

				switch (attr)
				{
					case eAttribute::Name:
						if (!detail::ConvertIf(std::move(fnConvert), item, pVariable->Name))
							return false;
						break;

					case eAttribute::Type:
						if (!detail::ConvertIf(std::move(fnConvert), item, std::visit(detail::TypeNameVisitor(), var.second->Data)))
							return false;
						break;

					case eAttribute::Index:
						if (!detail::ConvertIf(std::move(fnConvert), item, std::move(index)))
							return false;
						break;

					case eAttribute::Parent:
						if (!detail::ConvertIf(std::move(fnConvert), item, pVariable->Parent))
							return false;
						break;

					default:
						return false;
				}
			}

			return true;
		}

		// DoExtract - Set
		template<class Converter = DefaultConverter>
		static bool DoExtract(T& to, Converter fnConvert, const Selector::MatchList& vars, 
							  const EONObject& scope, detail::SetTag)
		{
			using Item = typename T::key_type;

			static_assert(std::is_default_constructible_v<Item>, "Key type must be default constructible.");

			for (const auto& var : vars)
			{
				Item item;

				if (!std::visit(detail::ConversionVisitor<Item, Converter>(item, std::move(fnConvert), scope), var.second->Data))
					return false;

				to.emplace(std::move(item));
			}

			return true;
		}

		// DoExtract - Parser<E> - Set
		template<class E, class Converter = DefaultConverter>
		static bool DoExtract(T& to, Parser<E> parser, Converter fnConvert, 
							  const Selector::MatchList& vars, const EONObject& scope, detail::SetTag)
		{
			using Item = typename T::key_type;

			static_assert(std::is_default_constructible_v<E>, "Parsed type must be default constructible.");
			static_assert(std::is_default_constructible_v<Item>, "Key type must be default constructible.");

			for (const auto& var : vars)
			{
				E extracted;
				if (!parser.Assign(extracted, *var.second, scope))
					return false;

				Item item;
				if (!detail::ConvertIf(std::move(fnConvert), item, std::move(extracted)))
					return false;

				to.emplace(std::move(item));
			}

			return true;
		}

		// DoExtract - Attribute - Set
		template<class Converter = DefaultConverter>
		static bool DoExtract(T& to, eAttribute attr, Converter fnConvert, const Selector::MatchList& vars,
							  const EONObject& scope, detail::SetTag)
		{
			using Item = typename T::key_type;

			static_assert(std::is_default_constructible_v<Item>, "Key type must be default constructible.");

			for (const auto& var : vars)
			{
				Item item;

				auto [isValid, pVariable, index] = detail::GetAttributes(var.second, scope);

				if (!isValid) return false;

				switch (attr)
				{
					case eAttribute::Name:
						if (!detail::ConvertIf(std::move(fnConvert), item, pVariable->Name))
							return false;
						break;

					case eAttribute::Type:
						if (!detail::ConvertIf(std::move(fnConvert), item, std::visit(detail::TypeNameVisitor(), var.second->Data)))
							return false;
						break;

					case eAttribute::Index:
						if (!detail::ConvertIf(std::move(fnConvert), item, std::move(index)))
							return false;
						break;

					case eAttribute::Parent:
						if (!detail::ConvertIf(std::move(fnConvert), item, pVariable->Parent))
							return false;
						break;

					default:
						return false;
				}
				
				to.emplace(std::move(item));
			}

			return true;
		}

		// DoExtract - Map
		template<class Converter = DefaultConverter>
		static bool DoExtract(T& to, Converter fnConvert, const Selector::MatchList& vars, 
							  const EONObject& scope, detail::MapTag)
		{
			using Key = typename T::key_type;
			using Item = typename T::mapped_type;

			static_assert(std::is_default_constructible_v<Key>, "Key type must be default constructible.");
			static_assert(std::is_default_constructible_v<Item>, "Value type must be default constructible.");

			for (const auto& var : vars)
			{
				Key key;

				if (!detail::ConvertIf(fnConvert, key, var.first))
					return false;

				Item item;

				if (!std::visit(detail::ConversionVisitor<Item, Converter>(item, std::move(fnConvert), scope), var.second->Data))
					return false;

				to.emplace(std::move(key), std::move(item));
			}

			return true;
		}

		// DoExtract - Parser<E> - Map
		template<class E, class Converter = DefaultConverter>
		static bool DoExtract(T& to, Parser<E> parser, Converter fnConvert, 
							  const Selector::MatchList& vars, const EONObject& scope, detail::MapTag)
		{
			using Key = typename T::key_type;
			using Item = typename T::mapped_type;

			static_assert(std::is_default_constructible_v<Key>, "Key type must be default constructible.");
			static_assert(std::is_default_constructible_v<Item>, "Value type must be default constructible.");

			for (const auto& var : vars)
			{
				Key key;

				if (!detail::ConvertIf(fnConvert, key, var.first))
					return false;

				E extracted;

				if (!parser.Assign(extracted, *var.second, scope))
					return false;

				Item item;

				if (!detail::ConvertIf(std::move(fnConvert), item, std::move(extracted)))
					return false;

				to.emplace(std::move(key), std::move(item));
			}

			return true;
		}

	public:
		template<class ResultPolicy, class Converter>
		static auto Extract(Selector selector, Converter fnConvert, 
							ResultPolicy& result, const EONObject& scope) -> typename ResultPolicy::ResultType
		{
			static_assert(std::is_default_constructible_v<T>, "Extracted type must be default constructible.");

			auto matches = selector.Evaluate(scope);

			if (std::empty(matches))
				return std::move(result.Empty(selector.Path(), selector.IsOptional()));

			if (T extracted; DoExtract(extracted, std::move(fnConvert), matches, scope, MakeAssignTag()))
				return result.Success(std::move(extracted));
			else
				return std::move(result.Failed(selector.Path()));
		}

		template<class ResultPolicy, class Converter>
		static auto Extract(Selector selector, eAttribute attr, Converter fnConvert, 
							ResultPolicy& result, const EONObject& scope) -> typename ResultPolicy::ResultType
		{
			static_assert(std::is_default_constructible_v<T>, "Extracted type must be default constructible.");

			auto matches = selector.Evaluate(scope);

			if (std::empty(matches))
				return std::move(result.Empty(selector.Path(), selector.IsOptional()));

			if (T extracted; DoExtract(extracted, attr, std::move(fnConvert), matches, scope, MakeAttrAssignTag()))
				return result.Success(std::move(extracted));
			else
				return std::move(result.Failed(selector.Path()));
		}

		template<class E, class ResultPolicy, class Converter>
		static auto Extract(Selector selector, Parser<E> parser, Converter fnConvert, 
							ResultPolicy& result, const EONObject& scope) -> typename ResultPolicy::ResultType
		{
			static_assert(std::is_default_constructible_v<T>, "Extracted type must be default constructible.");

			auto matches = selector.Evaluate(scope);

			if (std::empty(matches))
				return std::move(result.Empty(selector.Path(), selector.IsOptional()));

			if (T extracted; DoExtract(extracted, std::move(parser), std::move(fnConvert), matches, scope, MakeAssignTag()))
				return result.Success(std::move(extracted));
			else
				return std::move(result.Failed(selector.Path()));
		}
	};

public:
	template<class T, template<class> class ResultPolicy = ThrowResult, 
			 class Converter = DefaultConverter, class... Args>
	auto Extract(Selector selector, Converter fnConvert = Converter(), Args&&... resultPolicyArgs)
	{
		return ExtractHelper<T>::Extract
		(
			std::move(selector),
			std::move(fnConvert),
			ResultPolicy<T>{ std::forward<Args>(resultPolicyArgs)... }, 
			m_Scope
		);
	}

	template<class T, template<class> class ResultPolicy = ThrowResult,
			 class Converter = DefaultConverter, class E, class... Args>
	auto Extract(Selector selector, Parser<E> parser, 
				 Converter fnConvert = Converter(), Args&&... resultPolicyArgs)
	{
		return ExtractHelper<T>::Extract
		(
			std::move(selector),
			std::move(parser),
			std::move(fnConvert),
			ResultPolicy<T>{ std::forward<Args>(resultPolicyArgs)... }, 
			m_Scope
		);
	}

	template<class T, template<class> class ResultPolicy = ThrowResult,
			 class Converter = DefaultConverter, class... Args>
	auto Extract(Selector selector, eAttribute attr, Converter fnConvert = Converter(), Args&&... resultPolicyArgs)
	{
		return ExtractHelper<T>::Extract
		(
			std::move(selector),
			attr, 
			std::move(fnConvert), 
			ResultPolicy<T>{ std::forward<Args>(resultPolicyArgs)... }, 
			m_Scope
		);
	}

	template<class T, class Converter = DefaultConverter>
	auto TryExtract(Selector selector, Converter fnConvert = Converter())
	{
		return ExtractHelper<T>::Extract
		(
			std::move(selector),
			std::move(fnConvert), 
			OptionalResult<T>(), 
			m_Scope
		);
	}

	template<class T, class Converter = DefaultConverter, class E>
	auto TryExtract(Selector selector, Parser<E> parser, Converter fnConvert = Converter())
	{
		return ExtractHelper<T>::Extract
		(
			std::move(selector),
			std::move(parser),
			std::move(fnConvert), 
			OptionalResult<T>(), 
			m_Scope
		);
	}

	template<class T, class Converter = DefaultConverter>
	auto TryExtract(Selector selector, eAttribute attr, Converter fnConvert = Converter())
	{
		return ExtractHelper<T>
		(
			std::move(selector),
			attr, 
			std::move(fnConvert), 
			OptionalResult<T>(), 
			m_Scope
		);
	}

	template<class T, template<class> class ResultPolicy = GuaranteedResult, 
			 class Converter = DefaultConverter>
	auto ExtractOr(Selector selector, T&& defaultVal, Converter fnConvert = Converter())
	{
		return ExtractHelper<T>::Extract
		(
			std::move(selector),
			std::move(fnConvert), 
			ResultPolicy<T>{ std::forward<T>(defaultVal) }, 
			m_Scope
		);
	}

	template<class T, template<class> class ResultPolicy = GuaranteedResult,
			 class E, class Converter = DefaultConverter>
	auto ExtractOr(Selector selector, Parser<E> parser, E&& defaultVal, Converter fnConvert = Converter())
	{
		return ExtractHelper<T>::Extract
		(
			std::move(selector), 
			std::move(parser), 
			std::move(fnConvert), 
			ResultPolicy<T>{ std::forward<E>(defaultVal) }, 
			m_Scope
		);
	}
};