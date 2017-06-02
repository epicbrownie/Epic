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
#include <Epic/STL/ForwardList.hpp>
#include <algorithm>

//////////////////////////////////////////////////////////////////////////////

namespace Epic::EON
{
	namespace detail
	{
		template<class Container>
		struct ContainerInserter;
	}

	class Extractor;
}

//////////////////////////////////////////////////////////////////////////////

template<class ResultType, class Alloc, template<class, class> class _Container>
struct Epic::EON::detail::ContainerInserter<_Container<ResultType, Alloc>>
{
	using Container = _Container<ResultType, Alloc>;

	static inline void Reserve(Container& container, const typename Container::size_type)
	{
		/* Do Nothing */
	}

	static inline void Add(Container& container, ResultType&& result)
	{
		container.emplace_back(std::move(result));
	}
};

template<class ResultType, class Alloc>
struct Epic::EON::detail::ContainerInserter<std::vector<ResultType, Alloc>>
{
	using Container = std::vector<ResultType, Alloc>;

	static inline void Reserve(Container& container, const typename Container::size_type count)
	{
		container.reserve(count);
	}

	static inline void Add(Container& container, ResultType&& result)
	{
		container.emplace_back(std::move(result));
	}
};

template<class ResultType, class Alloc>
struct Epic::EON::detail::ContainerInserter<std::forward_list<ResultType, Alloc>>
{
	using Container = std::forward_list<ResultType, Alloc>;

	static inline void Reserve(Container& container, const typename Container::size_type)
	{
		/* Do Nothing */
	}

	static inline void Add(Container& container, ResultType&& result)
	{
		container.emplace_front(std::move(result));
	}
};

// Explicit support for Epic::SmallVector
template<class ResultType, class Alloc>
struct Epic::EON::detail::ContainerInserter<boost::container::small_vector_base<ResultType, Alloc>>
{
	using Container = boost::container::small_vector_base<ResultType, Alloc>;

	static inline void Reserve(Container& container, const typename Container::size_type count)
	{
		container.reserve(count);
	}

	static inline void Add(Container& container, ResultType&& result)
	{
		container.emplace_back(std::move(result));
	}
};

//////////////////////////////////////////////////////////////////////////////

// Extractor
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

	template<class SelectorFn>
	inline size_t _Extent(SelectorFn selectorFn) const
	{
		auto var = selectorFn(m_GlobalScope);
		if (!var) return 0;

		return std::visit(Epic::EON::detail::ExtentVisitor<void>(), var->Value.Data);
	}

	template<class SelectorFn, class FilterFn>
	inline size_t _Extent(SelectorFn selectorFn, FilterFn filterFn) const
	{
		auto var = selectorFn(m_GlobalScope);
		if (!var) return 0;

		auto vsExtent = Epic::EON::detail::ExtentVisitor<FilterFn>{ filterFn };
		return std::visit(vsExtent, var->Value.Data);
	}

	template<class ResultType, class SelectorFn, class DefaultFn, class AssignFn>
	bool _ExtractOne(ResultType& result, SelectorFn selectorFn, DefaultFn defaultFn, AssignFn assignFn) const
	{
		auto var = selectorFn(m_GlobalScope);
		if (!var)
			return detail::DefaultIf<DefaultFn, ResultType>::Apply(defaultFn, result);

		bool assigned = false;

		auto vsAssign = Epic::EON::detail::AssignVisitor<ResultType, AssignFn>{ result, assignFn };
		if (std::visit(vsAssign, var->Value.Data))
			assigned = true;
		
		return assigned;
	}

	template<class ResultType, class SelectorFn, class DefaultFn, class AssignFn, class ObjectType, class... MemberBindings>
	bool _ExtractOne(ResultType& result, SelectorFn selectorFn, DefaultFn defaultFn, 
		AssignFn assignFn, const ObjectBinding<ObjectType, MemberBindings...>& bindings) const
	{
		using Bindings = ObjectBinding<ObjectType, MemberBindings...>;

		auto var = selectorFn(m_GlobalScope);
		if (!var)
			return detail::DefaultIf<DefaultFn, ResultType>::Apply(defaultFn, result);

		bool assigned = false;

		// Extract var as ObjectType
		const EON::Object* pAsObject = std::get_if<EON::Object>(&var->Value.Data);

		if (pAsObject)
		{
			ObjectType thisResult;
			if ( (_ExtractObject(pAsObject, thisResult, bindings)) && 
					(detail::AssignIf<AssignFn, ObjectType, ResultType>::Apply(assignFn, thisResult, result)) )
				return true;
		}
		
		// Extract variable as array of ObjectType
		const EON::Array* pAsArray = std::get_if<EON::Array>(&var->Value.Data);

		if (pAsArray)
		{
			for (auto& member : pAsArray->Members)
			{
				const EON::Object* pMemberAsObj = std::get_if<EON::Object>(&member.Data);

				if (pMemberAsObj)
				{
					ObjectType thisResult;
					if ( (_ExtractObject(pMemberAsObj, thisResult, bindings)) && 
						(detail::AssignIf<AssignFn, ObjectType, ResultType>::Apply(assignFn, thisResult, result)) )
						assigned = true;
				}
			}
		}

		return assigned;
	}
	
	template<class ResultType, class Alloc, template<class, class> class Container, class SelectorFn, class DefaultFn, class AssignFn>
	bool _ExtractMany(Container<ResultType, Alloc>& results, SelectorFn selectorFn, DefaultFn defaultFn, AssignFn assignFn) const
	{
		using Inserter = detail::ContainerInserter<Container<ResultType, Alloc>>;

		auto vars = selectorFn(m_GlobalScope);
		if (std::begin(vars) == std::end(vars))
			return true;

		Inserter::Reserve(results, vars.size());

		bool assigned = false;

		for (auto pVariable : vars)
		{
			ResultType thisResult;

			auto vsAssign = detail::AssignVisitor<ResultType, AssignFn>{ thisResult, assignFn };
			if (std::visit(vsAssign, pVariable->Value.Data))
			{
				assigned = true;
				Inserter::Add(results, std::move(thisResult));
			}
		}

		return assigned;
	}
	
	template<class ResultType, class Alloc, template<class, class> class Container, class SelectorFn, class DefaultFn, class AssignFn, class ObjectType, class... MemberBindings>
	bool _ExtractMany(Container<ResultType, Alloc>& results, SelectorFn selectorFn, 
		DefaultFn defaultFn, AssignFn assignFn, const ObjectBinding<ObjectType, MemberBindings...>& bindings) const
	{
		using Inserter = detail::ContainerInserter<Container<ResultType, Alloc>>;
		using Bindings = ObjectBinding<ObjectType, MemberBindings...>;

		auto vars = selectorFn(m_GlobalScope);
		if (std::begin(vars) == std::end(vars))
			return true;
		
		Inserter::Reserve(results, vars.size());

		bool assigned = false;

		for (auto pVariable : vars)
		{
			const EON::Object* pAsObject = std::get_if<EON::Object>(&pVariable->Value.Data);

			if (pAsObject)
			{
				ObjectType thisResult;

				if (_ExtractObject(pAsObject, thisResult, bindings))
				{
					ResultType convertedResult;

					if (detail::AssignIf<AssignFn, ObjectType, ResultType>::Apply(assignFn, thisResult, convertedResult))
					{
						Inserter::Add(results, std::move(convertedResult));
						assigned = true;
					}
				}

				continue;
			}
			
			const EON::Array* pAsArray = std::get_if<EON::Array>(&pVariable->Value.Data);
			
			if (pAsArray)
			{
				ResultType accumResult;
				bool assignedArray = false;

				for (auto& member : pAsArray->Members)
				{
					const EON::Object* pMemberAsObj = std::get_if<EON::Object>(&member.Data);
					
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
					Inserter::Add(results, std::move(accumResult));
					assigned = true;
				}

				continue;
			}
		}

		return assigned;
	}

public:
	// Get a variable identified with 'selectorFn'.
	template<class ResultType, class SelectorFn, class AssignFn = detail::Assign<ResultType>>
	bool GetSingle(SelectorFn selectorFn, ResultType& result, AssignFn assignFn = AssignFn()) const
	{
		return _ExtractOne(result, selectorFn, detail::DefaultFail(), assignFn);
	}

	// Get a variable identified with 'selectorFn'.
	template<class ResultType, class SelectorFn, class AssignFn = detail::Assign<ResultType>>
	auto GetSingle(SelectorFn selectorFn, AssignFn assignFn = AssignFn()) const
	{
		ResultType result;
		GetSingle(selectorFn, result, assignFn);
		return result;
	}

	// Get a variable identified with 'selectorFn'. Extract it using 'bindings'.
	template<class ResultType, class SelectorFn, class ObjectType, class AssignFn = detail::Assign<ResultType>, class... MemberBindings>
	bool GetSingle(SelectorFn selectorFn, ResultType& result, const ObjectBinding<ObjectType, MemberBindings...>& bindings,
		AssignFn assignFn = AssignFn()) const
	{
		return _ExtractOne(result, selectorFn, detail::DefaultFail(), assignFn, bindings);
	}

	// Get a variable identified with 'selectorFn'. Extract it using 'bindings'.
	template<class ResultType, class SelectorFn, class ObjectType, class AssignFn = detail::Assign<ResultType>, class... MemberBindings>
	auto GetSingle(SelectorFn selectorFn, const ObjectBinding<ObjectType, MemberBindings...>& bindings,
		AssignFn assignFn = AssignFn()) const
	{
		ResultType result;
		GetSingle(selectorFn, result, bindings, assignFn);
		return result;
	}

public:
	// Get a variable identified with 'selectorFn' or assign 'defaultValue'.
	template<class ResultType, class SelectorFn, class DefaultType, class AssignFn = detail::Assign<ResultType>>
	inline bool GetSingleOr(SelectorFn selectorFn, ResultType& result, const DefaultType& defaultValue, 
		AssignFn assignFn = AssignFn()) const
	{
		return _ExtractOne(result, selectorFn, detail::Default<ResultType, DefaultType>{ defaultValue }, assignFn);
	}

	// Get a variable identified with 'selectorFn' or assign 'defaultValue'
	template<class ResultType, class SelectorFn, class DefaultType, class AssignFn = detail::Assign<ResultType>>
	inline auto GetSingleOr(SelectorFn selectorFn, const DefaultType& defaultValue, AssignFn assignFn = AssignFn()) const
	{
		ResultType result;
		GetSingleOr(selectorFn, result, defaultValue, assignFn);
		return result;
	}

	// Get a variable identified with 'selectorFn' or assign 'defaultValue'.
	template<class ResultType, class SelectorFn, class DefaultType, class ObjectType, class AssignFn = detail::Assign<ResultType>, class... MemberBindings>
	inline bool GetSingleOr(SelectorFn selectorFn, ResultType& result, const ObjectBinding<ObjectType, MemberBindings...>& bindings,
		const DefaultType& defaultValue, AssignFn assignFn = AssignFn()) const
	{
		return _ExtractOne(result, selectorFn, detail::Default<ResultType, DefaultType>{ defaultValue }, assignFn, bindings);
	}

	// Get a variable identified with 'selectorFn' or assign 'defaultValue'
	template<class ResultType, class SelectorFn, class DefaultType, class ObjectType, class AssignFn = detail::Assign<ResultType>, class... MemberBindings>
	inline auto GetSingleOr(SelectorFn selectorFn, const ObjectBinding<ObjectType, MemberBindings...>& bindings,
		const DefaultType& defaultValue, AssignFn assignFn = AssignFn()) const
	{
		ResultType result;
		GetSingleOr(selectorFn, result, bindings, defaultValue, assignFn);
		return result;
	}

public:
	// Get all variables identified with 'selectorFn'.
	template<class ResultType, class SelectorFn, class Alloc, template<class, class> class Container, class AssignFn = detail::Assign<ResultType>>
	inline bool GetMulti(SelectorFn selectorFn, Container<ResultType, Alloc>& results, AssignFn assignFn = AssignFn()) const
	{
		return _ExtractMany<ResultType>(results, selectorFn, detail::DefaultFail(), assignFn);
	}

	// Get all variables identified with 'selectorFn'.
	template<class ResultType, class SelectorFn, class AssignFn = detail::Assign<ResultType>>
	inline auto GetMulti(SelectorFn selectorFn, AssignFn assignFn = AssignFn()) const
	{
		Epic::STLVector<ResultType> results;
		GetMulti<ResultType>(selectorFn, results, assignFn);
		return results;
	}

	// Get all variables identified with 'selectorFn'. Extract them using 'bindings'.
	template<class ResultType, class SelectorFn, class Alloc, template<class, class> class Container, class ObjectType, class AssignFn = detail::Assign<ResultType>, class... MemberBindings>
	inline bool GetMulti(SelectorFn selectorFn, Container<ResultType, Alloc>& results,
		const ObjectBinding<ObjectType, MemberBindings...>& bindings, AssignFn assignFn = AssignFn()) const
	{
		return _ExtractMany<ResultType>(results, selectorFn, detail::DefaultFail(), assignFn, bindings);
	}

	// Get all variables identified with 'selectorFn'. Extract them using 'bindings'.
	template<class ResultType, class SelectorFn, class ObjectType, class AssignFn = detail::Assign<ResultType>, class... MemberBindings>
	inline auto GetMulti(SelectorFn selectorFn, const ObjectBinding<ObjectType, MemberBindings...>& bindings,
		AssignFn assignFn = AssignFn()) const
	{
		Epic::STLVector<ResultType> results;
		GetMulti<ResultType>(selectorFn, results, bindings, assignFn);
		return results;
	}

public:
	// Get all variables identified with 'selectorFn' or assign 'defaultValue'.
	template<class ResultType, class SelectorFn, class Alloc, template<class, class> class Container, class DefaultType, class AssignFn = detail::Assign<ResultType>>
	inline bool GetMultiOr(SelectorFn selectorFn, Container<ResultType, Alloc>& results,
		const DefaultType& defaultValue, AssignFn assignFn = AssignFn()) const
	{
		return _ExtractMany<ResultType>(results, selectorFn, detail::Default<ResultType, DefaultType>{ defaultValue }, assignFn);
	}

	// Get all variables identified with 'selectorFn' or assign 'defaultValue'
	template<class ResultType, class SelectorFn, class DefaultType, class AssignFn = detail::Assign<ResultType>>
	inline auto GetMultiOr(SelectorFn selectorFn, const DefaultType& defaultValue,
		AssignFn assignFn = AssignFn()) const
	{
		Epic::STLVector<ResultType> results;
		GetMultiOr<ResultType>(selectorFn, results, defaultValue, assignFn);
		return results;
	}

	// Get all variables identified with 'selectorFn' using 'bindings' or assign 'defaultValue'.
	template<class ResultType, class SelectorFn, class Alloc, template<class, class> class Container, class DefaultType, class ObjectType, class AssignFn = detail::Assign<ResultType>, class... MemberBindings>
	inline bool GetMultiOr(SelectorFn selectorFn, Container<ResultType, Alloc>& results,
		const ObjectBinding<ObjectType, MemberBindings...>& bindings, 
		const DefaultType& defaultValue, AssignFn assignFn = AssignFn()) const
	{
		return _ExtractMany<ResultType>(results, selectorFn, 
										detail::Default<ResultType, DefaultType>{ defaultValue }, 
										assignFn, bindings);
	}

	// Get all variables identified with 'selectorFn' using 'bindings' or assign 'defaultValue'
	template<class ResultType, class SelectorFn, class DefaultType, class ObjectType, class AssignFn = detail::Assign<ResultType>, class... MemberBindings>
	inline auto GetMultiOr(SelectorFn selectorFn, const ObjectBinding<ObjectType, MemberBindings...>& bindings,
		const DefaultType& defaultValue, AssignFn assignFn = AssignFn()) const
	{
		Epic::STLVector<ResultType> results;
		GetMultiOr<ResultType>(selectorFn, results, bindings, defaultValue, assignFn);
		return results;
	}

public:
	// Get a variable identified by 'varPath'.
	template<class ResultType, class AssignFn = detail::Assign<ResultType>>
	inline bool GetPath(const EON::Name& varPath, ResultType& result, AssignFn assignFn = AssignFn()) const
	{
		return GetSingle(EON::HasPath{ varPath }, result, assignFn);
	}

	// Get a variable identified by 'varPath'.
	template<class ResultType, class AssignFn = detail::Assign<ResultType>>
	inline auto GetPath(const EON::Name& varPath, AssignFn assignFn = AssignFn()) const
	{
		ResultType result;
		GetPath(varPath, result, assignFn);
		return result;
	}

	// Get a variable identified by 'varPath'. Extract it using 'bindings'.
	template<class ResultType, class ObjectType, class AssignFn = detail::Assign<ResultType>, class... MemberBindings>
	inline bool GetPath(const EON::Name& varPath, ResultType& result, const ObjectBinding<ObjectType, MemberBindings...>& bindings,
		AssignFn assignFn = AssignFn()) const
	{
		return GetSingle(EON::HasPath{ varPath }, result, bindings, assignFn);
	}

	// Get a variable identified by 'varPath'. Extract it using 'bindings'.
	template<class ResultType, class ObjectType, class AssignFn = detail::Assign<ResultType>, class... MemberBindings>
	inline auto GetPath(const EON::Name& varPath, const ObjectBinding<ObjectType, MemberBindings...>& bindings,
		AssignFn assignFn = AssignFn()) const
	{
		ResultType result;
		GetPath(varPath, result, bindings, assignFn);
		return result;
	}

public:
	// Get a variable identified by 'varPath' or assign 'defaultValue'.
	template<class ResultType, class DefaultType, class AssignFn = detail::Assign<ResultType>>
	inline bool GetPathOr(const EON::Name& varPath, ResultType& result, const DefaultType& defaultValue, 
		AssignFn assignFn = AssignFn()) const
	{
		return GetSingleOr(EON::HasPath{ varPath }, result, defaultValue, assignFn);
	}

	// Get a variable identified by 'varPath' or assign 'defaultValue'
	template<class ResultType, class DefaultType, class AssignFn = detail::Assign<ResultType>>
	inline auto GetPathOr(const EON::Name& varPath, const DefaultType& defaultValue,
		AssignFn assignFn = AssignFn()) const
	{
		ResultType result;
		GetPathOr(varPath, result, defaultValue, assignFn);
		return result;
	}

	// Get a variable identified by 'varPath' or assign 'defaultValue'.
	template<class ResultType, class DefaultType, class ObjectType, class AssignFn = detail::Assign<ResultType>, class... MemberBindings>
	inline bool GetPathOr(const EON::Name& varPath, ResultType& result, const ObjectBinding<ObjectType, MemberBindings...>& bindings,
		const DefaultType& defaultValue, AssignFn assignFn = AssignFn()) const
	{
		return GetSingleOr(EON::HasPath{ varPath }, result, bindings, defaultValue, assignFn);
	}

	// Get a variable identified by 'varPath' or assign 'defaultValue'
	template<class ResultType, class DefaultType, class ObjectType, class AssignFn = detail::Assign<ResultType>, class... MemberBindings>
	inline auto GetPathOr(const EON::Name& varPath, const ObjectBinding<ObjectType, MemberBindings...>& bindings,
		const DefaultType& defaultValue, AssignFn assignFn = AssignFn()) const
	{
		ResultType result;
		GetPathOr(varPath, result, bindings, defaultValue, assignFn);
		return result;
	}

public:
	// Get a variable identified by 'varName'.
	template<class ResultType, class AssignFn = detail::Assign<ResultType>>
	inline bool GetNamed(const EON::NameHash& varName, ResultType& result, AssignFn assignFn = AssignFn()) const
	{
		return GetSingle(EON::HasName{ varName }, result, assignFn);
	}

	// Get a variable identified by 'varName'.
	template<class ResultType, class AssignFn = detail::Assign<ResultType>>
	inline auto GetNamed(const EON::NameHash& varName, AssignFn assignFn = AssignFn()) const
	{
		ResultType result;
		GetNamed(varName, result, assignFn);
		return result;
	}

	// Get a variable identified by 'varName'. Extract it using 'bindings'.
	template<class ResultType, class ObjectType, class AssignFn = detail::Assign<ResultType>, class... MemberBindings>
	inline bool GetNamed(const EON::NameHash& varName, ResultType& result, const ObjectBinding<ObjectType, MemberBindings...>& bindings,
		AssignFn assignFn = AssignFn()) const
	{
		return GetSingle(EON::HasName{ varName }, result, bindings, assignFn);
	}

	// Get a variable identified by 'varName'. Extract it using 'bindings'.
	template<class ResultType, class ObjectType, class AssignFn = detail::Assign<ResultType>, class... MemberBindings>
	inline auto GetNamed(const EON::NameHash& varName, const ObjectBinding<ObjectType, MemberBindings...>& bindings,
		AssignFn assignFn = AssignFn()) const
	{
		ResultType result;
		GetNamed(varName, result, bindings, assignFn);
		return result;
	}

public:
	// Get a variable identified by 'varName' or assign 'defaultValue'.
	template<class ResultType, class DefaultType, class AssignFn = detail::Assign<ResultType>>
	inline bool GetNamedOr(const EON::NameHash& varName, ResultType& result, const DefaultType& defaultValue, 
		AssignFn assignFn = AssignFn()) const
	{
		return GetSingleOr(EON::HasName{ varName }, result, defaultValue, assignFn);
	}

	// Get a variable identified by 'varName' or assign 'defaultValue'
	template<class ResultType, class DefaultType, class AssignFn = detail::Assign<ResultType>>
	inline auto GetNamedOr(const EON::NameHash& varName, const DefaultType& defaultValue,
		AssignFn assignFn = AssignFn()) const
	{
		ResultType result;
		GetNamedOr(varName, result, defaultValue, assignFn);
		return result;
	}

	// Get a variable identified by 'varName' or assign 'defaultValue'.
	template<class ResultType, class DefaultType, class ObjectType, class AssignFn = detail::Assign<ResultType>, class... MemberBindings>
	inline bool GetNamedOr(const EON::NameHash& varName, ResultType& result, const ObjectBinding<ObjectType, MemberBindings...>& bindings,
		const DefaultType& defaultValue, AssignFn assignFn = AssignFn()) const
	{
		return GetSingleOr(EON::HasName{ varName }, result, bindings, defaultValue, assignFn);
	}

	// Get a variable identified by 'varName' or assign 'defaultValue'
	template<class ResultType, class DefaultType, class ObjectType, class AssignFn = detail::Assign<ResultType>, class... MemberBindings>
	inline auto GetNamedOr(const EON::NameHash& varName, const ObjectBinding<ObjectType, MemberBindings...>& bindings,
		const DefaultType& defaultValue, AssignFn assignFn = AssignFn()) const
	{
		ResultType result;
		GetNamedOr(varName, result, bindings, defaultValue, assignFn);
		return result;
	}

public:
	// Get all variables descended from 'varAncestor'.
	template<class ResultType, class Alloc, template<class, class> class Container, class AssignFn = detail::Assign<ResultType>>
	inline bool GetDesc(const EON::Name& varAncestor, Container<ResultType, Alloc>& results, AssignFn assignFn = AssignFn()) const
	{
		return GetMulti(EON::HasParent{ varAncestor }, results, assignFn);
	}

	// Get all variables descended from 'varAncestor'.
	template<class ResultType, class AssignFn = detail::Assign<ResultType>>
	inline auto GetDesc(const EON::Name& varAncestor, AssignFn assignFn = AssignFn()) const
	{
		Epic::STLVector<ResultType> results;
		GetDesc<ResultType>(varAncestor, results, assignFn);
		return results;
	}

	// Get all variables descended from 'varAncestor'. Extract them using 'bindings'.
	template<class ResultType, class Alloc, template<class, class> class Container, class ObjectType, class AssignFn = detail::Assign<ResultType>, class... MemberBindings>
	inline bool GetDesc(const EON::Name& varAncestor, Container<ResultType, Alloc>& results,
		const ObjectBinding<ObjectType, MemberBindings...>& bindings, AssignFn assignFn = AssignFn()) const
	{
		return GetMulti(EON::HasParent{ varAncestor }, results, bindings, assignFn);
	}

	// Get all variables descended from 'varAncestor'. Extract them using 'bindings'.
	template<class ResultType, class ObjectType, class AssignFn = detail::Assign<ResultType>, class... MemberBindings>
	inline auto GetDesc(const EON::Name& varAncestor, const ObjectBinding<ObjectType, MemberBindings...>& bindings,
		AssignFn assignFn = AssignFn()) const
	{
		Epic::STLVector<ResultType> results;
		GetDesc<ResultType>(varAncestor, results, bindings, assignFn);
		return results;
	}

public:
	// Get all variables descended from 'varAncestor' or assign 'defaultValue'.
	template<class ResultType, class Alloc, template<class, class> class Container, class DefaultType, class AssignFn = detail::Assign<ResultType>>
	inline bool GetDescOr(const EON::Name& varAncestor, Container<ResultType, Alloc>& results,
		const DefaultType& defaultValue, AssignFn assignFn = AssignFn()) const
	{
		return GetMultiOr(EON::HasParent{ varAncestor }, results, defaultValue, assignFn);
	}

	// Get all variables descended from 'varAncestor' or assign 'defaultValue'
	template<class ResultType, class DefaultType, class AssignFn = detail::Assign<ResultType>>
	inline auto GetDescOr(const EON::Name& varAncestor, const DefaultType& defaultValue,
		AssignFn assignFn = AssignFn()) const
	{
		Epic::STLVector<ResultType> results;
		GetDescOr<ResultType>(varAncestor, results, defaultValue, assignFn);
		return results;
	}

	// Get all variables descended from 'varAncestor' using 'bindings' or assign 'defaultValue'.
	template<class ResultType, class Alloc, template<class, class> class Container, class DefaultType, class ObjectType, class AssignFn = detail::Assign<ResultType>, class... MemberBindings>
	inline bool GetDescOr(const EON::Name& varAncestor, Container<ResultType, Alloc>& results,
		const ObjectBinding<ObjectType, MemberBindings...>& bindings, 
		const DefaultType& defaultValue, AssignFn assignFn = AssignFn()) const
	{
		return GetMultiOr(EON::HasParent{ varAncestor }, results, bindings, defaultValue, assignFn);
	}

	// Get all variables descended from 'varAncestor' using 'bindings' or assign 'defaultValue'
	template<class ResultType, class DefaultType, class ObjectType, class AssignFn = detail::Assign<ResultType>, class... MemberBindings>
	inline auto GetDescOr(const EON::Name& varAncestor, const ObjectBinding<ObjectType, MemberBindings...>& bindings,
		const DefaultType& defaultValue, AssignFn assignFn = AssignFn()) const
	{
		Epic::STLVector<ResultType> results;
		GetDescOr<ResultType>(varAncestor, results, bindings, defaultValue, assignFn);
		return results;
	}

public:
	// Get the extent of the variable identified with 'selectorFn'.
	template<class SelectorFn>
	inline size_t GetSingleExtent(SelectorFn selectorFn) const
	{
		return _Extent(selectorFn);
	}

	template<class SelectorFn, class FilterFn>
	inline size_t GetSingleExtent(SelectorFn selectorFn, FilterFn filterFn) const
	{
		return _Extent(selectorFn, filterFn);
	}

	inline size_t GetPathExtent(const EON::Name& varPath) const
	{
		return _Extent(Epic::EON::HasPath{ varPath });
	}

	template<class FilterFn>
	inline size_t GetPathExtent(const EON::Name& varPath, FilterFn filterFn) const
	{
		return _Extent(Epic::EON::HasPath{ varPath }, filterFn);
	}

	inline size_t GetNamedExtent(const EON::NameHash& varName) const
	{
		return _Extent(Epic::EON::HasName{ varName });
	}

	template<class FilterFn>
	inline size_t GetNamedExtent(const EON::NameHash& varName, FilterFn filterFn) const
	{
		return _Extent(Epic::EON::HasName{ varName }, filterFn);
	}
};
