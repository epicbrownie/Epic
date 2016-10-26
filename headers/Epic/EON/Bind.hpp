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

	template<class CType, class MType, class DefaultFn, class AssignFn, class... Bindings>
	struct MemBnd;
}

//////////////////////////////////////////////////////////////////////////////

template<class CType, class MType, class DefaultFn, class AssignFn, class... Bindings>
struct Epic::EON::detail::MemBnd
{
	typedef MType CType::*Pointer;
	using ObjectValueBinding = ObjBnd<MType, Bindings...>;

	const Pointer _pMember;
	const ObjectValueBinding _ObjBinding;
	const Epic::EON::NameHash _Name;
	mutable DefaultFn _DefaultFn;
	mutable AssignFn _AssignFn;

	MemBnd(Pointer pMember, const Epic::EON::NameHash& name, DefaultFn defaultFn, AssignFn assignFn)
		: _pMember(pMember), _Name{ name }, _DefaultFn(defaultFn), _AssignFn(assignFn), _ObjBinding() { }

	MemBnd(Pointer pMember, const Epic::EON::NameHash& name, DefaultFn defaultFn, AssignFn assignFn, const ObjectValueBinding& objBinding)
		: _pMember(pMember), _Name{ name }, _DefaultFn(defaultFn), _AssignFn(assignFn), _ObjBinding(objBinding) { }

	bool Assign(const Epic::EON::Object& scope, CType& instance) const
	{
		// Apply filter to scope
		auto var = Epic::EON::HasName(_Name) (scope);
		if (!var)
			return DefaultIf<DefaultFn, MType>::Apply(_DefaultFn, instance.*_pMember);

		// Try to assign
		bool result = true;

		if (ObjectValueBinding::MemberBindings > 0)
		{
			// An object binding was supplied.
			// Member object binding requires EON::Object types.
			auto pObject = boost::get<Epic::EON::Object>(&var->Value.Data);
			if (!pObject)
				return false;

			for (size_t i = 0; i < ObjectValueBinding::MemberBindings; ++i)
			{
				if (!_ObjBinding.Assign(*pObject, instance.*_pMember, i))
				{
					result = false;
					break;
				}
			}
		}
		else
		{
			// Assign as a normal member variable via AssignVisitor
			auto vsAssign = AssignVisitor<MType, AssignFn>{ instance.*_pMember, _AssignFn };
			result = boost::apply_visitor(vsAssign, var->Value.Data);
		}

		return result;
	}
};

template<class CType, class MType, class DefaultFn, class AssignFn, class... BTypes, class... Bindings>
struct Epic::EON::detail::ObjBnd<CType, Epic::EON::detail::MemBnd<CType, MType, DefaultFn, AssignFn, BTypes...>, Bindings...>
	: public Epic::EON::detail::ObjBnd<CType, Bindings...>
{
	using ParentType = ObjBnd<CType, Bindings...>;
	using ResultType = CType;
	using Binding = MemBnd<CType, MType, DefaultFn, AssignFn, BTypes...>;
	
	Binding _Binding;

	static constexpr size_t MemberBindings = 1 + ParentType::MemberBindings;

	explicit ObjBnd(const Binding& binding, const Bindings&... bindings)
		: _Binding{ binding }, ParentType(bindings...) { }

	bool Assign(const Epic::EON::Object& scope, CType& instance, const size_t index) const
	{
		if (index > 0)
			return ParentType::Assign(scope, instance, index - 1);

		return _Binding.Assign(scope, instance);
	}
};

template<class CType, class MType, class DefaultFn, class AssignFn, class... BTypes>
struct Epic::EON::detail::ObjBnd<CType, Epic::EON::detail::MemBnd<CType, MType, DefaultFn, AssignFn, BTypes...>>
	: public Epic::EON::detail::ObjBnd<CType>
{
	using ParentType = ObjBnd<CType>;
	using ResultType = CType;
	using Binding = MemBnd<CType, MType, DefaultFn, AssignFn, BTypes...>;

	Binding _Binding;

	static constexpr size_t MemberBindings = 1;
	
	explicit ObjBnd(const Binding& binding)
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

	template<class CType, class MType, class DefaultFn, class AssignFn, class... Bindings>
	using MemberBinding = detail::MemBnd<CType, MType, DefaultFn, AssignFn, Bindings...>;
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
		// Bind a named variable to an object member
		template<class ClassType, class MemberType, class AssignFn = Epic::EON::detail::Assign<MemberType>>
		auto Require(const Epic::EON::NameHash& name, MemberType ClassType::*pMember, AssignFn assignFn = AssignFn())
		{
			using DefaultFn = Epic::EON::detail::DefaultFail;
			using Binding = MemberBinding<ClassType, MemberType, DefaultFn, AssignFn>;

			return Binding{ pMember, name, DefaultFn(), assignFn };
		}

		// Bind a named variable to an object member
		template<class ClassType, class MemberType, class AssignFn = Epic::EON::detail::Assign<MemberType>, class... BoundMemberTypes>
		auto Require(const Epic::EON::NameHash& name, MemberType ClassType::*pMember,
			const ObjectBinding<MemberType, BoundMemberTypes...>& bindings, AssignFn assignFn = AssignFn())
		{
			using DefaultFn = Epic::EON::detail::DefaultFail;
			using Binding = MemberBinding<ClassType, MemberType, DefaultFn, AssignFn, BoundMemberTypes...>;

			return Binding{ pMember, name, DefaultFn(), assignFn, bindings };
		}
	}

	namespace
	{
		// Bind an optional named variable to an object member (without default)
		template<class ClassType, class MemberType, class AssignFn = Epic::EON::detail::Assign<MemberType>>
		auto Include(const Epic::EON::NameHash& name, MemberType ClassType::*pMember, 
			AssignFn assignFn = AssignFn())
		{
			using DefaultFn = Epic::EON::detail::NoDefault<MemberType>;
			using Binding = MemberBinding<ClassType, MemberType, DefaultFn, AssignFn>;

			return Binding{ pMember, name, DefaultFn(), assignFn };
		}

		// Bind an optional named variable to an object member (without default)
		template<class ClassType, class MemberType, class AssignFn = Epic::EON::detail::Assign<MemberType>, class... BoundMemberTypes>
		auto Include(const Epic::EON::NameHash& name, MemberType ClassType::*pMember, const ObjectBinding<MemberType, BoundMemberTypes...>& bindings, 
			AssignFn assignFn = AssignFn())
		{
			using DefaultFn = Epic::EON::detail::NoDefault<MemberType>;
			using Binding = MemberBinding<ClassType, MemberType, DefaultFn, AssignFn, BoundMemberTypes...>;

			return Binding{ pMember, name, DefaultFn(), assignFn, bindings };
		}

		// Bind an optional named variable to an object member (with default)
		template<class ClassType, class DefaultType, class MemberType, class AssignFn = Epic::EON::detail::Assign<MemberType>>
		auto Include(const Epic::EON::NameHash& name, const DefaultType& defaultValue, MemberType ClassType::*pMember, 
			AssignFn assignFn = AssignFn())
		{
			using DefaultFn = Epic::EON::detail::Default<MemberType, DefaultType>;
			using Binding = MemberBinding<ClassType, MemberType, DefaultFn, AssignFn>;

			return Binding{ pMember, name, DefaultFn{ defaultValue }, assignFn };
		}

		// Bind an optional named variable to an object member (with default)
		template<class ClassType, class DefaultType, class MemberType, class AssignFn = Epic::EON::detail::Assign<MemberType>, class... BoundMemberTypes>
		auto Include(const Epic::EON::NameHash& name, const DefaultType& defaultValue, MemberType ClassType::*pMember, 
			const ObjectBinding<MemberType, BoundMemberTypes...>& bindings, 
			AssignFn assignFn = AssignFn())
		{
			using DefaultFn = Epic::EON::detail::Default<MemberType, DefaultType>;
			using Binding = MemberBinding<ClassType, MemberType, DefaultFn, AssignFn, BoundMemberTypes...>;

			return Binding{ pMember, name, DefaultFn{ defaultValue }, assignFn, bindings };
		}
	}
}