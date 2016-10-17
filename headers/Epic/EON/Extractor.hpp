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

#include <Epic/EON/Bind.hpp>
#include <Epic/EON/Types.hpp>
#include <Epic/EON/Selector.hpp>
#include <Epic/EON/detail/Assign.hpp>
#include <Epic/EON/detail/Utility.hpp>
#include <Epic/STL/Vector.hpp>
#include <algorithm>

//////////////////////////////////////////////////////////////////////////////

namespace Epic::EON
{
	class Extractor;
}

//////////////////////////////////////////////////////////////////////////////

class Epic::EON::Extractor
{
private:
	const EON::Object& m_GlobalScope;

public:
	Extractor() = delete;
	
	explicit Extractor(const EON::Object& globalScope)
		: m_GlobalScope{ globalScope } 
	{ }
	
	Extractor(const Extractor& other)
		: m_GlobalScope{ other.m_GlobalScope } 
	{ }

private:
	template<class ObjectType, class... MemberBindings>
	bool _ExtractObject(const EON::Object* pObject, ObjectType& result, 
		const ObjectBinding<ObjectType, MemberBindings...>& bindings) const
	{
		using Bindings = ObjectBinding<ObjectType, MemberBindings...>;

		for (size_t i = 0; i < Bindings::MemberBindings; ++i)
		{
			if (!bindings.Assign(*pObject, result, i))
				return false;
		}

		return true;
	}

	template<class ResultType, class SelectorFn, class DefaultFn, class AssignFn>
	bool _ExtractOne(ResultType& result, SelectorFn selectorFn, DefaultFn defaultFn, AssignFn assignFn) const
	{
		auto vars = selectorFn(m_GlobalScope);
		if (std::begin(vars) == std::end(vars))
			return detail::DefaultIf<DefaultFn, ResultType>::Apply(defaultFn, result);

		bool assigned = false;

		for (auto pVariable : vars)
		{
			auto vsAssign = Epic::EON::detail::AssignVisitor<ResultType, AssignFn>{ result, assignFn };
			if (boost::apply_visitor(vsAssign, pVariable->Value.Data))
				assigned = true;
		}

		return assigned;
	}

	template<class ResultType, class SelectorFn, class DefaultFn, class AssignFn, class ObjectType, class... MemberBindings>
	bool _ExtractOne(ResultType& result, SelectorFn selectorFn, DefaultFn defaultFn, 
		AssignFn assignFn, const ObjectBinding<ObjectType, MemberBindings...>& bindings) const
	{
		using Bindings = ObjectBinding<ObjectType, MemberBindings...>;

		auto vars = selectorFn(m_GlobalScope);
		if (std::begin(vars) == std::end(vars))
			return detail::DefaultIf<DefaultFn, ResultType>::Apply(defaultFn, result);

		bool assigned = false;

		for (auto pVariable : vars)
		{
			// Extract pVariable as ObjectType
			const EON::Object* pAsObject = boost::get<EON::Object>(&pVariable->Value.Data);

			if (pAsObject)
			{
				ObjectType thisResult;
				if ( (_ExtractObject(pAsObject, thisResult, bindings)) && 
					 (detail::AssignIf<AssignFn, ObjectType, ResultType>::Apply(assignFn, thisResult, result)) )
					assigned = true;

				continue;
			}
		
			// Extract variable as array of ObjectType
			const EON::Array* pAsArray = boost::get<EON::Array>(&pVariable->Value.Data);

			if (pAsArray)
			{
				for (auto& member : pAsArray->Members)
				{
					const EON::Object* pMemberAsObj = boost::get<EON::Object>(&member.Data);

					if (pMemberAsObj)
					{
						ObjectType thisResult;
						if ( (_ExtractObject(pMemberAsObj, thisResult, bindings)) && 
							(detail::AssignIf<AssignFn, ObjectType, ResultType>::Apply(assignFn, thisResult, result)) )
							assigned = true;
					}
				}

				continue;
			}
		}

		return assigned;
	}
	
	template<class ResultType, class Alloc, class SelectorFn, class DefaultFn, class AssignFn>
	bool _ExtractMany(std::vector<ResultType, Alloc>& result, SelectorFn selectorFn, 
		DefaultFn defaultFn, AssignFn assignFn) const
	{
		auto vars = selectorFn(m_GlobalScope);
		if (std::begin(vars) == std::end(vars))
			return detail::DefaultIf<DefaultFn, std::vector<ResultType, Alloc>>::Apply(defaultFn, result);

		result.reserve(result.size() + vars.size());
		
		bool assigned = false;

		for (auto pVariable : vars)
		{
			ResultType thisResult;

			auto vsAssign = detail::AssignVisitor<ResultType, AssignFn>{ thisResult, assignFn };
			if (boost::apply_visitor(vsAssign, pVariable->Value.Data))
			{
				assigned = true;
				result.emplace_back(std::move(thisResult));
			}
		}

		return assigned;
	}
	
	template<class ResultType, class Alloc, class SelectorFn, class DefaultFn, class AssignFn, class ObjectType, class... MemberBindings>
	bool _ExtractMany(std::vector<ResultType, Alloc>& result, SelectorFn selectorFn, 
		DefaultFn defaultFn, AssignFn assignFn, const ObjectBinding<ObjectType, MemberBindings...>& bindings) const
	{
		using Bindings = ObjectBinding<ObjectType, MemberBindings...>;

		auto vars = selectorFn(m_GlobalScope);
		if (std::begin(vars) == std::end(vars))
			return detail::DefaultIf<DefaultFn, std::vector<ResultType, Alloc>>::Apply(defaultFn, result);

		result.reserve(result.size() + vars.size());
		
		bool assigned = false;

		for (auto pVariable : vars)
		{
			const EON::Object* pAsObject = boost::get<EON::Object>(&pVariable->Value.Data);

			if (pAsObject)
			{
				ObjectType thisResult;

				if (_ExtractObject(pAsObject, thisResult, bindings))
				{
					ResultType convertedResult;

					if (detail::AssignIf<AssignFn, ObjectType, ResultType>::Apply(assignFn, thisResult, convertedResult))
					{
						result.emplace_back(std::move(convertedResult));
						assigned = true;
					}
				}

				continue;
			}
			
			const EON::Array* pAsArray = boost::get<EON::Array>(&pVariable->Value.Data);
			
			if (pAsArray)
			{
				ResultType accumResult;
				bool assignedArray = false;

				for (auto& member : pAsArray->Members)
				{
					const EON::Object* pMemberAsObj = boost::get<EON::Object>(&member.Data);
					
					if (pMemberAsObj)
					{
						ObjectType thisResult;

						if( (_ExtractObject(pMemberAsObj, thisResult, bindings)) &&
							(detail::AssignIf<AssignFn, ObjectType, ResultType>::Apply(assignFn, thisResult, accumResult)) )
							assignedArray = true;
					}
				}

				if (assignedArray)
				{
					result.emplace_back(std::move(accumResult));
					assigned = true;
				}

				continue;
			}
		}

		return assigned;
	}

public:
	// Get a variable identified by 'varName'.
	template<class ResultType, class AssignFn = detail::Assign<ResultType>>
	bool GetNamed(const EON::Name& varName, ResultType& result, AssignFn assignFn = AssignFn()) const
	{
		return _ExtractOne(result, EON::HasName{ varName }, detail::DefaultFail(), assignFn);
	}

	// Get a variable identified by 'varName'.
	template<class ResultType, class AssignFn = detail::Assign<ResultType>>
	auto GetNamed(const EON::Name& varName, AssignFn assignFn = AssignFn()) const
	{
		ResultType result;
		GetNamed(varName, result, assignFn);
		return result;
	}

	// Get a variable identified by 'varName'. Extract it using 'bindings'.
	template<class ResultType, class ObjectType, class AssignFn = detail::Assign<ResultType>, class... MemberBindings>
	bool GetNamed(const EON::Name& varName, ResultType& result, const ObjectBinding<ObjectType, MemberBindings...>& bindings,
		AssignFn assignFn = AssignFn()) const
	{
		return _ExtractOne(result, EON::HasName{ varName }, detail::DefaultFail(), assignFn, bindings);
	}

	// Get a variable identified by 'varName'. Extract it using 'bindings'.
	template<class ResultType, class ObjectType, class AssignFn = detail::Assign<ResultType>, class... MemberBindings>
	auto GetNamed(const EON::Name& varName, const ObjectBinding<ObjectType, MemberBindings...>& bindings,
		AssignFn assignFn = AssignFn()) const
	{
		ResultType result;
		GetNamed(varName, result, bindings, assignFn);
		return result;
	}

public:
	// Get a variable identified by 'varName' or assign 'defaultValue'.
	template<class ResultType, class DefaultType, class AssignFn = detail::Assign<ResultType>>
	bool GetNamedOr(const EON::Name& varName, ResultType& result, const DefaultType& defaultValue,
		AssignFn assignFn = AssignFn()) const
	{
		return _ExtractOne(result, EON::HasName{ varName }, detail::Default<ResultType, DefaultType>{ defaultValue }, assignFn);
	}

	// Get a variable identified by 'varName' or assign 'defaultValue'
	template<class ResultType, class DefaultType, class AssignFn = detail::Assign<ResultType>>
	auto GetNamedOr(const EON::Name& varName, const DefaultType& defaultValue,
		AssignFn assignFn = AssignFn()) const
	{
		ResultType result;
		GetNamedOr(varName, result, defaultValue, assignFn);
		return result;
	}

	// Get a variable identified by 'varName' or assign 'defaultValue'.
	template<class ResultType, class DefaultType, class ObjectType, class AssignFn = detail::Assign<ResultType>, class... MemberBindings>
	bool GetNamedOr(const EON::Name& varName, ResultType& result, const ObjectBinding<ObjectType, MemberBindings...>& bindings,
		const DefaultType& defaultValue, AssignFn assignFn = AssignFn()) const
	{
		return _ExtractOne(result, EON::HasName{ varName }, 
			detail::Default<ResultType, DefaultType>{ defaultValue }, assignFn, bindings);
	}

	// Get a variable identified by 'varName' or assign 'defaultValue'
	template<class ResultType, class DefaultType, class ObjectType, class AssignFn = detail::Assign<ResultType>, class... MemberBindings>
	auto GetNamedOr(const EON::Name& varName, const ObjectBinding<ObjectType, MemberBindings...>& bindings,
		const DefaultType& defaultValue, AssignFn assignFn = AssignFn()) const
	{
		ResultType result;
		GetNamedOr(varName, result, bindings, defaultValue, assignFn);
		return result;
	}

public:
	// Get all variables descended from 'varAncestor'.
	template<class ResultType, class Alloc, class AssignFn = detail::Assign<ResultType>>
	bool GetDesc(const EON::Name& varAncestor, std::vector<ResultType, Alloc>& result, 
		AssignFn assignFn = AssignFn()) const
	{
		return _ExtractMany(result, EON::HasParent{ varAncestor }, detail::DefaultFail(), assignFn);
	}

	// Get a variable identified by 'varAncestor'.
	template<class ResultType, class AssignFn = detail::Assign<ResultType>>
	auto GetDesc(const EON::Name& varAncestor, AssignFn assignFn = AssignFn()) const
	{
		Epic::STLVector<ResultType> result;
		GetDesc(varAncestor, result, assignFn);
		return result;
	}

	// Get a variable identified by 'varAncestor'. Extract it using 'bindings'.
	template<class ResultType, class Alloc, class ObjectType, class AssignFn = detail::Assign<ResultType>, class... MemberBindings>
	bool GetDesc(const EON::Name& varAncestor, std::vector<ResultType, Alloc>& result, 
		const ObjectBinding<ObjectType, MemberBindings...>& bindings, AssignFn assignFn = AssignFn()) const
	{
		return _ExtractMany(result, EON::HasParent{ varAncestor }, detail::DefaultFail(), assignFn, bindings);
	}

	// Get a variable identified by 'varAncestor'. Extract it using 'bindings'.
	template<class ResultType, class ObjectType, class AssignFn = detail::Assign<ResultType>, class... MemberBindings>
	auto GetDesc(const EON::Name& varAncestor, const ObjectBinding<ObjectType, MemberBindings...>& bindings,
		AssignFn assignFn = AssignFn()) const
	{
		Epic::STLVector<ResultType> result;
		GetDesc(varAncestor, result, bindings, assignFn);
		return result;
	}

public:
	// Get a variable identified by 'varName' or assign 'defaultValue'.
	template<class ResultType, class Alloc, class DefaultType, class AssignFn = detail::Assign<ResultType>>
	bool GetDescOr(const EON::Name& varAncestor, std::vector<ResultType, Alloc>& result, 
		const DefaultType& defaultValue, AssignFn assignFn = AssignFn()) const
	{
		return _ExtractMany(result, EON::HasParent{ varAncestor }, detail::Default<ResultType, DefaultType>{ defaultValue }, assignFn);
	}

	// Get a variable identified by 'varName' or assign 'defaultValue'
	template<class ResultType, class DefaultType, class AssignFn = detail::Assign<ResultType>>
	auto GetDescOr(const EON::Name& varAncestor, const DefaultType& defaultValue,
		AssignFn assignFn = AssignFn()) const
	{
		Epic::STLVector<ResultType> result;
		GetDescOr(varAncestor, result, defaultValue, assignFn);
		return result;
	}

	// Get a variable identified by 'varName' or assign 'defaultValue'.
	template<class ResultType, class Alloc, class DefaultType, class ObjectType, class AssignFn = detail::Assign<ResultType>, class... MemberBindings>
	bool GetDescOr(const EON::Name& varAncestor, std::vector<ResultType, Alloc>& result, 
		const ObjectBinding<ObjectType, MemberBindings...>& bindings, 
		const DefaultType& defaultValue, AssignFn assignFn = AssignFn()) const
	{
		return _ExtractMany(result, EON::HasParent{ varAncestor }, 
			detail::Default<ResultType, DefaultType>{ defaultValue }, assignFn, bindings);
	}

	// Get a variable identified by 'varName' or assign 'defaultValue'
	template<class ResultType, class DefaultType, class ObjectType, class AssignFn = detail::Assign<ResultType>, class... MemberBindings>
	auto GetDescOr(const EON::Name& varAncestor, const ObjectBinding<ObjectType, MemberBindings...>& bindings,
		const DefaultType& defaultValue, AssignFn assignFn = AssignFn()) const
	{
		Epic::STLVector<ResultType> result;
		GetDescOr(varAncestor, result, bindings, defaultValue, assignFn);
		return result;
	}
};
