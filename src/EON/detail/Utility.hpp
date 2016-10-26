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
		const Epic::EON::Variable* GetVariableInObject(const Epic::EON::Object& scope, const Epic::EON::NameHash& namehash);
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
				if (!vname.empty())
				{
					const Epic::EON::NameHash vnamehash{ vname };

					// Binary search for the variable
					auto it = std::lower_bound
					(
						std::begin(pScope->Members),
						std::end(pScope->Members),
						vnamehash,
						[&] (const auto& v, const auto& vh) { return v.NameHash < vh; }
					);

					// Assert that the hash value is unique
					assert(it == std::end(pScope->Members) || (*it).NameHash != vnamehash || (*it).Name == vname);

					pVariable = (it == std::end(pScope->Members) || (*it).NameHash != vnamehash) ? nullptr : &(*it);
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
			// Binary search for the variable
			auto it = std::lower_bound
			(
				std::begin(scope.Members),
				std::end(scope.Members),
				namehash,
				[&] (const auto& v, const auto& vh) { return v.NameHash < vh; }
			);

			return (it == std::end(scope.Members) || (*it).NameHash != namehash) ? nullptr : &(*it);
		}
	}
}
