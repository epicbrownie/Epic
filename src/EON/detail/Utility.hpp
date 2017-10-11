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

#include <Epic/EON/Error.hpp>
#include <Epic/EON/Types.hpp>
#include <Epic/EON/detail/Visitors.hpp>
#include <Epic/STL/Stack.hpp>
#include <cassert>
#include <cstdint>
#include <stdexcept>
#include <tuple>

//////////////////////////////////////////////////////////////////////////////

namespace Epic::EON::detail
{
	namespace
	{
		const EONVariable* FindVariableInObject(const EONObject& scope, const EONName& name);
		std::tuple<bool, const EONVariable*, size_t> GetAttributes(const EONVariant* pVar, const EONObject& scope);
		std::pair<const EONObject*, const EONVariable*> FindParent(const EONObject& scope, const EONName& name);
		STLStack<EONName> TraceInheritance(const EONVariable& variable, const EONObject& scope);
		bool Tidy(const EONObject* pGlobal, EONObject* pScope, EONVariable& variable, bool resolveInheritance = true);
		void ResolveInheritance(EONVariable& variable, const EONObject* pGlobal);
	}
}

//////////////////////////////////////////////////////////////////////////////

// Helpers
namespace Epic::EON::detail
{
	namespace
	{
		const EONVariable* FindVariableInObject(const EONObject& scope, const EONName& name)
		{
			// Resolve 'name' as a qualified path
			const EONVariable* pVariable = nullptr;
			const EONObject* pScope = &scope;

			auto itKey = std::begin(name);
			const auto itEnd = std::end(name);

			while (itKey != itEnd)
			{
				// Ensure the current search scope is valid
				if (!pScope) break;

				// Find the next path entry
				auto itNext = itKey;
				for (; itNext != itEnd; ++itNext)
					if (*itNext == '.') break;

				// Find the variant for this path entry
				const EONName vname{ itKey, itNext };
				if (!vname.empty())
				{
					const EONNameHash vnamehash{ vname };

					// Search for the variable
					auto it = std::find_if
					(
						std::begin(pScope->Members),
						std::end(pScope->Members),
						[&] (const auto& v) { return v.NameHash == vnamehash; }
					);

					pVariable = (it == std::end(pScope->Members)) ? nullptr : &(*it);
					if (!pVariable) break;

					pScope = std::get_if<EONObject>(&pVariable->Value.Data);
				}

				// Reset for next path entry
				itKey = (itNext == itEnd) ? itEnd : std::next(itNext);
			}

			return pVariable;
		}

		std::tuple<bool, const EONVariable*, size_t> GetAttributes(const EONVariant* pVar, const EONObject& scope)
		{
			for (size_t i = 0; i < scope.Members.size(); ++i)
			{
				auto& v = scope.Members[i];

				if (&v.Value == pVar)
					return { true, &v, i };

				if (auto pAsObject = std::get_if<EONObject>(&v.Value.Data); pAsObject)
				{
					if (auto attr = GetAttributes(pVar, *pAsObject); std::get<0>(attr))
						return attr;
				}
			}

			return { false, nullptr, 0 };
		}

		std::pair<const EONObject*, const EONVariable*> FindParent(const EONObject& scope, const EONName& name)
		{
			if (auto pV = FindVariableInObject(scope, name); pV != nullptr)
				return { &scope, pV };

			for (const auto& v : scope.Members)
			{
				if (auto pAsObject = std::get_if<EONObject>(&v.Value.Data); pAsObject)
				{
					if (auto pV = FindParent(*pAsObject, name); pV.second != nullptr)
						return pV;
				}
			}
			
			return { nullptr, nullptr };
		}

		STLStack<EONName> TraceInheritance(const EONVariable& variable, const EONObject& scope)
		{
			STLStack<EONName> results;

			auto pParent = &variable;
			auto pScope = &scope;

			while (pScope && pParent && !pParent->Parent.empty())
			{
				results.emplace(pParent->Parent);
				std::tie(pScope, pParent) = FindParent(*pScope, pParent->Parent);
			}

			return results;
		}

		bool Tidy(const EONObject* pGlobal, EONObject* pScope, EONVariable& variable, bool resolveInheritance)
		{
			// Resolve inheritance of the variable and tidy its members.
			// If 'variable' is a duplicate of an existing variable in 'pScope',
			// Tidy will merge its data into the existing member and return false.
			// Tidy returns true if no duplicate was found.

			assert(pGlobal);
			assert(pScope);

			// If the variable is an object, resolve the inheritance of its native members
			EONObject* pObject = std::get_if<EONObject>(&variable.Value.Data);
			
			if (pObject)
			{
				for (auto& member : pObject->Members)
					if (!member.Parent.empty())
						detail::ResolveInheritance(member, pGlobal);
			}

			// Now resolve the inheritance of the variable
			if (resolveInheritance && !variable.Parent.empty())
				detail::ResolveInheritance(variable, pGlobal);

			// Finally, if the variable is an object, tidy its members
			if (pObject)
			{
				for (size_t i = 0; i < pObject->Members.size(); )
				{
					if (!Tidy(pGlobal, pObject, pObject->Members[i], false))
						pObject->Members.erase(std::begin(pObject->Members) + i);
					else
						++i;
				}
			}

			// Search the scope for a duplicate of the variable
			auto itDup = std::find_if(std::begin(pScope->Members), std::end(pScope->Members),
				[&] (const EONVariable& v) { return v.NameHash == variable.NameHash; });

			if (itDup != std::end(pScope->Members) && &(*itDup) != &variable)
			{
				MergeVisitor vsMerge((*itDup).Value, variable.Value);
				std::visit(vsMerge, (*itDup).Value.Data, variable.Value.Data);

				return false;
			}

			return true;
		}

		void ResolveInheritance(EONVariable& variable, const EONObject* pGlobal)
		{
			assert(!variable.Parent.empty());
			assert(pGlobal);

			const EONVariable* pParent = FindVariableInObject(*pGlobal, variable.Parent);
			if (!pParent)
			{
				std::string err = "Invalid inheritance specified for variable '";
				err += variable.Name.c_str();
				err += "'.  No variable was found matching the desired path.";

				throw InvalidIdentifierException{ err };
			}

			if (&variable != pParent)
			{
				// Now inherit into variable from pParent
				try
				{
					std::visit(InheritVisitor(), variable.Value.Data, pParent->Value.Data);
				}
				catch (InvalidValueException&)
				{
					std::string err = "Invalid inheritance specified for variable '";
					err += variable.Name.c_str();
					err += "'.  Types were incompatible.";

					throw InvalidValueException{ err };
				}
			}
		}
	}
}
