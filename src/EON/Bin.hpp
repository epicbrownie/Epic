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
#include <Epic/EON/Extractor.hpp>
#include <Epic/EON/detail/Utility.hpp>
#include <algorithm>
#include <cassert>

//////////////////////////////////////////////////////////////////////////////

namespace Epic::EON
{
	class Bin;
}

//////////////////////////////////////////////////////////////////////////////

// Bin
class Epic::EON::Bin
{
private:
	EONObject m_Data;

public:
	Bin() = default;
	Bin(const Bin&) = default;
	Bin(Bin&&) = default;

public:
	const EONObject& Root() const 
	{ 
		return m_Data; 
	}

public:
	Extractor CreateExtractor() const noexcept
	{
		return { m_Data };
	}

public:
	void Insert(const EONObject& obj)
	{
		for(auto& v : obj.Members)
			Insert(std::move(EONVariable{ v }));
	}

	void Insert(EONObject&& obj)
	{
		for (auto& v : obj.Members)
			Insert(std::move(v));
	}

	void Insert(const EONVariable& variable)
	{
		Insert(std::move(EONVariable{ variable }));
	}

	void Insert(EONVariable&& variable)
	{
		assert(variable.Name != "" && "Variable must have a name");

		if (detail::Tidy(&m_Data, &m_Data, variable, true))
			m_Data.Members.emplace_back(std::move(variable));
	}

public:
	Bin& operator << (const EONObject& obj)
	{
		Insert(obj);
		return *this;
	}

	Bin& operator << (EONObject&& obj)
	{
		Insert(std::move(obj));
		return *this;
	}

	Bin& operator << (const EONVariable& var)
	{
		Insert(var);
		return *this;
	}

	Bin& operator << (EONVariable&& var)
	{
		Insert(std::move(var));
		return *this;
	}
};
