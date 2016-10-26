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

#include <Epic/EON/Convert.hpp>
#include <Epic/EON/Types.hpp>
#include <Epic/STL/Vector.hpp>
#include <Epic/TMP/TypeTraits.hpp>
#include <boost/variant/apply_visitor.hpp>
#include <boost/variant/static_visitor.hpp>
#include <type_traits>

//////////////////////////////////////////////////////////////////////////////

namespace Epic::EON::detail
{
	// Assign<DestType> - Assigns a value
	//    Explicitly Convertible: Constructs a DestType from the source and assigns it; Returns true
	//    Implicitly Convertible: Static casts the source into DestType and assigns it; Returns true
	//    Non-Convertible: Delegates to Convert<SourceType, DestType>
	template<class DestType>
	struct Assign;

	// SafeCast<From, To> - Cast a From value to a To value
	template<class From, class To>
	struct SafeCast;

	// Default<DestType, DefaultType> - Assigns a default value if DefaultType != void
	template<class DestType, class DefaultType = DestType>
	struct Default;

	// DefaultFail - Assigns no default value and returns failure
	struct DefaultFail;

	// AssignIf<Function, SrcType, DestType> 
	//		Attempts to invoke user-supplied assignment function for src and dest types.
	//		The user-supplied assignment function should have the signature: bool (const SrcType&, DestType&)
	//		If the user-supplied assignment function cannot accept the arguments, Assign is used.
	template<class Function, class SrcType, class DestType,
		bool Enabled = Epic::TMP::IsCallable<Function(const SrcType&, DestType&), bool>::value>
	struct AssignIf;

	// NamedAssignIf<Function, SrcType, DestType>
	//		Attempts to invoke user-supplied assignment function for src and dest types.
	//		The user-supplied assignment function should have the signature: bool (const Epic::EON::Name&, const SrcType&, DestType&)
	//		If the user-supplied assignment function cannot accept the arguments, Assign is used (dropping the Name argument).
	template<class Function, class SrcType, class DestType,
		bool Enabled = Epic::TMP::IsCallable<Function(const Epic::EON::Name&, const SrcType&, DestType&), bool>::value>
	struct NamedAssignIf;

	// DefaultIf<Function, DestType>
	//		Attempts to invoke user-supplied default assignment function for dest type.
	//		The user-supplied assignment function should have the signature: bool (DestType&)
	//		If the user-supplied default assignment function cannot accept the argument, Default<DestType, void> is used instead.
	template<class Function, class DestType, 
		bool Enabled = Epic::TMP::IsCallable<Function(DestType&), bool>::value>
	struct DefaultIf;

	// ObjectAssignVisitor<DestType, Function>
	//		Visits an EON::Variant variant to be used as the source type for an invoked assignment.
	//		This visitor is invoked by AssignVisitor when an EON::Object type is being assigned.
	template<class DestType, class Function>
	struct ObjectAssignVisitor;

	// ArrayAssignVisitor<DestType, Function>
	//		Visits an EON::Variant variant to be used as the source type for an invoked assignment.
	//		This visitor is invoked by AssignVisitor when an EON::Array type is being assigned.
	template<class DestType, class Function>
	struct ArrayAssignVisitor;

	// AssignVisitor<DestType, Function>
	//		Visits an EON::Variant variant to be used as the source type for an invoked assignment.
	template<class DestType, class Function>
	struct AssignVisitor;
}

//////////////////////////////////////////////////////////////////////////////

template<class From, class To>
struct Epic::EON::detail::SafeCast
{
	static inline auto Apply(const From& from)
	{
		return std::move(static_cast<To>(from));
	}
};

template<>
struct Epic::EON::detail::SafeCast<Epic::EON::Float, bool>
{
	static inline auto Apply(const Epic::EON::Float& from)
	{
		return from.Value != 0;
	}
};

template<>
struct Epic::EON::detail::SafeCast<Epic::EON::Integer, bool>
{
	static inline auto Apply(const Epic::EON::Integer& from)
	{
		return from.Value != 0;
	}
};

//////////////////////////////////////////////////////////////////////////////

// Assign<DestType>
template<class DestType>
struct Epic::EON::detail::Assign
{
	struct DirectAssignTag { };
	struct CastAssignTag { };
	struct ConvertAssignTag { };

	template<class SrcType>
	struct SelectTag
	{
		using Type =
			std::conditional_t<
				!std::is_convertible<SrcType, DestType>::value,
				ConvertAssignTag,
				std::conditional_t<
					Epic::TMP::IsExplicitlyConvertible<SrcType, DestType>::value,
					DirectAssignTag,
					CastAssignTag>
			>;
	};

	// Named assign dispatches to the best method of assignment (dropping Name argument)
	template<class SrcType>
	inline bool operator() (const Epic::EON::Name&, const SrcType& src, DestType& dest) const
	{
		return _Assign(src, dest, SelectTag<SrcType>::Type());
	}

	// Assign dispatches to the best method of assignment
	template<class SrcType>
	inline bool operator() (const SrcType& src, DestType& dest) const
	{
		return _Assign(src, dest, SelectTag<SrcType>::Type());
	}

private:
	template<class SrcType>
	inline bool _Assign(const SrcType& src, DestType& dest, DirectAssignTag) const
	{
		dest = DestType{ src };
		return true;
	}

	template<class SrcType>
	inline bool _Assign(const SrcType& src, DestType& dest, CastAssignTag) const
	{
		dest = SafeCast<SrcType, DestType>::Apply(src);
		return true;
	}

	template<class SrcType>
	inline bool _Assign(const SrcType& src, DestType& dest, ConvertAssignTag) const
	{
		return Epic::EON::Convert<SrcType, DestType>::Apply(src, dest);
	}
};

// Assign<Container<DestType, Alloc>>
template<class DestType, class Alloc, template<class, class> class Container>
struct Epic::EON::detail::Assign<Container<DestType, Alloc>>
{
	using ContainerType = Container<DestType, Alloc>;

	struct DirectAssignTag { };
	struct CastAssignTag { };
	struct ConvertAssignTag { };

	template<class SrcType>
	struct SelectTag
	{
		using Type =
			std::conditional_t<
				!std::is_convertible<SrcType, DestType>::value,
				ConvertAssignTag,
					std::conditional_t<
					Epic::TMP::IsExplicitlyConvertible<SrcType, DestType>::value,
					DirectAssignTag,
					CastAssignTag>
			>;
	};

	// Named assign dispatches to the best method of assignment (dropping Name argument)
	template<class SrcType>
	inline bool operator() (const Epic::EON::Name&, const SrcType& src, ContainerType& dest) const
	{
		return _Assign(src, dest, SelectTag<SrcType>::Type());
	}

	// Assign dispatches to the best method of assignment
	template<class SrcType>
	inline bool operator() (const SrcType& src, ContainerType& dest) const
	{
		return _Assign(src, dest, SelectTag<SrcType>::Type());
	}

private:
	template<class SrcType>
	inline bool _Assign(const SrcType& src, ContainerType& dest, DirectAssignTag) const
	{
		dest.emplace_back(src);
		return true;
	}

	template<class SrcType>
	inline bool _Assign(const SrcType& src, ContainerType& dest, CastAssignTag) const
	{
		dest.emplace_back(SafeCast<SrcType, DestType>::Apply(src));
		return true;
	}

	template<class SrcType>
	inline bool _Assign(const SrcType& src, ContainerType& dest, ConvertAssignTag) const
	{
		DestType value;
		if (Epic::EON::Convert<SrcType, DestType>::Apply(src, value))
		{
			dest.emplace_back(std::move(value));
			return true;
		}

		return false;
	}
};

// Assign<SmallVector<DestType, N, Alloc>>
// Explicit support for Epic::SmallVector
template<class DestType, size_t N, class Alloc>
struct Epic::EON::detail::Assign<boost::container::small_vector<DestType, N, Alloc>>
{
	using ContainerType = boost::container::small_vector_base<DestType, Alloc>;

	// Named assign delegates to Assign<Container>
	template<class SrcType>
	inline bool operator() (const Epic::EON::Name& name, const SrcType& src, ContainerType& dest) const
	{
		return Assign<ContainerType>() (name, src, dest);
	}

	// Assign delegates to Assign<Container>
	template<class SrcType>
	inline bool operator() (const SrcType& src, ContainerType& dest) const
	{
		return Assign<ContainerType>() (src, dest);
	}
};

//////////////////////////////////////////////////////////////////////////////

// Default<DestType, DefaultType>
template<class DestType, class DefaultType>
struct Epic::EON::detail::Default
{
	struct DirectAssignTag { };
	struct CastAssignTag { };
	struct ConvertAssignTag { };

	template<class SrcType>
	struct SelectTag
	{
		using Type =
			std::conditional_t<
				!std::is_convertible<SrcType, DestType>::value,
				ConvertAssignTag,
				std::conditional_t<
					Epic::TMP::IsExplicitlyConvertible<SrcType, DestType>::value,
					DirectAssignTag,
					CastAssignTag>
			>;
	};

	const DefaultType _DefaultValue;

	explicit Default(const DefaultType& defaultValue)
		: _DefaultValue{ defaultValue } { }

	// Default dispatches to the best method of assignment
	inline bool operator() (DestType& dest) const
	{
		return _Assign(dest, SelectTag<DefaultType>::Type());
	}

private:
	inline bool _Assign(DestType& dest, DirectAssignTag) const
	{
		dest = _DefaultValue;
		return true;
	}

	inline bool _Assign(DestType& dest, CastAssignTag) const
	{
		dest = SafeCast<DefaultType, DestType>::Apply(_DefaultValue);
		return true;
	}

	inline bool _Assign(DestType& dest, ConvertAssignTag) const
	{
		return Epic::EON::Convert<DefaultType, DestType>::Apply(_DefaultValue, dest);
	}
};

// Default<DestType, void>
template<class DestType>
struct Epic::EON::detail::Default<DestType, void>
{
	inline bool operator() (DestType&) const
	{
		return true;
	}
};

// DefaultFail
struct Epic::EON::detail::DefaultFail
{
	template<class DestType>
	inline bool operator() (DestType&) const
	{
		return false;
	}
};

//////////////////////////////////////////////////////////////////////////////

namespace Epic::EON::detail
{
	template<class DestType>
	using NoDefault = Default<DestType, void>;
}

//////////////////////////////////////////////////////////////////////////////

// AssignIf<false>
template<class Function, class SrcType, class DestType, bool Enabled>
struct Epic::EON::detail::AssignIf
{
	static inline bool Apply(Function&, const SrcType& src, DestType& dest)
	{
		// User-supplied assignment function has no function with signature:
		// bool (const SrcType& src, DestType& dest)
		// Default to using Assign::operator()

		return Assign<DestType>() (src, dest);
	}
};

// AssignIf<true>
template<class Function, class SrcType, class DestType>
struct Epic::EON::detail::AssignIf<Function, SrcType, DestType, true>
{
	static inline bool Apply(Function& assignFn, const SrcType& src, DestType& dest)
	{
		// User-supplied assignment function works for these arguments
		return assignFn(src, dest);
	}
};

// NamedAssignIf<false>
template<class Function, class SrcType, class DestType, bool Enabled>
struct Epic::EON::detail::NamedAssignIf
{
	static inline bool Apply(Function& assignFn, const Epic::EON::Name&, const SrcType& src, DestType& dest)
	{
		// User-supplied assignment function has no function with signature:
		// bool (const EON::Name&, const SrcType& src, DestType& dest)
		// Default will try dropping the Name argument

		return AssignIf<Function, SrcType, DestType>::Apply(assignFn, src, dest);
	}
};

// NamedAssignIf<true>
template<class Function, class SrcType, class DestType>
struct Epic::EON::detail::NamedAssignIf<Function, SrcType, DestType, true>
{
	static inline bool Apply(Function& assignFn, const Epic::EON::Name& name, const SrcType& src, DestType& dest)
	{
		// User-supplied assignment function will work for these arguments
		return assignFn(name, src, dest);
	}
};

// DefaultIf<false>
template<class Function, class DestType, bool Enabled>
struct Epic::EON::detail::DefaultIf
{
	static inline bool Apply(Function&, DestType& dest)
	{
		// User-supplied default assignment function has no function with signature:
		// bool (DestType& dest)
		// Default to using Default<DestType, void>::operator()

		return Default<DestType, void>() (dest);
	}
};

// DefaultIf<true>
template<class Function, class DestType>
struct Epic::EON::detail::DefaultIf<Function, DestType, true>
{
	static inline bool Apply(Function& defaultFn, DestType& dest)
	{
		// User-supplied default assignment function will work with this argument
		return defaultFn(dest);
	}
};

//////////////////////////////////////////////////////////////////////////////

template<class DestType, class Function>
struct Epic::EON::detail::ObjectAssignVisitor : public boost::static_visitor<bool>
{
	// NOTE: These parameters are stored internally rather than by passing via
	//		 std::bind to help prevent warning C4503

	DestType& _Dest;
	Function& _AssignFn;
	const Epic::EON::Name& _Name;

	ObjectAssignVisitor(DestType& dest, Function& assignFn, const Epic::EON::Name& name)
		: _Dest{ dest }, _AssignFn{ assignFn }, _Name{ name } { }

	template<class SrcType>
	inline bool operator() (const SrcType& src) const
	{
		return NamedAssignIf<Function, SrcType, DestType>::Apply(_AssignFn, _Name, src, _Dest);
	}
};

template<class DestType, class Function>
struct Epic::EON::detail::ArrayAssignVisitor : public boost::static_visitor<bool>
{
	// NOTE: These parameters are stored internally rather than by passing via
	//		 std::bind to help prevent warning C4503

	DestType& _Dest;
	Function& _AssignFn;

	ArrayAssignVisitor(DestType& dest, Function& assignFn)
		: _Dest{ dest }, _AssignFn{ assignFn } { }

	template<class SrcType>
	inline bool operator() (const SrcType& src) const
	{
		return AssignIf<Function, SrcType, DestType>::Apply(_AssignFn, src, _Dest);
	}
};

template<class DestType, class Function>
struct Epic::EON::detail::AssignVisitor : public boost::static_visitor<bool>
{
	// NOTE: These parameters are stored internally rather than by passing via
	//		 std::bind to help prevent warning C4503

	DestType& _Dest;
	Function& _AssignFn;

	AssignVisitor(DestType& dest, Function& assignFn)
		: _Dest{ dest }, _AssignFn{ assignFn } { }

	template<class SrcType>
	inline bool operator() (const SrcType& src) const
	{
		return AssignIf<Function, SrcType, DestType>::Apply(_AssignFn, src, _Dest);
	}

	inline bool operator() (const Epic::EON::Object& src) const
	{
		bool assigned = src.Members.empty();

		for (auto& var : src.Members)
		{
			auto vsAssign = ObjectAssignVisitor<DestType, Function>{ _Dest, _AssignFn, var.Name };
			if (boost::apply_visitor(vsAssign, var.Value.Data))
				assigned = true;
		}

		return assigned;
	}

	inline bool operator() (const Epic::EON::Array& src) const
	{
		bool assigned = src.Members.empty();

		// Assign each array value individually
		for (auto& var : src.Members)
		{
			auto vsAssign = ArrayAssignVisitor<DestType, Function>{ _Dest, _AssignFn };
			if (boost::apply_visitor(vsAssign, var.Data))
				assigned = true;
		}

		return assigned;
	}
};
