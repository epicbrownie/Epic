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
	Extractor(const Extractor&) noexcept = default;
	
	Extractor& operator= (const Extractor&) = delete;

private:
	struct FailTag { };
	struct ScalarTag { };
	struct ArrayTag { };
	struct SetTag { };
	struct MapTag { };

	template<class T>
	struct ExtractHelper
	{
	private:
		using Traits = detail::EONTraits<T>;

		using MakeAssignTag =
			std::conditional_t<!Traits::IsContainer, ScalarTag,
			std::conditional_t<Traits::IsVectorLike, ArrayTag,
			std::conditional_t<Traits::IsSetLike, SetTag,
			std::conditional_t<Traits::IsMapLike, MapTag,
			FailTag>>>>;

		using MakeAttrAssignTag =
			std::conditional_t<!Traits::IsContainer, ScalarTag,
			std::conditional_t<Traits::IsVectorLike, ArrayTag,
			std::conditional_t<Traits::IsSetLike, SetTag,
			FailTag>>>;
		
		// DoExtract - Fail
		template<class Converter = DefaultConverter>
		static bool DoExtract(T&, Converter, const Selector::MatchList&, const EONObject&, FailTag) noexcept
		{
			return false;
		}

		// DoExtract - Parser<E> - Fail
		template<class E, class Converter = DefaultConverter>
		static bool DoExtract(T&, const Parser<E>&, Converter, const Selector::MatchList&, const EONObject&, FailTag) noexcept
		{
			return false;
		}

		// DoExtract - Attribute - Fail
		template<class Converter = DefaultConverter>
		static bool DoExtract(T&, eAttribute, Converter, const Selector::MatchList&, const EONObject&, FailTag) noexcept
		{
			return false;
		}

		// DoExtract - Scalar
		template<class Converter = DefaultConverter>
		static bool DoExtract(T& to, Converter fnConvert, const Selector::MatchList& vars, 
							  const EONObject& scope, ScalarTag)
		{
			for (const auto& var : vars)
			{
				if (!std::visit(detail::ConversionVisitor<T, Converter>(to, fnConvert, scope), var.second->Data))
					return false;
			}

			return true;
		}

		// DoExtract - Parser<E> - Scalar
		template<class E, class Converter = DefaultConverter>
		static bool DoExtract(T& to, const Parser<E>& parser, Converter fnConvert, 
							  const Selector::MatchList& vars, const EONObject& scope, ScalarTag)
		{
			for (const auto& var : vars)
			{
				E extracted;

				if (!parser.Assign(extracted, *var.second, scope))
					return false;

				if (!detail::ConvertIf(fnConvert, to, extracted))
					return false;
			}

			return true;
		}

		// DoExtract - Attribute - Scalar
		template<class Converter = DefaultConverter>
		static bool DoExtract(T& to, eAttribute attr, Converter fnConvert, const Selector::MatchList& vars, 
							  const EONObject& scope, ScalarTag)
		{
			for (const auto& var : vars)
			{
				auto [isValid, pVariable, index] = detail::GetAttributes(var.second, scope);

				if (!isValid) return false;

				switch (attr)
				{
					case eAttribute::Name:
						if (!detail::ConvertIf(fnConvert, to, pVariable->Name))
							return false;
						break;

					case eAttribute::Type:
						if (!detail::ConvertIf(fnConvert, to, std::visit(detail::TypeNameVisitor(), var.second->Data)))
							return false;
						break;

					case eAttribute::Index:
						if (!detail::ConvertIf(fnConvert, to, index))
							return false;
						break;

					case eAttribute::Parent:
						if (!detail::ConvertIf(fnConvert, to, pVariable->Parent))
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
							  const EONObject& scope, ArrayTag)
		{
			using Item = typename T::value_type;

			static_assert(std::is_default_constructible_v<T>, "Array type must be default constructible.");
			static_assert(std::is_default_constructible_v<Item>, "Value type must be default constructible.");

			T items;

			for (const auto& var : vars)
			{
				Item item;

				if (!std::visit(detail::ConversionVisitor<Item, Converter>(item, fnConvert, scope), var.second->Data))
					return false;
				
				items.emplace_back(std::move(item));
			}

			to = std::move(items);

			return true;
		}

		// DoExtract - Array
		template<class E, class Converter = DefaultConverter>
		static bool DoExtract(T& to, const Parser<E>& parser, Converter fnConvert, 
							  const Selector::MatchList& vars, const EONObject& scope, ArrayTag)
		{
			using Item = typename T::value_type;

			static_assert(std::is_default_constructible_v<T>, "Array type must be default constructible.");
			static_assert(std::is_default_constructible_v<Item>, "Value type must be default constructible.");

			T items;

			for (const auto& var : vars)
			{
				E extracted;
				if (!parser.Assign(extracted, *var.second, scope))
					return false;

				Item item;
				if (!detail::ConvertIf(fnConvert, item, extracted))
					return false;

				items.emplace_back(std::move(item));
			}

			to = std::move(items);

			return true;
		}

		// DoExtract - Attribute - Array
		template<class Converter = DefaultConverter>
		static bool DoExtract(T& to, eAttribute attr, Converter fnConvert, const Selector::MatchList& vars, 
							  const EONObject& scope, ArrayTag)
		{
			using Item = typename T::value_type;

			static_assert(std::is_default_constructible_v<T>, "Array type must be default constructible.");
			static_assert(std::is_default_constructible_v<Item>, "Value type must be default constructible.");

			T items;

			for (const auto& var : vars)
			{
				Item item;

				auto [isValid, pVariable, index] = detail::GetAttributes(var.second, scope);

				if (!isValid) return false;

				switch (attr)
				{
					case eAttribute::Name:
						if (!detail::ConvertIf(fnConvert, item, pVariable->Name))
							return false;
						break;

					case eAttribute::Type:
						if (!detail::ConvertIf(fnConvert, item, std::visit(detail::TypeNameVisitor(), var.second->Data)))
							return false;
						break;

					case eAttribute::Index:
						if (!detail::ConvertIf(fnConvert, item, index))
							return false;
						break;

					case eAttribute::Parent:
						if (!detail::ConvertIf(fnConvert, item, pVariable->Parent))
							return false;
						break;

					default:
						return false;
				}
				
				items.emplace_back(std::move(item));
			}

			to = std::move(items);

			return true;
		}

		// DoExtract - Set
		template<class Converter = DefaultConverter>
		static bool DoExtract(T& to, Converter fnConvert, const Selector::MatchList& vars, 
							  const EONObject& scope, SetTag)
		{
			using Item = typename T::key_type;

			static_assert(std::is_default_constructible_v<T>, "Set type must be default constructible.");
			static_assert(std::is_default_constructible_v<Item>, "Key type must be default constructible.");

			T items;

			for (const auto& var : vars)
			{
				Item item;

				if (!std::visit(detail::ConversionVisitor<Item, Converter>(item, fnConvert, scope), var.second->Data))
					return false;

				items.emplace(std::move(item));
			}

			to = std::move(items);

			return true;
		}

		// DoExtract - Parser<E> - Set
		template<class E, class Converter = DefaultConverter>
		static bool DoExtract(T& to, const Parser<E>& parser, Converter fnConvert, 
							  const Selector::MatchList& vars, const EONObject& scope, SetTag)
		{
			using Item = typename T::key_type;

			static_assert(std::is_default_constructible_v<T>, "Set type must be default constructible.");
			static_assert(std::is_default_constructible_v<Item>, "Key type must be default constructible.");

			T items;

			for (const auto& var : vars)
			{
				E extracted;
				if (!parser.Assign(extracted, *var.second, scope))
					return false;

				Item item;
				if (!detail::ConvertIf(fnConvert, item, extracted))
					return false;

				items.emplace(std::move(item));
			}

			to = std::move(items);

			return true;
		}

		// DoExtract - Attribute - Set
		template<class Converter = DefaultConverter>
		static bool DoExtract(T& to, eAttribute attr, Converter fnConvert, const Selector::MatchList& vars,
							  const EONObject& scope, SetTag)
		{
			using Item = typename T::key_type;

			static_assert(std::is_default_constructible_v<T>, "Set type must be default constructible.");
			static_assert(std::is_default_constructible_v<Item>, "Key type must be default constructible.");

			T items;

			for (const auto& var : vars)
			{
				Item item;

				auto [isValid, pVariable, index] = detail::GetAttributes(var.second, scope);

				if (!isValid) return false;

				switch (attr)
				{
					case eAttribute::Name:
						if (!detail::ConvertIf(fnConvert, item, pVariable->Name))
							return false;
						break;

					case eAttribute::Type:
						if (!detail::ConvertIf(fnConvert, item, std::visit(detail::TypeNameVisitor(), var.second->Data)))
							return false;
						break;

					case eAttribute::Index:
						if (!detail::ConvertIf(fnConvert, item, index))
							return false;
						break;

					case eAttribute::Parent:
						if (!detail::ConvertIf(fnConvert, item, pVariable->Parent))
							return false;
						break;

					default:
						return false;
				}
				
				items.emplace(std::move(item));
			}

			to = std::move(items);

			return true;
		}

		// DoExtract - Map
		template<class Converter = DefaultConverter>
		static bool DoExtract(T& to, Converter fnConvert, const Selector::MatchList& vars, 
							  const EONObject& scope, MapTag)
		{
			using Key = typename T::key_type;
			using Item = typename T::mapped_type;

			static_assert(std::is_default_constructible_v<T>, "Map type must be default constructible.");
			static_assert(std::is_default_constructible_v<Key>, "Key type must be default constructible.");
			static_assert(std::is_default_constructible_v<Item>, "Value type must be default constructible.");

			T items;

			for (const auto& var : vars)
			{
				Key key;

				if (!detail::ConvertIf(fnConvert, key, var.first))
					return false;

				Item item;

				if (!std::visit(detail::ConversionVisitor<Item, Converter>(item, fnConvert, scope), var.second->Data))
					return false;

				items.emplace(std::move(key), std::move(item));
			}

			to = std::move(items);

			return true;
		}

		// DoExtract - Parser<E> - Map
		template<class E, class Converter = DefaultConverter>
		static bool DoExtract(T& to, const Parser<E>& parser, Converter fnConvert, 
							  const Selector::MatchList& vars, const EONObject& scope, MapTag)
		{
			using Key = typename T::key_type;
			using Item = typename T::mapped_type;

			static_assert(std::is_default_constructible_v<T>, "Map type must be default constructible.");
			static_assert(std::is_default_constructible_v<Key>, "Key type must be default constructible.");
			static_assert(std::is_default_constructible_v<Item>, "Value type must be default constructible.");

			T items;

			for (const auto& var : vars)
			{
				Key key;

				if (!detail::ConvertIf(fnConvert, key, var.first))
					return false;

				E extracted;
				if (!parser.Assign(extracted, *var.second, scope))
					return false;

				Item item;
				if (!detail::ConvertIf(fnConvert, item, extracted))
					return false;

				items.emplace(std::move(key), std::move(item));
			}

			to = std::move(items);

			return true;
		}

	public:
		template<class Converter, class ResultPolicy>
		static auto Extract(const Selector& selector, Converter fnConvert, 
							ResultPolicy& result, const EONObject& scope) -> typename ResultPolicy::ResultType
		{
			static_assert(std::is_default_constructible_v<T>, "Extracted type must be default constructible.");

			auto matches = selector.Evaluate(scope);

			if (std::empty(matches))
				return result.Empty(selector.Path(), selector.IsOptional());

			if (T extracted; DoExtract(extracted, fnConvert, matches, scope, MakeAssignTag()))
				return result.Success(std::move(extracted));
			else
				return result.Failed(selector.Path());
		}

		template<class Converter, class ResultPolicy>
		static auto Extract(const Selector& selector, eAttribute attr, Converter fnConvert, 
							ResultPolicy& result, const EONObject& scope) -> typename ResultPolicy::ResultType
		{
			static_assert(std::is_default_constructible_v<T>, "Extracted type must be default constructible.");

			auto matches = selector.Evaluate(scope);

			if (std::empty(matches))
				return result.Empty(selector.Path(), selector.IsOptional());

			if (T extracted; DoExtract(extracted, attr, fnConvert, matches, scope, MakeAttrAssignTag()))
				return result.Success(std::move(extracted));
			else
				return result.Failed(selector.Path());
		}

		template<class E, class Converter, class ResultPolicy>
		static auto Extract(const Selector& selector, const Parser<E>& parser, Converter fnConvert, 
							ResultPolicy& result, const EONObject& scope) -> typename ResultPolicy::ResultType
		{
			static_assert(std::is_default_constructible_v<T>, "Extracted type must be default constructible.");

			auto matches = selector.Evaluate(scope);

			if (std::empty(matches))
				return result.Empty(selector.Path(), selector.IsOptional());

			if (T extracted; DoExtract(extracted, parser, fnConvert, matches, scope, MakeAssignTag()))
				return result.Success(std::move(extracted));
			else
				return result.Failed(selector.Path());
		}
	};

public:
	template<class T, template<class> class ResultPolicy = ThrowResult, 
			 class Converter = DefaultConverter, class... Args>
	auto Extract(const Selector& selector, Converter fnConvert = Converter(), Args&&... resultPolicyArgs)
	{
		return ExtractHelper<T>::Extract
		(
			selector, 
			fnConvert, 
			ResultPolicy<T>{ std::forward<Args>(resultPolicyArgs)... }, 
			m_Scope
		);
	}

	template<class T, template<class> class ResultPolicy = ThrowResult,
			 class Converter = DefaultConverter, class E, class... Args>
	auto Extract(const Selector& selector, const Parser<E>& parser, 
				 Converter fnConvert = Converter(), Args&&... resultPolicyArgs)
	{
		return ExtractHelper<T>::Extract
		(
			selector, 
			parser, 
			fnConvert, 
			ResultPolicy<T>{ std::forward<Args>(resultPolicyArgs)... }, 
			m_Scope
		);
	}

	template<class T, template<class> class ResultPolicy = ThrowResult,
			 class Converter = DefaultConverter, class... Args>
	auto Extract(const Selector& selector, eAttribute attr, Converter fnConvert = Converter(), Args&&... resultPolicyArgs)
	{
		return ExtractHelper<T>::Extract
		(
			selector, 
			attr, 
			fnConvert, 
			ResultPolicy<T>{ std::forward<Args>(resultPolicyArgs)... }, 
			m_Scope
		);
	}

	template<class T, class Converter = DefaultConverter>
	auto TryExtract(const Selector& selector, Converter fnConvert = Converter())
	{
		return ExtractHelper<T>::Extract
		(
			selector, 
			fnConvert, 
			OptionalResult<T>(), 
			m_Scope
		);
	}

	template<class T, class Converter = DefaultConverter, class E>
	auto TryExtract(const Selector& selector, const Parser<E>& parser, Converter fnConvert = Converter())
	{
		return ExtractHelper<T>::Extract
		(
			selector, 
			parser, 
			fnConvert, 
			OptionalResult<T>(), 
			m_Scope
		);
	}

	template<class T, class Converter = DefaultConverter>
	auto TryExtract(const Selector& selector, eAttribute attr, Converter fnConvert = Converter())
	{
		return ExtractHelper<T>
		(
			selector, 
			attr, 
			fnConvert, 
			OptionalResult<T>(), 
			m_Scope
		);
	}

	template<class T, template<class> class ResultPolicy = GuaranteedResult, 
			 class Converter = DefaultConverter>
	auto ExtractOr(const Selector& selector, T&& defaultVal, Converter fnConvert = Converter())
	{
		return ExtractHelper<T>::Extract
		(
			selector, 
			fnConvert, 
			ResultPolicy<T>{ std::forward<T>(defaultVal) }, 
			m_Scope
		);
	}

	template<class T, template<class> class ResultPolicy = GuaranteedResult,
			 class E, class Converter = DefaultConverter>
	auto ExtractOr(const Selector& selector, const Parser<E>& parser, 
				   E&& defaultVal, Converter fnConvert = Converter())
	{
		return ExtractHelper<T>::Extract
		(
			selector, 
			parser, 
			fnConvert, 
			ResultPolicy<T>{ std::forward<E>(defaultVal) }, 
			m_Scope
		);
	}
};