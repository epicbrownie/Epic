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
#include <Epic/EON/Extractor.hpp>
#include <Epic/EON/detail/Utility.hpp>
#include <algorithm>
#include <boost/variant.hpp>
#include <cassert>
#include <stdexcept>
#include <string>

//////////////////////////////////////////////////////////////////////////////

namespace Epic::EON
{
	class Bin;

	struct InvalidIdentifierException;
	struct InvalidValueException;

	namespace detail
	{
		class VariableInheritVisitor;
		class VariableMergeVisitor;

		namespace
		{
			bool Tidy(const EON::Object* pGlobal, EON::Object* pScope, EON::Variable& variable, bool resolveInheritance = true);
			void ResolveInheritance(EON::Variable& variable, const EON::Object* pGlobal);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////

// InvalidIdentifierException
struct Epic::EON::InvalidIdentifierException : public std::runtime_error
{
	using std::runtime_error::runtime_error;
};

// InvalidValueException
struct Epic::EON::InvalidValueException : public std::runtime_error
{
	using std::runtime_error::runtime_error;
};

//////////////////////////////////////////////////////////////////////////////

// VariableInheritVisitor
class Epic::EON::detail::VariableInheritVisitor : public boost::static_visitor<>
{
public:
	VariableInheritVisitor() = default;

private:
	template<typename From, typename To>
	struct Inheritor
	{
		inline static void Apply(const From&, const To&)
		{
			// Invalid inheritance
			throw InvalidValueException("Incompatible types.");
		}
	};

	template<typename Type>
	struct Inheritor<Type, Type>
	{
		inline static void Apply(const Type& from, Type& to)
		{
			to.Value = from.Value;
		}
	};

	template<>
	struct Inheritor<EON::Object, EON::Object>
	{
		inline static void Apply(const EON::Object& from, EON::Object& to)
		{
			std::copy(std::begin(from.Members),
					  std::end(from.Members),
					  std::inserter(to.Members, std::begin(to.Members)));
		}
	};

	template<>
	struct Inheritor<EON::Array, EON::Array>
	{
		inline static void Apply(const EON::Array& from, EON::Array& to)
		{
			std::copy(std::begin(from.Members),
					  std::end(from.Members),
					  std::inserter(to.Members, std::begin(to.Members)));
		}
	};
	
	template<>
	struct Inheritor<EON::String, EON::String>
	{
		inline static void Apply(const EON::String& from, EON::String& to)
		{
			to.Value.insert(0, from.Value);
		}
	};

public:
	template<typename From, typename To>
	void operator() (const From& from, To& to) const
	{
		Inheritor<From, To>::Apply(from, to);
	}
};

//////////////////////////////////////////////////////////////////////////////

// VariableMergeVisitor
class Epic::EON::detail::VariableMergeVisitor : public boost::static_visitor<>
{
private:
	const EON::Variant& _From;
	EON::Variant& _To;

public:
	VariableMergeVisitor(const EON::Variant& from, EON::Variant& to)
		: _From{ from }, _To{ to }  { }

private:
	template<typename From, typename To>
	struct Merger
	{
		inline static void Apply(const EON::Variant& fromv, const From&, EON::Variant& tov, To&)
		{
			tov = fromv;
		}
	};

	template<>
	struct Merger<EON::Object, EON::Object>
	{
		inline static void Apply(const EON::Variant&, const EON::Object& from, EON::Variant&, EON::Object& to)
		{
			// Move all of the members of "from" to "to"
			for (auto& vfrom : from.Members)
			{
				bool isDuplicate = false;

				for (auto& vto : to.Members)
				{
					if (vto.Name == vfrom.Name)
					{
						isDuplicate = true;

						VariableMergeVisitor vsMerger(vfrom.Value, vto.Value);
						boost::apply_visitor(vsMerger, vfrom.Value.Data, vto.Value.Data);

						break;
					}
				}

				if (!isDuplicate)
					to.Members.emplace_back(vfrom);
			}
		}
	};

public:
	template<typename From, typename To>
	void operator() (const From& from, To& to)
	{
		Merger<From, To>::Apply(_From, from, _To, to);
	}
};

//////////////////////////////////////////////////////////////////////////////

// Bin
class Epic::EON::Bin
{
private:
	EON::Object m_Data;

public:
	Bin() = default;
	Bin(const Bin&) = default;
	Bin(Bin&&) = default;

public:
	EON::Extractor GetExtractor() const
	{
		return EON::Extractor(m_Data);
	}

	const EON::Object& Get() const { return m_Data; }

public:
	inline void Insert(const EON::Object& obj)
	{
		EON::Object v{ obj };
		Insert(std::move(v));
	}

	inline void Insert(EON::Object&& obj)
	{
		for (auto& v : obj.Members)
			Insert(std::move(v));
	}

	inline void Insert(const EON::Variable& variable)
	{
		EON::Variable v{ variable };
		Insert(std::move(v));
	}

	inline void Insert(EON::Variable&& variable)
	{
		assert(variable.Name != "" && "Variable must have a name");

		if (detail::Tidy(&m_Data, &m_Data, variable, true))
		{
			m_Data.Members.emplace
			(
				std::upper_bound
				(
					std::begin(m_Data.Members),
					std::end(m_Data.Members),
					variable,
					[] (const auto& a, const auto& b) { return a.NameHash < b.NameHash; }
				),
				std::move(variable)
			);
		}
	}
};

//////////////////////////////////////////////////////////////////////////////

// Helpers
namespace Epic::EON::detail
{
	namespace
	{
		bool Tidy(const EON::Object* pGlobal, EON::Object* pScope, EON::Variable& variable, bool resolveInheritance)
		{
			// Resolve inheritance of the variable and tidy its members.
			// If 'variable' is a duplicate of an existing variable in 'pScope',
			// Tidy will merge its data into the existing member and return false.
			// Tidy returns true if no duplicate was found.

			assert(pGlobal);
			assert(pScope);

			// If the variable is an object, resolve the inheritance of its native members
			EON::Object* pObject = boost::get<Object>(&variable.Value.Data);
			if (pObject)
			{
				for (auto& member : pObject->Members)
					if(!member.Parent.empty())
						detail::ResolveInheritance(member, pGlobal);
			}

			// Now resolve the inheritance of the variable
			if (resolveInheritance && !variable.Parent.empty())
				detail::ResolveInheritance(variable, pGlobal);

			// Finally, if the variable is an object, tidy and sort its members
			if (pObject)
			{
				for (size_t i = 0; i < pObject->Members.size(); )
				{
					if (!Tidy(pGlobal, pObject, pObject->Members[i], false))
						pObject->Members.erase(std::begin(pObject->Members) + i);
					else
						++i;
				}

				std::sort
				(
					std::begin(pObject->Members),
					std::end(pObject->Members),
					[] (const auto& a, const auto& b) { return a.NameHash < b.NameHash; }
				);
			}

			// Search the scope for a duplicate of the variable
			auto itDup = std::find_if(std::begin(pScope->Members), std::end(pScope->Members),
									  [&] (auto& v) { return v.Name == variable.Name; });

			if (itDup != std::end(pScope->Members) && &(*itDup) != &variable)
			{
				VariableMergeVisitor vsMerge(variable.Value, (*itDup).Value);
				boost::apply_visitor(vsMerge, variable.Value.Data, (*itDup).Value.Data);

				return false;
			}

			return true;
		}

		void ResolveInheritance(EON::Variable& variable, const EON::Object* pGlobal)
		{
			assert(!variable.Parent.empty());
			assert(pGlobal);
			
			const EON::Variable* pParent = GetVariableInObject(*pGlobal, variable.Parent);
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
					boost::apply_visitor(detail::VariableInheritVisitor(), pParent->Value.Data, variable.Value.Data);
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
