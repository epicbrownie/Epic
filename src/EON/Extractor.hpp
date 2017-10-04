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
		
		template<class Converter = DefaultConverter>
		static bool DoExtract(T&, Converter, const Selector::MatchList&, FailTag) noexcept
		{
			return false;
		}

		template<class E, class Converter = DefaultConverter>
		static bool DoExtract(T&, const Parser<E>&, Converter, const Selector::MatchList&, FailTag) noexcept
		{
			return false;
		}

		template<class Converter = DefaultConverter>
		static bool DoExtract(T& to, Converter fnConvert, const Selector::MatchList& vars, ScalarTag)
		{
			for (const auto& var : vars)
			{
				if (!std::visit(detail::ConversionVisitor<T, Converter>(to, fnConvert), var.second->Data))
					return false;
			}

			return true;
		}

		template<class E, class Converter = DefaultConverter>
		static bool DoExtract(T& to, const Parser<E>& parser, Converter fnConvert, const Selector::MatchList& vars, ScalarTag)
		{
			for (const auto& var : vars)
			{
				E extracted;

				if (!parser.Assign(extracted, *var.second))
					return false;

				if (!detail::ConvertIf<E, T, Converter>::Apply(fnConvert, to, extracted))
					return false;
			}

			return true;
		}

		template<class Converter = DefaultConverter>
		static bool DoExtract(T& to, Converter fnConvert, const Selector::MatchList& vars, ArrayTag)
		{
			using Item = typename T::value_type;

			static_assert(std::is_default_constructible_v<T>, "Array type must be default constructible.");
			static_assert(std::is_default_constructible_v<Item>, "Value type must be default constructible.");

			T items;

			for (const auto& var : vars)
			{
				Item item;

				if (!std::visit(detail::ConversionVisitor<Item, Converter>(item, fnConvert), var.second->Data))
					return false;
				
				items.emplace_back(std::move(item));
			}

			to = std::move(items);

			return true;
		}

		template<class E, class Converter = DefaultConverter>
		static bool DoExtract(T& to, const Parser<E>& parser, Converter fnConvert, const Selector::MatchList& vars, ArrayTag)
		{
			using Item = typename T::value_type;

			static_assert(std::is_default_constructible_v<T>, "Array type must be default constructible.");
			static_assert(std::is_default_constructible_v<Item>, "Value type must be default constructible.");

			T items;

			for (const auto& var : vars)
			{
				E extracted;
				if (!parser.Assign(extracted, *var.second))
					return false;

				Item item;
				if (!detail::ConvertIf<E, Item, Converter>::Apply(fnConvert, item, extracted))
					return false;

				items.emplace_back(std::move(item));
			}

			to = std::move(items);

			return true;
		}

		template<class Converter = DefaultConverter>
		static bool DoExtract(T& to, Converter fnConvert, const Selector::MatchList& vars, SetTag)
		{
			using Item = typename T::key_type;

			static_assert(std::is_default_constructible_v<T>, "Set type must be default constructible.");
			static_assert(std::is_default_constructible_v<Item>, "Key type must be default constructible.");

			T items;

			for (const auto& var : vars)
			{
				Item item;

				if (!std::visit(detail::ConversionVisitor<Item, Converter>(item, fnConvert), var.second->Data))
					return false;

				items.emplace(std::move(item));
			}

			to = std::move(items);

			return true;
		}

		template<class E, class Converter = DefaultConverter>
		static bool DoExtract(T& to, const Parser<E>& parser, Converter fnConvert, const Selector::MatchList& vars, SetTag)
		{
			using Item = typename T::key_type;

			static_assert(std::is_default_constructible_v<T>, "Set type must be default constructible.");
			static_assert(std::is_default_constructible_v<Item>, "Key type must be default constructible.");

			T items;

			for (const auto& var : vars)
			{
				E extracted;
				if (!parser.Assign(extracted, *var.second))
					return false;

				Item item;
				if (!detail::ConvertIf<E, Item, Converter>::Apply(fnConvert, item, extracted))
					return false;

				items.emplace(std::move(item));
			}

			to = std::move(items);

			return true;
		}

		template<class Converter = DefaultConverter>
		static bool DoExtract(T& to, Converter fnConvert, const Selector::MatchList& vars, MapTag)
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

				if (!detail::ConvertIf<decltype(var.first), Key, Converter>::Apply(fnConvert, key, var.first))
					return false;

				Item item;

				if (!std::visit(detail::ConversionVisitor<Item, Converter>(item, fnConvert), var.second->Data))
					return false;

				items.emplace(std::move(key), std::move(item));
			}

			to = std::move(items);

			return true;
		}

		template<class E, class Converter = DefaultConverter>
		static bool DoExtract(T& to, const Parser<E>& parser, Converter fnConvert, const Selector::MatchList& vars, MapTag)
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

				if (!detail::ConvertIf<decltype(var.first), Key, Converter>::Apply(fnConvert, key, var.first))
					return false;

				E extracted;
				if (!parser.Assign(extracted, *var.second))
					return false;

				Item item;
				if (!detail::ConvertIf<E, Item, Converter>::Apply(fnConvert, item, extracted))
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

			if (T extracted; DoExtract(extracted, fnConvert, matches, MakeAssignTag()))
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

			if (T extracted; DoExtract(extracted, parser, fnConvert, matches, MakeAssignTag()))
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
		return ExtractHelper<T>::Extract<Converter, ResultPolicy<T>>(
			selector, fnConvert, ResultPolicy<T>{ std::forward<Args>(resultPolicyArgs)... }, m_Scope
		);
	}

	template<class T, template<class> class ResultPolicy = ThrowResult,
		class Converter = DefaultConverter, class E, class... Args>
	auto Extract(const Selector& selector, const Parser<E>& parser, 
				 Converter fnConvert = Converter(), Args&&... resultPolicyArgs)
	{
		return ExtractHelper<T>::Extract<E, Converter, ResultPolicy<T>>
		(
			selector, parser, fnConvert, 
			ResultPolicy<T>{ std::forward<Args>(resultPolicyArgs)... }, 
			m_Scope
		);
	}

	template<class T, template<class> class ResultPolicy = GuaranteedResult, 
			 class Converter = DefaultConverter>
	auto ExtractOr(const Selector& selector, T&& defaultVal, Converter fnConvert = Converter())
	{
		return ExtractHelper<T>::Extract<Converter, ResultPolicy<T>>(
			selector, fnConvert, ResultPolicy<T>{ std::forward<T>(defaultVal) }, m_Scope
		);
	}

	template<class T, template<class> class ResultPolicy = GuaranteedResult,
			 class E, class Converter = DefaultConverter>
	auto ExtractOr(const Selector& selector, const Parser<E>& parser, 
				   E&& defaultVal, Converter fnConvert = Converter())
	{
		return ExtractHelper<T>::Extract<E, Converter, ResultPolicy<T>>(
			selector, parser, fnConvert, ResultPolicy<T>{ std::forward<E>(defaultVal) }, m_Scope
		);
	}

	template<class T, class Converter = DefaultConverter>
	auto TryExtract(const Selector& selector, Converter fnConvert = Converter())
	{
		return ExtractHelper<T>::Extract<Converter, OptionalResult<T>>(
			selector, fnConvert, OptionalResult<T>(), m_Scope
		);
	}

	template<class T, class Converter = DefaultConverter, class E>
	auto TryExtract(const Selector& selector, const Parser<E>& parser, Converter fnConvert = Converter())
	{
		return ExtractHelper<T>::Extract<E, Converter, OptionalResult<T>>(
			selector, parser, fnConvert, OptionalResult<T>(), m_Scope
		);
	}
};