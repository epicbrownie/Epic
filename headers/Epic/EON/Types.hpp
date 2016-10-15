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

#include <Epic/STL/Vector.hpp>
#include <Epic/STL/String.hpp>
#include <boost/variant.hpp>

//////////////////////////////////////////////////////////////////////////////

namespace Epic::EON
{
	struct Variable;
	struct Variant;

	struct Object;
	struct Array;
	struct String;
	struct Integer;
	struct Float;
	struct Boolean;

	using Name = Epic::STLString<char>;
}

//////////////////////////////////////////////////////////////////////////////

// Object
struct Epic::EON::Object
{
	Epic::STLVector<EON::Variable> Members;
};

//////////////////////////////////////////////////////////////////////////////

// Array
struct Epic::EON::Array
{
	Epic::STLVector<EON::Variant> Members;
};

//////////////////////////////////////////////////////////////////////////////

// String
struct Epic::EON::String
{
	using ValueType = Epic::STLString<wchar_t>;
	
	ValueType Value;

	operator ValueType() const
	{
		return Value;
	}
};

//////////////////////////////////////////////////////////////////////////////

// Integer
struct Epic::EON::Integer
{
	using ValueType = std::int64_t;
	
	ValueType Value;

	operator ValueType() const
	{
		return Value;
	}
};

//////////////////////////////////////////////////////////////////////////////

// Float
struct Epic::EON::Float
{
	using ValueType = std::float_t;
	
	ValueType Value;

	operator ValueType() const
	{
		return Value;
	}
};

//////////////////////////////////////////////////////////////////////////////

// Boolean
struct Epic::EON::Boolean
{
	using ValueType = bool;
	
	ValueType Value;

	operator ValueType() const
	{
		return Value;
	}
};

//////////////////////////////////////////////////////////////////////////////

// Variant
struct Epic::EON::Variant
{
	using VariantType = boost::variant<EON::Object, EON::Array, EON::String, EON::Float, EON::Integer, EON::Boolean>;
	using Types = typename VariantType::types;

	VariantType Data;
};

//////////////////////////////////////////////////////////////////////////////

// Variable
struct Epic::EON::Variable
{
	EON::Name Name;
	EON::Name Parent;
	EON::Variant Value;
};
