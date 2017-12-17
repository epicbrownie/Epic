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

#include <Epic/EON/Types.hpp>
#include <Epic/EON/detail/Utility.hpp>
#include <Epic/EON/detail/Visitors.hpp>
#include <Epic/STL/Vector.hpp>
#include <cctype>
#include <string_view>

//////////////////////////////////////////////////////////////////////////////

namespace Epic::EON
{
	class Selector;
}

//////////////////////////////////////////////////////////////////////////////

namespace Epic::EON
{
	enum class eScope { Matched, Default = Matched };
	enum class eMatch { First, Last, All, Default = First };

	namespace detail
	{
		using EONStringView = std::basic_string_view<EONName::value_type, EONName::traits_type>;

		// Global
		constexpr EONStringView::value_type GlobalRequired 
			= EONStringView::traits_type::to_char_type('!');	// The path is required (default)
		constexpr EONStringView::value_type GlobalOptional 
			= EONStringView::traits_type::to_char_type('#');	// The path is optional

		// Match
		constexpr EONStringView::value_type MatchFirst 
			= EONStringView::traits_type::to_char_type('+');	// Match first child of the current scope (default)
		constexpr EONStringView::value_type MatchLast 
			= EONStringView::traits_type::to_char_type('-');	// Match last child of the current scope
		constexpr EONStringView::value_type MatchAll 
			= EONStringView::traits_type::to_char_type('*');	// Match all children of the current scope
		
		// Filters
		constexpr EONStringView::value_type FilterDivider 
			= EONStringView::traits_type::to_char_type('|');

		constexpr EONStringView::value_type FilterTypeB 
			= EONStringView::traits_type::to_char_type('[');	// Filter the type of match(es)
		constexpr EONStringView::value_type FilterTypeE 
			= EONStringView::traits_type::to_char_type(']');
		constexpr EONStringView::value_type FilterParentB 
			= EONStringView::traits_type::to_char_type('<');	// Filter the parent of match(es)
		constexpr EONStringView::value_type FilterParentE 
			= EONStringView::traits_type::to_char_type('>');
		constexpr EONStringView::value_type FilterChain 
			= EONStringView::traits_type::to_char_type('^');	// Follow the inheritance chain

		// Scope
		constexpr EONStringView::value_type ScopeMatched 
			= EONStringView::traits_type::to_char_type('.');	// Set scope to matched (default)
	}
}

//////////////////////////////////////////////////////////////////////////////

class Epic::EON::Selector
{
public:
	using Type = Epic::EON::Selector;

private:
	struct Segment
	{
		std::string_view Selector;
		eScope Scope;
		eMatch Match;
		eEONVariantType TypeFilter;
		EONName ParentFilter;
		EONNameHash Identifier;
	};

private:
	using SegmentList = STLVector<Segment>;
	using SegmentIterator = SegmentList::const_iterator;
	using StringView = detail::EONStringView;

public:
	using Match = std::pair<EONName, const EONVariant*>;
	using MatchList = STLVector<Match>;

private:
	bool m_PathIsOptional = false;
	SegmentList m_Segments;
	StringView m_TotalPath;

public:
	Selector() noexcept = default;
	Selector(const Type&) = default;
	Selector(Type&&) = default;

	template<size_t N>
	Selector(const StringView::value_type(&path)[N]) noexcept 
		: Selector(StringView{ path, N })
	{ }

	Selector(const StringView::value_type* path) noexcept
		: Selector(StringView{ path })
	{ }
	
	Selector(StringView path) noexcept
		: m_TotalPath{ path }
	{
		// Global
		if (!path.empty())
			{
				if (path.front() == detail::GlobalRequired)
				{
					m_PathIsOptional = false;
					path.remove_prefix(1);
				}

				if (path.front() == detail::GlobalOptional)
				{
					m_PathIsOptional = true;
					path.remove_prefix(1);
				}
			}

		// Segments
		while (!path.empty())
		{
			bool isSet = false;
			Segment segment{ path, eScope::Default, eMatch::Default, eEONVariantType::Any, EONName(), EONNameHash() };

			// Matches
			switch (path.front())
			{
			case detail::MatchAll:
				segment.Match = eMatch::All;
				path.remove_prefix(1);
				isSet = true;
				break;

			case detail::MatchFirst:
				segment.Match = eMatch::First;
				path.remove_prefix(1);
				isSet = true;
				break;

			case detail::MatchLast:
				segment.Match = eMatch::Last;
				path.remove_prefix(1);
				isSet = true;
				break;

			default /* Identifier */:
			{
				size_t ide = 0;
				while (ide < path.size())
				{
					const auto c = path.at(ide);
					if (!std::isalnum(c) && c != StringView::traits_type::to_char_type('_'))
						break;

					++ide;
				}

				if (ide != 0)
				{
					segment.Identifier = path.substr(0, ide);
					path.remove_prefix(ide);
					isSet = true;
				}
			}
			break;
			}

			// Filters
			if (!path.empty())
			{
				do
				{
					if (path.front() == detail::FilterTypeB)
					{
						auto filter = path.find(detail::FilterTypeE, 1);
						if (filter != StringView::npos)
						{
							auto typeFilters = path.substr(1, filter - 1);
							size_t result = 0;

							while (!typeFilters.empty())
							{
								EONName typeFilter{ typeFilters };

								if (auto i = typeFilters.find(detail::FilterDivider); i != StringView::npos)
								{
									typeFilter = typeFilters.substr(0, i);
									typeFilters.remove_prefix(i + 1);
								}
								else
									typeFilters.remove_suffix(typeFilters.length());

								if (typeFilter == "INTEGER")
									result |= (size_t)eEONVariantType::Integer;
								else if (typeFilter == "FLOAT")
									result |= (size_t)eEONVariantType::Float;
								else if (typeFilter == "BOOLEAN")
									result |= (size_t)eEONVariantType::Boolean;
								else if (typeFilter == "STRING")
									result |= (size_t)eEONVariantType::String;
								else if (typeFilter == "ARRAY")
									result |= (size_t)eEONVariantType::Array;
								else if (typeFilter == "OBJECT")
									result |= (size_t)eEONVariantType::Object;
							}

							segment.TypeFilter = (eEONVariantType)result;
							isSet = true;
						}
						else
							filter = path.size() - 1;

						path.remove_prefix(filter + 1);
					}
					else if (path.front() == detail::FilterParentB)
					{
						auto filter = path.find(detail::FilterParentE, 1);
						if (filter != StringView::npos)
						{
							auto parentFilter = path.substr(1, filter - 1);
							if (!parentFilter.empty())
								segment.ParentFilter = parentFilter;

							isSet = true;
						}
						else
							filter = path.size() - 1;

						path.remove_prefix(filter + 1);
					}
					else
						break;
				} while (!path.empty());
			}

			// Scopes
			if (!path.empty())
			{
				if (path.front() == detail::ScopeMatched)
				{
					segment.Scope = eScope::Matched;
					path.remove_prefix(1);
					isSet = true;
				}
			}

			segment.Selector.remove_suffix(path.size());

			m_Segments.emplace_back(std::move(segment));

			if (!isSet)
				break;
		}
	}

public:
	bool IsOptional() const noexcept
	{
		return m_PathIsOptional;
	}

	auto Path() const noexcept
	{
		return m_TotalPath;
	}

	auto begin() const -> decltype(std::cbegin(m_Segments))
	{
		return std::cbegin(m_Segments);
	}

	auto end() const -> decltype(std::cend(m_Segments))
	{
		return std::cend(m_Segments);
	}

private:
	bool IsSegmentMatch(const Segment& segment, const EONVariable& v, const EONObject* pGlobalScope) const
	{
		constexpr auto BlankIdentifier = EONNameHash();
		
		if (segment.Identifier != BlankIdentifier && segment.Identifier != v.NameHash)
			return false;

		if (!segment.ParentFilter.empty())
		{
			// Follow inheritance chain if ParentFilter's first character is '^'
			if (pGlobalScope && segment.ParentFilter[0] == detail::FilterChain)
			{
				StringView parentFilter = segment.ParentFilter;
				parentFilter.remove_prefix(1);

				bool found = false;
				auto trace = detail::TraceInheritance(v, *pGlobalScope);

				while(!trace.empty())
				{
					if (trace.top() == parentFilter)
					{
						found = true;
						break;
					}

					trace.pop();
				}
				
				if (!found)
					return false;
			}
			else if (v.Parent != segment.ParentFilter)
				return false;
		}

		if (segment.TypeFilter != eEONVariantType::Any &&
			!std::visit(detail::FilterVisitor(segment.TypeFilter), v.Value.Data))
			return false;

		return true;
	}

	bool IsSegmentMatch(const Segment& segment, const EONVariant& v) const
	{
		constexpr auto BlankIdentifier = EONNameHash();

		if (segment.Identifier != BlankIdentifier)
			return false;

		if (!segment.ParentFilter.empty())
			return false;

		if (segment.TypeFilter != eEONVariantType::Any &&
			!std::visit(detail::FilterVisitor(segment.TypeFilter), v.Data))
			return false;

		return true;
	}

	void EvaluateSegment(const Segment& segment, MatchList& matches, const Match& scope, const EONObject* pGlobalScope) const
	{
		if (!scope.second)
			return;

		// Match on an EONObject scope type
		if (auto pScopeObject = std::get_if<EONObject>(&scope.second->Data); pScopeObject)
		{
			if (pScopeObject->Members.empty())
				return;

			switch (segment.Match)
			{
				default:
				case eMatch::First:
					for (size_t i = 0; i < pScopeObject->Members.size(); ++i)
					{
						if (IsSegmentMatch(segment, pScopeObject->Members[i], pGlobalScope))
						{
							matches.emplace_back(pScopeObject->Members[i].Name, &pScopeObject->Members[i].Value);
							break;
						}
					}
					break;

				case eMatch::Last:
					for (size_t i = pScopeObject->Members.size() - 1; i >= 0; --i)
					{
						if (IsSegmentMatch(segment, pScopeObject->Members[i], pGlobalScope))
						{
							matches.emplace_back(pScopeObject->Members[i].Name, &pScopeObject->Members[i].Value);
							break;
						}
					}
					break;

				case eMatch::All:
					for (size_t i = 0; i < pScopeObject->Members.size(); ++i)
						if (IsSegmentMatch(segment, pScopeObject->Members[i], pGlobalScope))
							matches.emplace_back(pScopeObject->Members[i].Name, &pScopeObject->Members[i].Value);
					break;
			}
		}

		// Match on an EONArray scope type
		else if (auto pScopeArray = std::get_if<EONArray>(&scope.second->Data); pScopeArray)
		{
			if (pScopeArray->Members.empty())
				return;

			switch (segment.Match)
			{
				default:
				case eMatch::First:
					for (size_t i = 0; i < pScopeArray->Members.size(); ++i)
					{
						if (IsSegmentMatch(segment, pScopeArray->Members[i]))
						{
							matches.emplace_back("", &pScopeArray->Members[i]);
							break;
						}
					}
					break;

				case eMatch::Last:
					for (size_t i = pScopeArray->Members.size() - 1; i >= 0; --i)
					{
						if (IsSegmentMatch(segment, pScopeArray->Members[i]))
						{
							matches.emplace_back("", &pScopeArray->Members[i]);
							break;
						}
					}
					break;

				case eMatch::All:
					for (size_t i = 0; i < pScopeArray->Members.size(); ++i)
						if (IsSegmentMatch(segment, pScopeArray->Members[i]))
							matches.emplace_back("", &pScopeArray->Members[i]);
					break;
			}
		}

		// Match on another type
		else if (IsSegmentMatch(segment, *scope.second))
			matches.emplace_back(scope.first, scope.second);
	}

	void EvaluateSegment(const Segment& segment, MatchList& matches, const EONObject& rootScope) const
	{
		if (rootScope.Members.empty())
			return;

		switch (segment.Match)
		{
			default:
			case eMatch::First:
				for (size_t i = 0; i < rootScope.Members.size(); ++i)
				{
					if (IsSegmentMatch(segment, rootScope.Members[i], &rootScope))
					{
						matches.emplace_back(rootScope.Members[i].Name, &rootScope.Members[i].Value);
						break;
					}
				}
				break;

			case eMatch::Last:
				for (size_t i = rootScope.Members.size() - 1; i >= 0; --i)
				{
					if (IsSegmentMatch(segment, rootScope.Members[i], &rootScope))
					{
						matches.emplace_back(rootScope.Members[i].Name, &rootScope.Members[i].Value);
						break;
					}
				}
				break;

			case eMatch::All:
				for (size_t i = 0; i < rootScope.Members.size(); ++i)
					if (IsSegmentMatch(segment, rootScope.Members[i], &rootScope))
						matches.emplace_back(rootScope.Members[i].Name, &rootScope.Members[i].Value);
				break;
		}
	}

public:
	MatchList Evaluate(const EONVariant* const pScope) const
	{
		if (!pScope)
			return MatchList();

		MatchList matches({ { "", pScope } });
		auto pScopeAsObject = std::get_if<EONObject>(&pScope->Data);

		for (auto& segment : m_Segments)
		{
			MatchList segMatches;

			for (const auto& match : matches)
				EvaluateSegment(segment, segMatches, match, pScopeAsObject);
			
			if (segMatches.empty())
				return {};

			switch (segment.Scope)
			{
				default:
				case eScope::Matched:
					matches.swap(segMatches);
					break;
			}
		}

		return matches;
	}

	MatchList Evaluate(const EONObject& rootScope) const
	{
		MatchList matches;

		if (std::empty(m_Segments))
			return matches;

		auto& firstSegment = m_Segments[0];
		EvaluateSegment(firstSegment, matches, rootScope);

		if (std::empty(matches))
			return matches;

		for (size_t i = 1; i < std::size(m_Segments); ++i)
		{
			auto& segment = m_Segments[i];

			MatchList segMatches;

			for (const auto& match : matches)
				EvaluateSegment(segment, segMatches, match, &rootScope);

			if (segMatches.empty())
				return {};

			switch (segment.Scope)
			{
				default:
				case eScope::Matched:
					matches.swap(segMatches);
					break;
			}
		}

		return matches;
	}
};
