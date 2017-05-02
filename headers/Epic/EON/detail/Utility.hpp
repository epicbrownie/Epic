//////////////////////////////////////////////////////////////////////////////
//
//            Copyright (c) 2016 Ronnie Brohn (EpicBrownie)      
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
#include <Epic/TMP/TypeTraits.hpp>
#include <boost/variant/get.hpp>
#include <algorithm>

//////////////////////////////////////////////////////////////////////////////

namespace Epic::EON::detail
{
	namespace
	{
		const Epic::EON::Variable* GetVariableInObject(const Epic::EON::Object& scope, const Epic::EON::Name& name);
		const Epic::EON::Variable* GetVariableInObject(const Epic::EON::Object& scope, const Epic::EON::NameHash& namehash);
	}

	// ExtentFilterIf<Function, T>
	//		Attempts to invoke user-supplied default extent filter for T.
	//		The user-supplied extent filter function should have the signature: bool (const T&)
	//		If the user-supplied extent filter function cannot accept the argument, false is returned.
	template<class Function, class T, bool Enabled = Epic::TMP::IsCallable<Function(const T&), bool>::value>
	struct ExtentFilterIf;

	// ExtentVisitor<FilterFn>
	//		Visits an EON::Variant variant and counts the extent of the type.
	//		Returns the number of members that satisfy the unary predicate function FilterFn.
	template<class FilterFn>
	struct ExtentVisitor;
}

//////////////////////////////////////////////////////////////////////////////

namespace Epic::EON::detail
{
	namespace
	{
		const Epic::EON::Variable* GetVariableInObject(const Epic::EON::Object& scope, const Epic::EON::Name& name)
		{
			// Resolve 'name' as a qualified path
			const Epic::EON::Variable* pVariable = nullptr;
			const Epic::EON::Object* pScope = &scope;

			auto itKey = std::begin(name);
			const auto itEnd = std::end(name);

			while (itKey != itEnd)
			{
				// Ensure the current search scope is valid
				if (!pScope)
					break;

				// Find the next path entry
				auto itNext = itKey;
				for (; itNext != itEnd; ++itNext)
					if (*itNext == '.') break;

				// Find the variant for this path entry
				const Epic::EON::Name vname{ itKey, itNext };
				if (!vname.empty())
				{
					const Epic::EON::NameHash vnamehash{ vname };

					// Search for the variable
					auto it = std::find_if
					(
						std::begin(pScope->Members),
						std::end(pScope->Members),
						[&] (const auto& v) { return v.NameHash == vnamehash; }
					);

					pVariable = (it == std::end(pScope->Members)) ? nullptr : &(*it);
					if (!pVariable) break;

					pScope = boost::get<Epic::EON::Object>(&pVariable->Value.Data);
				}

				// Reset for next path entry
				itKey = (itNext == itEnd) ? itEnd : std::next(itNext);
			}

			return pVariable;
		}

		const Epic::EON::Variable* GetVariableInObject(const Epic::EON::Object& scope, const Epic::EON::NameHash& namehash)
		{
			// Search for the variable
			auto it = std::find_if
			(
				std::begin(scope.Members),
				std::end(scope.Members),
				[&] (const auto& v) { return v.NameHash == namehash; }
			);

			return (it == std::end(scope.Members)) ? nullptr : &(*it);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////

// ExtentFilterIf<false>
template<class Function, class T, bool Enabled>
struct Epic::EON::detail::ExtentFilterIf
{
	static inline bool Apply(Function, const T&)
	{
		// User-supplied default assignment function has no function with signature:
		// bool (const T&)
		// Default to return false

		return false;
	}
};

// ExtentFilterIf<true>
template<class Function, class T>
struct Epic::EON::detail::ExtentFilterIf<Function, T, true>
{
	static inline bool Apply(Function filterFn, const T& value)
	{
		// User-supplied extent filter function will work with this argument
		return filterFn(value);
	}
};

//////////////////////////////////////////////////////////////////////////////

// ExtentVisitor<FilterFn>
template<class FilterFn>
struct Epic::EON::detail::ExtentVisitor : public boost::static_visitor<size_t>
{
	const FilterFn _FilterFn;

	ExtentVisitor(const FilterFn& filterFn)
		: _FilterFn{ filterFn } { }

	template<class SrcType>
	inline size_t operator() (const SrcType& src) const
	{
		return ExtentFilterIf<FilterFn, SrcType>::Apply(_FilterFn, src) ? 1 : 0;
	}

	inline size_t operator() (const Epic::EON::Object& src) const
	{
		size_t n = 0;
		for (const auto& v : src.Members)
			if (ExtentFilterIf<FilterFn, EON::Variable>::Apply(_FilterFn, v))
				++n;
		return n;
	}

	inline size_t operator() (const Epic::EON::Array& src) const
	{
		size_t n = 0;
		for (const auto& v : src.Members)
			if (ExtentFilterIf<FilterFn, EON::Variant>::Apply(_FilterFn, v))
				++n;
		return n;
	}
};

// ExtentVisitor<void>
template<>
struct Epic::EON::detail::ExtentVisitor<void> : public boost::static_visitor<size_t>
{
	template<class SrcType>
	constexpr size_t operator() (const SrcType& src) const
	{
		return 1;
	}

	inline size_t operator() (const Epic::EON::Object& src) const
	{
		return src.Members.size();
	}

	inline size_t operator() (const Epic::EON::Array& src) const
	{
		return src.Members.size();
	}
};
