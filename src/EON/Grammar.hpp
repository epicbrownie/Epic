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
#include <boost/spirit/include/qi.hpp>
#include <boost/phoenix/bind.hpp>
#include <locale>
#include <codecvt>

//////////////////////////////////////////////////////////////////////////////

namespace Epic::EON
{
	template<class Iterator>
	class Grammar;
}

//////////////////////////////////////////////////////////////////////////////

// Grammar
template<class Iterator>
class Epic::EON::Grammar : public boost::spirit::qi::grammar<Iterator, EONObject(), boost::spirit::qi::standard::space_type>
{
public:
	using Skipper = boost::spirit::qi::standard::space_type;

private:
	static void IdentifierToName(EONName& name, const EONString::ValueType& identifier)
	{
		name = identifier;
	}

	static void AddVariantToArray(EONArray& arr, const EONVariant& var)
	{
		arr.Members.emplace_back(var);
	}

	static void AddVariableToObject(EONObject& obj, const EONVariable& variable)
	{
		obj.Members.push_back(variable);
	}

	template<class T>
	static void SetVariantValue(T& var, const typename T::ValueType& value)
	{
		var.Value = value;
	}

	template<typename T>
	static void SetVariantData(EONVariant& var, const T& value)
	{
		var.Data = value;
	}

	static void SetVariableName(EONVariable& variable, const EONName& name)
	{
		variable.Name = name;
		variable.NameHash = name;
	}
	
	static void SetVariableValue(EONVariable& variable, const EONVariant& var)
	{
		variable.Value = var;
	}

	static void SetVariableParent(EONVariable& variable, const EONName& sourceName)
	{
		variable.Parent = sourceName;
	}

private:
	///////////////////////////////////////////////////////////////////////////////
	//  IntegerPolicies specify the following:
	//    - '.' does not parse at any location
	//    - 'nan' and 'inf' are not allowed
	//    - Digits are allowed to be comma-separated in groups of 3
	///////////////////////////////////////////////////////////////////////////////
	template<typename T>
	struct IntegerPolicies : boost::spirit::qi::real_policies<T>
	{
		static constexpr bool expect_dot = false;
		static constexpr bool allow_leading_dot = false;
		static constexpr bool allow_trailing_dot = false;

		// Try to parse as thousands-separated groups.
		// Let the default base policies parse it otherwise.
		template<typename Iterator, typename Attribute>
		static bool parse_n(Iterator& first, const Iterator& last, Attribute& attr)
		{
			namespace qi = boost::spirit::qi;

			qi::uint_parser<unsigned int, 10, 1, 3> LeadingParser;
			qi::uint_parser<unsigned int, 10, 3, 3> GroupParser;

			Iterator itFirst = first;
			T result = 0;
			bool success = false;

			// Try parsing as comma-separated groups of 3
			if (qi::parse(first, last, LeadingParser, result))
			{
				Iterator itMemento = first;
				unsigned int group;

				while (qi::parse(first, last, qi::lit(',')) && qi::parse(first, last, GroupParser, group))
				{
					result = (result * 1000) + group;
					itMemento = first;
					success = true;
				}

				first = itMemento;
			}

			// Try parsing it normally
			if (!success)
			{
				first = itFirst;
				return qi::strict_real_policies<T>::parse_n(first, last, attr);
			}

			boost::spirit::traits::assign_to(result, attr);

			return true;
		}

		// Always mark the dot as NOT parsed
		template<typename Iterator>
		static bool parse_dot(Iterator& /*first*/, const Iterator& /*last*/)
		{
			return false;
		}

		// Always mark the frac as parsed
		template<typename Iterator, typename Attribute>
		static bool parse_frac_n(Iterator& /*first*/, const Iterator& /*last*/, Attribute& /*attr*/, int& /*frac_digits*/)
		{
			return true;
		}

		// Always mark the exp prefix as NOT parsed
		template<typename Iterator>
		static bool parse_exp(Iterator& /*first*/, const Iterator& /*last*/)
		{
			return false;
		}

		// Always mark the exp as parsed
		template<typename Iterator, typename Attribute>
		static bool parse_exp_n(Iterator& /*first*/, const Iterator& /*last*/, Attribute& /*attr*/)
		{
			return true;
		}

		// Disallow 'NaN'
		template<typename Iterator, typename Attribute>
		static bool parse_nan(Iterator& /*first*/, const Iterator& /*last*/, Attribute& /*attr*/)
		{
			return false;
		}

		// Disallow 'INF'
		template<typename Iterator, typename Attribute>
		static bool parse_inf(Iterator& /*first*/, const Iterator& /*last*/, Attribute& /*attr*/)
		{
			return false;
		}
	};

	///////////////////////////////////////////////////////////////////////////////
	//  FloatPolicies specify the following:
	//    - '.' is required
	//	  - Trailing '.' and Leading '.' are both allowed
	//    - 'nan' and 'inf' are not allowed
	//    - Digits before the '.' are allowed to be comma-separated in groups of 3
	///////////////////////////////////////////////////////////////////////////////
	template<typename T>
	struct FloatPolicies : boost::spirit::qi::strict_real_policies<T>
	{
		static constexpr bool expect_dot = true;
		static constexpr bool allow_leading_dot = true;
		static constexpr bool allow_trailing_dot = true;

		// Try to parse as thousands-separated groups.
		// Let the default base policies parse it otherwise.
		template<typename Iterator, typename Attribute>
		static bool parse_n(Iterator& first, const Iterator& last, Attribute& attr)
		{
			namespace qi = boost::spirit::qi;

			qi::uint_parser<unsigned int, 10, 1, 3> LeadingParser;
			qi::uint_parser<unsigned int, 10, 3, 3> GroupParser;

			Iterator itFirst = first;
			unsigned int result = 0;
			bool success = false;

			// Try parsing as comma-separated groups of 3
			if (qi::parse(first, last, LeadingParser, result))
			{
				Iterator itMemento = first;
				unsigned int group;

				while (qi::parse(first, last, qi::lit(',')) && qi::parse(first, last, GroupParser, group))
				{
					result = (result * 1000) + group;
					itMemento = first;
					success = true;
				}

				first = itMemento;
			}

			// Try parsing it normally
			if (!success)
			{
				first = itFirst;
				return qi::strict_real_policies<T>::parse_n(first, last, attr);
			}

			boost::spirit::traits::assign_to(result, attr);

			return true;
		}

		// Disallow 'NaN'
		template<typename Iterator, typename Attribute>
		static bool parse_nan(Iterator& /*first*/, const Iterator& /*last*/, Attribute& /*attr*/)
		{
			return false;
		}

		// Disallow 'INF'
		template<typename Iterator, typename Attribute>
		static bool parse_inf(Iterator& /*first*/, const Iterator& /*last*/, Attribute& /*attr*/)
		{
			return false;
		}
	};

	///////////////////////////////////////////////////////////////////////////////
	//  BoolPolicies allow the following:
	//    - true: "true", "TRUE", "yes", "YES", "on", "ON"
	//	  - false: "false", "FALSE", "no", "NO", "off", "OFF"
	///////////////////////////////////////////////////////////////////////////////
	template<typename T = bool>
	struct BoolPolicies : boost::spirit::qi::bool_policies<T>
	{
		template<typename Iterator, typename Attribute>
		static bool parse_true(Iterator& first, const Iterator& last, Attribute& attr)
		{
			namespace qi = boost::spirit::qi;

			if (qi::detail::string_parse("TRUE", "true", first, last, qi::unused) ||
				qi::detail::string_parse("YES", "yes", first, last, qi::unused) ||
				qi::detail::string_parse("ON", "on", first, last, qi::unused))
			{
				boost::spirit::traits::assign_to(true, attr);
				return true;
			}

			return false;
		}

		template <typename Iterator, typename Attribute>
		static bool parse_false(Iterator& first, Iterator const & last, Attribute& attr)
		{
			namespace qi = boost::spirit::qi;

			if (qi::detail::string_parse("FALSE", "false", first, last, qi::unused) ||
				qi::detail::string_parse("NO", "no", first, last, qi::unused) ||
				qi::detail::string_parse("OFF", "off", first, last, qi::unused))
			{
				boost::spirit::traits::assign_to(false, attr);
				return true;
			}

			return false;
		}
	};

public:
	Grammar() : Grammar::base_type{ r_Root }
	{
		using boost::phoenix::bind;
		using boost::spirit::_val;
		using boost::spirit::_1;

		namespace qi = boost::spirit::qi;
		namespace ctype = qi::standard;
		namespace phx = boost::phoenix;
		
		// EON document
		r_Root = *(r_Comment | r_Variable[bind(Grammar::AddVariableToObject, _val, _1)] | r_Terminator);
		
		// Literals
		r_Terminator = qi::lit(',') | qi::lit(';');
		r_Separator = qi::lit(',');

		// Comment
		r_Comment = r_LineComment | r_BlockComment;
		r_LineComment = (qi::lit('#') | qi::lit("//")) >> *(ctype::char_ - qi::eol) >> -qi::eol;
		r_BlockComment = qi::lit("/*") >> *(ctype::char_ - qi::lit("*/")) >> -qi::lit("*/");

		// Variable (key/value)
		r_Variable = (r_PrimitiveVariable | r_ArrayVariable | r_ObjectVariable) >> *r_Terminator;
		
		r_PrimitiveVariable = r_VariableName[bind(Grammar::SetVariableName, _val, _1)]
						   >> -r_VariableInheritor[bind(Grammar::SetVariableParent, _val, _1)]
						   >> *r_Comment
						   >> r_VariableSeparator
						   >> *r_Comment
						   >> r_PrimitiveVariant[bind(Grammar::SetVariableValue, _val, _1)];
		r_ArrayVariable = r_VariableName[bind(Grammar::SetVariableName, _val, _1)]
					   >> -r_VariableInheritor[bind(Grammar::SetVariableParent, _val, _1)]
					   >> *r_Comment
					   >> -r_VariableSeparator
					   >> *r_Comment
					   >> r_ArrayVariant[bind(Grammar::SetVariableValue, _val, _1)];
		r_ObjectVariable = r_VariableName[bind(Grammar::SetVariableName, _val, _1)]
						>> -r_VariableInheritor[bind(Grammar::SetVariableParent, _val, _1)]
						>> *r_Comment
						>> -r_VariableSeparator
						>> *r_Comment
						>> r_ObjectVariant[bind(Grammar::SetVariableValue, _val, _1)];

		// Variable name (key)
		r_VariableName = r_VariableIdentifier[bind(Grammar::IdentifierToName, _val, _1)];
		r_VariableIdentifier = qi::lexeme[ctype::char_("A-Za-z_") >> *ctype::char_("A-Za-z0-9_")];

		// Separator between key and value
		r_VariableSeparator = qi::lit('=') | qi::lit(':');

		// Inheritance classifier
		r_VariableInheritor = qi::lit('<') >> r_VariablePath[bind(Grammar::IdentifierToName, _val, _1)] >> qi::lit('>');
		r_VariablePath = -r_VariableIdentifier >> *(ctype::char_('.') >> r_VariableIdentifier);

		// Variants (value)
		r_Variant = r_PrimitiveVariant | r_ArrayVariant | r_ObjectVariant;

		r_PrimitiveVariant = r_String[bind(Grammar::SetVariantData<EONString>, _val, _1)]
						   | r_Float[bind(Grammar::SetVariantData<EONFloat>, _val, _1)]
						   | r_Integer[bind(Grammar::SetVariantData<EONInteger>, _val, _1)]
						   | r_Boolean[bind(Grammar::SetVariantData<EONBoolean>, _val, _1)];
		r_ArrayVariant = r_Array[bind(Grammar::SetVariantData<EONArray>, _val, _1)];
		r_ObjectVariant = r_Object[bind(Grammar::SetVariantData<EONObject>, _val, _1)];
		
		// Variant types
		r_Float = r_CustomFloat[bind(Grammar::SetVariantValue<EONFloat>, _val, _1)] >> -(qi::lit('f') | qi::lit('F'));
		r_Integer = r_CustomInteger[bind(Grammar::SetVariantValue<EONInteger>, _val, _1)];
		r_Boolean = qi::no_case[r_CustomBool][bind(Grammar::SetVariantValue<EONBoolean>, _val, _1)];
		r_String = r_DoubleQuotedString[bind(Grammar::SetVariantValue<EONString>, _val, _1)] 
				 | r_SingleQuotedString[bind(Grammar::SetVariantValue<EONString>, _val, _1)];
		r_Array = qi::lit('[')
			   >> *r_Comment
			   >> -r_Variant[bind(Grammar::AddVariantToArray, _val, _1)]
			   >> *(-r_Separator >> *r_Comment >> r_Variant[bind(Grammar::AddVariantToArray, _val, _1)])
			   >> *r_Comment
			   >> qi::lit(']');
		r_Object = qi::lit('{') 
				>> *(r_Comment | r_Variable[bind(Grammar::AddVariableToObject, _val, _1)])
				>> qi::lit('}');

		// String types
		r_DoubleQuotedString = qi::lit('"')
			>> *(s_EscapeChars | (qi::lit("\\x") >> qi::hex) | (ctype::char_ - qi::lit('"')))
			>> qi::lit('"');
		r_SingleQuotedString = qi::lit('\'')
			>> *(s_EscapeChars | (qi::lit("\\x") >> qi::hex) | (ctype::char_ - qi::lit('\'')))
			>> qi::lit('\'');

		// String escape characters
		s_EscapeChars.add("\\a", '\a')("\\b", '\b')("\\f", '\f')
			("\\n", '\n')("\\r", '\r')("\\t", '\t')("\\v", '\v')
			("\\\\", '\\')("\\\'", '\'')("\\\"", '\"');
	}

private:
	// Primary rule results in an unnamed Object
	boost::spirit::qi::rule<Iterator, EONObject(), Skipper> r_Root;

	// Comments are denoted by "# ... eol", "// ... eol", or "/* ... */"
	// All comments are ignored
	boost::spirit::qi::rule<Iterator> r_Comment, r_LineComment, r_BlockComment;

	// Terminators denote the end of a statement
	boost::spirit::qi::rule<Iterator> r_Terminator;

	// Separators denote the separation of siblings
	boost::spirit::qi::rule<Iterator> r_Separator;

	// Variables are Name:Variant pairs
	boost::spirit::qi::rule<Iterator, EONVariable(), Skipper> r_Variable, r_PrimitiveVariable, r_ArrayVariable, r_ObjectVariable;
	boost::spirit::qi::rule<Iterator, EONName(), Skipper> r_VariableName;
	boost::spirit::qi::rule<Iterator, EONString::ValueType(), Skipper> r_VariableIdentifier;
	boost::spirit::qi::rule<Iterator, EONName(), Skipper> r_VariableInheritor;
	boost::spirit::qi::rule<Iterator, EONString::ValueType(), Skipper> r_VariablePath;
	boost::spirit::qi::rule<Iterator, Skipper> r_VariableSeparator;
	boost::spirit::qi::rule<Iterator, EONVariant(), Skipper> r_Variant, r_PrimitiveVariant, r_ArrayVariant, r_ObjectVariant;

	// Variant types
	boost::spirit::qi::rule<Iterator, EONObject(), Skipper> r_Object;
	boost::spirit::qi::rule<Iterator, EONArray(), Skipper> r_Array;
	boost::spirit::qi::rule<Iterator, EONFloat()> r_Float;
	boost::spirit::qi::rule<Iterator, EONInteger()> r_Integer;
	boost::spirit::qi::rule<Iterator, EONBoolean()> r_Boolean;
	boost::spirit::qi::rule<Iterator, EONString()> r_String;
	
	// Custom parser components
	boost::spirit::qi::real_parser<EONInteger::ValueType, IntegerPolicies<EONInteger::ValueType>> r_CustomInteger;
	boost::spirit::qi::real_parser<EONFloat::ValueType, FloatPolicies<EONFloat::ValueType>> r_CustomFloat;
	boost::spirit::qi::bool_parser<EONBoolean::ValueType, BoolPolicies<EONBoolean::ValueType>> r_CustomBool;
	boost::spirit::qi::rule<Iterator, EONString::ValueType()> r_SingleQuotedString, r_DoubleQuotedString;

	boost::spirit::qi::symbols<const char, const char> s_EscapeChars;
};
