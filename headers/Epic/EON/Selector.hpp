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
#include <Epic/EON/detail/Utility.hpp>
#include <Epic/STL/Vector.hpp>

//////////////////////////////////////////////////////////////////////////////

namespace Epic::EON
{
	struct HasName;
	struct HasParent;
}

//////////////////////////////////////////////////////////////////////////////

struct Epic::EON::HasName
{
	Epic::EON::Name _Name;

	explicit HasName(const Epic::EON::Name& name)
		: _Name{ name } { }

	inline Epic::STLVector<const Epic::EON::Variable*> operator() (const Epic::EON::Object& scope) const
	{
		const Epic::EON::Variable* pVar = detail::GetVariableInObject(scope, _Name);
		if (pVar)
			return{ pVar };
		else
			return{ };
	}
};

struct Epic::EON::HasParent
{
	Epic::EON::Name _Parent;
	Epic::EON::Name _Scope;

	explicit HasParent(const Epic::EON::Name& parent, const Epic::EON::Name& scope = "")
		: _Parent{ parent }, _Scope{ scope } { }

	Epic::STLVector<const Epic::EON::Variable*> operator() (const Epic::EON::Object& initScope) const
	{
		Epic::STLVector<const Epic::EON::Variable*> results;

		// Refine scope
		const Epic::EON::Object* pScope = &initScope;

		if (!_Scope.empty())
		{
			const Epic::EON::Variable* pScopeVar = detail::GetVariableInObject(initScope, _Scope);
			if (!pScopeVar)
				return results;

			const Epic::EON::Object* pScopeObj = boost::get<Epic::EON::Object>(&pScopeVar->Value.Data);
			if (!pScopeObj)
				return results;

			pScope = pScopeObj;
		}

		// Find all variables within the scope that inherit _Parent
		for (auto& v : pScope->Members)
			if (v.Parent == _Parent)
				results.emplace_back(&v);

		return results;
	}
};
