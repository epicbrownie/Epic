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
	struct HasPath;
	struct HasName;
	struct HasParent;
}

//////////////////////////////////////////////////////////////////////////////

struct Epic::EON::HasPath
{
	Epic::EON::Name _Path;

	explicit HasPath(const Epic::EON::Name& path)
		: _Path{ path } { }

	inline const Epic::EON::Variable* operator() (const Epic::EON::Object& scope) const
	{
		return detail::GetVariableInObject(scope, _Path);
	}
};

struct Epic::EON::HasName
{
	Epic::EON::NameHash _Hash;

	explicit HasName(const Epic::EON::NameHash& hash)
		: _Hash{ hash } { }

	inline const Epic::EON::Variable* operator() (const Epic::EON::Object& scope) const
	{
		return detail::GetVariableInObject(scope, _Hash);
	}
};

struct Epic::EON::HasParent
{
	Epic::EON::Name _Parent;

	explicit HasParent(const Epic::EON::Name& parent)
		: _Parent{ parent } { }

	Epic::STLVector<const Epic::EON::Variable*> operator() (const Epic::EON::Object& scope) const
	{
		Epic::STLVector<const Epic::EON::Variable*> results;

		// Find all variables within the scope that inherit _Parent
		for (auto& v : scope.Members)
			if (v.Parent == _Parent)
				results.emplace_back(&v);

		return results;
	}
};
