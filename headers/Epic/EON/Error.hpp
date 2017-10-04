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

#include <stdexcept>

//////////////////////////////////////////////////////////////////////////////

namespace Epic::EON
{
	struct InvalidIdentifierException : public std::runtime_error
	{
		using std::runtime_error::runtime_error;
	};

	struct InvalidValueException : public std::runtime_error
	{
		using std::runtime_error::runtime_error;
	};

	struct SelectionEmptyException : public std::runtime_error
	{
		using std::runtime_error::runtime_error;
	};

	struct ExtractionFailedExeception : public std::runtime_error
	{
		using std::runtime_error::runtime_error;
	};
}
