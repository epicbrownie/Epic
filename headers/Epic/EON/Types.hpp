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

#include <Epic/StringHash.hpp>
#include <Epic/STL/String.hpp>
#include <Epic/STL/Vector.hpp>
#include <variant>

//////////////////////////////////////////////////////////////////////////////

namespace Epic::EON
{
	struct EONArray;
	struct EONBoolean;
	struct EONFloat;
	struct EONInteger;
	struct EONObject;
	struct EONString;
	struct EONVariable;
	struct EONVariant;

	using EONName = STLString<char>;
	using EONNameHash = Epic::StringHash;
	using EONArrayMemberList = STLVector<EONVariant>;
	using EONObjectMemberList = STLVector<EONVariable>;
	using EONStringValueType = STLString<wchar_t>;
	using EONVariantValueType = std::variant<EONInteger, EONFloat, EONBoolean, EONString, EONArray, EONObject>;

	enum class eEONVariantType : std::size_t
	{ 
		Any = 0,
		Integer	= (1 << 0), 
		Float	= (1 << 1), 
		Boolean	= (1 << 2), 
		String	= (1 << 3), 
		Array	= (1 << 4), 
		Object	= (1 << 5)
	};
}

//////////////////////////////////////////////////////////////////////////////

struct Epic::EON::EONArray
{
	EONArrayMemberList Members;
};

struct Epic::EON::EONBoolean
{
	using ValueType = bool;

	ValueType Value;

	operator ValueType& () { return Value; }
	operator const ValueType& () const { return Value; }

};

struct Epic::EON::EONFloat
{
	using ValueType = double;

	ValueType Value;

	operator ValueType& () { return Value; }
	operator const ValueType& () const { return Value; }
};

struct Epic::EON::EONInteger
{
	using ValueType = long;

	ValueType Value;

	operator ValueType& () { return Value; }
	operator const ValueType& () const { return Value; }
};

struct Epic::EON::EONObject
{
	EONObjectMemberList Members;
};

struct Epic::EON::EONString
{
	using ValueType = EONStringValueType;

	ValueType Value;

	operator ValueType& () { return Value; }
	operator const ValueType& () const { return Value; }
};

//////////////////////////////////////////////////////////////////////////////

struct Epic::EON::EONVariant
{
	using ValueType = EONVariantValueType;

	ValueType Data;

	operator ValueType& () { return Data; }
	operator const ValueType& () const { return Data; }
};

struct Epic::EON::EONVariable
{
	EONName Name;
	EONVariant Value;
	EONName Parent;
	EONNameHash NameHash;
};
