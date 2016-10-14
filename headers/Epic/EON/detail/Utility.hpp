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
#include <boost/variant/get.hpp>
#include <algorithm>

//////////////////////////////////////////////////////////////////////////////

namespace Epic::EON::detail
{
	namespace
	{
		const Epic::EON::Variable* GetVariableInObject(const Epic::EON::Object& scope, const Epic::EON::Name& name);
	}
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
				if (vname.empty()) break;

				auto it = std::find_if
				(
					std::begin(pScope->Members),
					std::end(pScope->Members),
					[&] (const auto& v) { return v.Name == vname; }
				);

				pVariable = (it == std::end(pScope->Members)) ? nullptr : &(*it);
				if (!pVariable) break;

				pScope = boost::get<Epic::EON::Object>(&pVariable->Value.Data);

				// Reset for next path entry
				itKey = (itNext == itEnd) ? itEnd : std::next(itNext);
			}

			return pVariable;
		}
	}
}
