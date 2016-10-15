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

#include <Epic/EON/detail/Assign.hpp>
#include <Epic/EON/detail/Utility.hpp>
#include <Epic/EON/Types.hpp>
#include <Epic/EON/Selector.hpp>
#include <boost/variant/get.hpp>
#include <boost/variant/apply_visitor.hpp>

//////////////////////////////////////////////////////////////////////////////

namespace Epic::EON::detail
{
	template<class CType, class... MTypes>
	struct ObjBnd;

	template<class CType, class MType, class FilterFn, class AssignFn, class... Bindings>
	struct MemBnd;
}

//////////////////////////////////////////////////////////////////////////////

template<class CType, class MType, class FilterFn, class AssignFn, class... Bindings>
struct Epic::EON::detail::MemBnd
{
	typedef MType CType::*Pointer;
	using ObjectValueBinding = ObjBnd<MType, Bindings...>;

	const Pointer _pMember;
	const ObjectValueBinding _ObjBinding;
	FilterFn _FilterFn;
	AssignFn _AssignFn;

	MemBnd(Pointer pMember, FilterFn filterFn, AssignFn assignFn)
		: _pMember(pMember), _FilterFn(filterFn), _AssignFn(assignFn), _ObjBinding() { }

	MemBnd(Pointer pMember, FilterFn filterFn, AssignFn assignFn, const ObjectValueBinding& objBinding)
		: _pMember(pMember), _FilterFn(filterFn), _AssignFn(assignFn), _ObjBinding(objBinding) { }

	bool Assign(const Epic::EON::Object& scope, CType& instance) const
	{
		// Apply filter to scope
		auto vars = _FilterFn(scope);
		if (std::begin(vars) == std::end(vars))
			return DefaultAssignIf<AssignFn, MType>::Apply(_AssignFn, instance.*_pMember);

		// Try to assign for each variable found with filter
		bool result = false;

		for (auto pVariable : vars)
		{
			if (ObjectValueBinding::MemberBindings > 0)
			{
				// An object binding was supplied.
				// Member object binding requires EON::Object types.
				auto pObject = boost::get<Epic::EON::Object>(&pVariable->Value.Data);
				if (!pObject)
					continue;

				for (size_t i = 0; i < ObjectValueBinding::MemberBindings; ++i)
					if (_ObjBinding.Assign(*pObject, instance.*_pMember, i))
						result = true;
			}
			else
			{
				// Assign as a normal member variable via AssignVisitor
				auto vsAssign = AssignVisitor<MType, AssignFn>{ instance.*_pMember, _AssignFn };
				if (boost::apply_visitor(vsAssign, pVariable->Value.Data))
					result = true;
			}
		}

		return result;
	}
};

template<class CType, class MType, class FilterFn, class AssignFn, class... BTypes, class... Bindings>
struct Epic::EON::detail::ObjBnd<CType, Epic::EON::detail::MemBnd<CType, MType, FilterFn, AssignFn, BTypes...>, Bindings...>
	: public Epic::EON::detail::ObjBnd<CType, Bindings...>
{
	using ParentType = ObjBnd<CType, Bindings...>;
	using ResultType = CType;
	using Binding = MemBnd<CType, MType, FilterFn, AssignFn, BTypes...>;
	
	Binding _Binding;

	static constexpr size_t MemberBindings = 1 + ParentType::MemberBindings;

	ObjBnd(const Binding& binding, const Bindings&... bindings)
		: _Binding{ binding }, ParentType(bindings...) { }

	bool Assign(const Epic::EON::Object& scope, CType& instance, const size_t index) const
	{
		if (index > 0)
			return ParentType::Assign(scope, instance, index - 1);

		return _Binding.Assign(scope, instance);
	}
};

template<class CType, class MType, class FilterFn, class AssignFn, class... BTypes>
struct Epic::EON::detail::ObjBnd<CType, Epic::EON::detail::MemBnd<CType, MType, FilterFn, AssignFn, BTypes...>>
	: public Epic::EON::detail::ObjBnd<CType>
{
	using ParentType = ObjBnd<CType>;
	using ResultType = CType;
	using Binding = MemBnd<CType, MType, FilterFn, AssignFn, BTypes...>;

	Binding _Binding;

	static constexpr size_t MemberBindings = 1;
	
	ObjBnd(const Binding& binding)
		: _Binding{ binding }, ParentType() { }

	bool Assign(const Epic::EON::Object& scope, CType& instance, const size_t /*index*/) const
	{
		return _Binding.Assign(scope, instance);
	}
};

template<class CType>
struct Epic::EON::detail::ObjBnd<CType>
{
	using ResultType = CType;

	static constexpr size_t MemberBindings = 0;

	ObjBnd() = default;

	bool Assign(const Epic::EON::Object& /*scope*/, CType& /*instance*/, const size_t /*index*/) const
	{
		return false;
	}
};

//////////////////////////////////////////////////////////////////////////////

namespace Epic::EON
{
	template<class CType, class... MTypes>
	using ObjectBinding = detail::ObjBnd<CType, MTypes...>;

	template<class CType, class MType, class FilterFn, class AssignFn, class... Bindings>
	using MemberBinding = detail::MemBnd<CType, MType, FilterFn, AssignFn, Bindings...>;
}

//////////////////////////////////////////////////////////////////////////////

namespace Epic::EON
{
	namespace
	{
		// Bind an object
		template<class ClassType, class... MemberBindings>
		auto Bind(const MemberBindings&... bindings)
		{
			return ObjectBinding<ClassType, MemberBindings...>{ bindings... };
		}
	}
}

//////////////////////////////////////////////////////////////////////////////

namespace Epic::EON
{
	namespace
	{
		// TODO: Additional Includes that default the default value
		// TODO: Additional options for Selectors

		// Bind a named variable to an object member
		template<class ClassType, class MemberType>
		auto Require(const Epic::EON::Name& name, MemberType ClassType::*pMember)
		{
			using AssignFn = Epic::EON::detail::NoDefaultAssign<MemberType>;
			using Binding = MemberBinding<ClassType, MemberType, Epic::EON::HasName, AssignFn>;

			return Binding{ pMember, Epic::EON::HasName{ name }, AssignFn{ } };
		}

		// Bind a named variable to an object member with a custom assignment function
		template<class ClassType, class MemberType, class AssignFn>
		auto Require(const Epic::EON::Name& name, MemberType ClassType::*pMember, AssignFn assignFn)
		{
			using Binding = MemberBinding<ClassType, MemberType, HasName, AssignFn>;

			return Binding{ pMember, Epic::EON::HasName{ name }, assignFn };
		}

		// Bind a named variable to an object member
		template<class ClassType, class MemberType, class... BoundMemberTypes>
		auto Require(const Epic::EON::Name& name, MemberType ClassType::*pMember, const ObjectBinding<MemberType, BoundMemberTypes...>& bindings)
		{
			using AssignFn = Epic::EON::detail::NoDefaultAssign<MemberType>;
			using Binding = MemberBinding<ClassType, MemberType, HasName, AssignFn, BoundMemberTypes...>;

			return Binding{ pMember, Epic::EON::HasName{ name }, AssignFn{ }, bindings };
		}

		// Bind an optional named variable to an object member
		template<class ClassType, class DefaultType, class MemberType>
		auto Include(const Epic::EON::Name& name, const DefaultType& defaultValue, MemberType ClassType::*pMember)
		{
			using AssignFn = Epic::EON::detail::DefaultAssign<MemberType, DefaultType>;
			using Binding = MemberBinding<ClassType, MemberType, HasName, AssignFn>;

			return Binding{ pMember, Epic::EON::HasName{ name }, AssignFn{ defaultValue } };
		}

		// Bind an optional named variable to an object member
		template<class ClassType, class DefaultType, class MemberType, class... BoundMemberTypes>
		auto Include(const Epic::EON::Name& name, const DefaultType& defaultValue, MemberType ClassType::*pMember, const ObjectBinding<MemberType, BoundMemberTypes...>& bindings)
		{
			using AssignFn = Epic::EON::detail::DefaultAssign<MemberType, DefaultType>;
			using Binding = MemberBinding<ClassType, MemberType, HasName, AssignFn, BoundMemberTypes...>;

			return Binding{ pMember, Epic::EON::HasName{ name }, AssignFn{ defaultValue }, bindings };
		}
	}
}