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
#include <Epic/TMP/TypeTraits.hpp>
#include <boost/variant/apply_visitor.hpp>
#include <boost/variant/static_visitor.hpp>
#include <type_traits>
#include <vector>

//////////////////////////////////////////////////////////////////////////////

namespace Epic::EON::detail
{
	// NoDefaultAssign<DestType> - Assign a value with no default value
	//    Non-assigning initialization: Does nothing; Returns false
	//    Explicitly Convertible: Constructs a DestType from the source and assigns it; Returns true
	//    Implicitly Convertible: Static casts the source into DestType and assigns it; Returns true
	//    Non-Convertible: Delegates to Convert<SourceType, DestType>
	template<class DestType>
	struct NoDefaultAssign;

	// DefaultAssign<DestType> - Assign a value or assign a default value
	//    Non-assigning initialization: Assigns default value; Returns true
	//    Explicitly Convertible: Constructs a DestType from the source and assigns it; Returns true
	//    Implicitly Convertible: Static casts the source into DestType and assigns it; Returns true
	//    Non-Convertible: Attempts to delegate to Convert<SourceType, DestType>; Assigns default value if it fails
	template<class DestType, class DefaultType = DestType>
	struct DefaultAssign;

	// AssignIf<Function, SrcType, DestType> 
	//		Attempts to invoke user-supplied assignment function for src and dest types.
	//		The user-supplied assignment function should have the signature: bool (const SrcType&, DestType&)
	//		If the user-supplied assignment function cannot accept the arguments, NoDefaultAssign is used instead.
	template<class Function, class SrcType, class DestType,
		bool Enabled = Epic::TMP::IsCallable<Function(const SrcType&, DestType&), bool>::value>
	struct AssignIf;

	// NamedAssignIf<Function, SrcType, DestType>
	//		Attempts to invoke user-supplied assignment function for src and dest types.
	//		The user-supplied assignment function should have the signature: bool (const Epic::EON::Name&, const SrcType&, DestType&)
	//		If the user-supplied assignment function cannot accept the arguments, NoDefaultAssign is used instead.
	template<class Function, class SrcType, class DestType,
		bool Enabled = Epic::TMP::IsCallable<Function(const Epic::EON::Name&, const SrcType&, DestType&), bool>::value>
	struct NamedAssignIf;

	// DefaultAssignIf<Function, DestType>
	//		Attempts to invoke user-supplied assignment function for dest type (default assignment).
	//		The user-supplied assignment function should have the signature: bool (DestType&)
	//		If the user-supplied assignment function cannot accept the argument, NoDefaultAssign is used instead.
	template<class Function, class DestType, 
		bool Enabled = Epic::TMP::IsCallable<Function(DestType&), bool>::value>
	struct DefaultAssignIf;

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

// NoDefaultAssign<DestType>
template<class DestType>
struct Epic::EON::detail::NoDefaultAssign
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

	// Default Assign does nothing
	inline bool operator() (DestType&) const
	{
		return false;
	}

	// Named assign dispatches to the best method of assignment (ignoring name parameter)
	template<class SrcType>
	inline bool operator() (const Epic::EON::Name&, const SrcType& src, DestType& dest) const
	{
		return Assign(src, dest, SelectTag<SrcType>::Type());
	}

	// Assign dispatches to the best method of assignment
	template<class SrcType>
	bool operator() (const SrcType& src, DestType& dest) const
	{
		return Assign(src, dest, SelectTag<SrcType>::Type());
	}

private:
	template<class SrcType>
	bool Assign(const SrcType& src, DestType& dest, DirectAssignTag) const
	{
		dest = DestType{ src };
		return true;
	}

	template<class SrcType>
	bool Assign(const SrcType& src, DestType& dest, CastAssignTag) const
	{
		dest = static_cast<DestType>(src);
		return true;
	}

	template<class SrcType>
	bool Assign(const SrcType& src, DestType& dest, ConvertAssignTag) const
	{
		return Epic::EON::Convert<SrcType, DestType>{ } (src, dest);
	}
};

// NoDefaultAssign<std::vector<DestType>>
template<class DestType, class Alloc>
struct Epic::EON::detail::NoDefaultAssign<std::vector<DestType, Alloc>>
{
	using VectorType = std::vector<DestType, Alloc>;

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

	// Default Assign does nothing
	bool operator() (VectorType& dest) const
	{
		return false;
	}

	// Named assign dispatches to the best method of assignment (ignoring name parameter)
	template<class SrcType>
	bool operator() (const Epic::EON::Name&, const SrcType& src, VectorType& dest) const
	{
		return Assign(src, dest, SelectTag<SrcType>::Type());
	}

	// Assign dispatches to the best method of assignment
	template<class SrcType>
	bool operator() (const SrcType& src, VectorType& dest) const
	{
		return Assign(src, dest, SelectTag<SrcType>::Type());
	}

private:
	template<class SrcType>
	bool Assign(const SrcType& src, VectorType& dest, DirectAssignTag) const
	{
		dest.emplace_back(src);
		return true;
	}

	template<class SrcType>
	bool Assign(const SrcType& src, VectorType& dest, CastAssignTag) const
	{
		dest.emplace_back(static_cast<DestType>(src));
		return true;
	}

	template<class SrcType>
	bool Assign(const SrcType& src, VectorType& dest, ConvertAssignTag) const
	{
		DestType value;
		if (Epic::EON::Convert<SrcType, DestType>{ } (src, value))
		{
			dest.emplace_back(std::move(value));
			return true;
		}

		return false;
	}
};

// DefaultAssign<DestType, DefaultType>
template<class DestType, class DefaultType>
struct Epic::EON::detail::DefaultAssign
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

	DefaultAssign(const DefaultType& defaultValue)
		: _DefaultValue{ defaultValue } { }

	// Default Assign dispatches to the best method of assignment
	bool operator() (DestType& dest) const
	{
		return Assign(dest, SelectTag<DefaultType>::Type());
	}

	// Named assign dispatches to the best method of assignment (ignoring name parameter)
	template<class SrcType>
	bool operator() (const Epic::EON::Name&, const SrcType& src, DestType& dest) const
	{
		return Assign(src, dest, SelectTag<SrcType>::Type());
	}

	// Assign dispatches to the best method of assignment
	template<class SrcType>
	bool operator() (const SrcType& src, DestType& dest) const
	{
		return Assign(src, dest, SelectTag<SrcType>::Type());
	}

private:
	bool Assign(DestType& dest, DirectAssignTag) const
	{
		dest = _DefaultValue;
		return true;
	}

	bool Assign(DestType& dest, CastAssignTag) const
	{
		dest = static_cast<DestType>(_DefaultValue);
		return true;
	}

	bool Assign(DestType& dest, ConvertAssignTag) const
	{
		return Epic::EON::Convert<DefaultType, DestType>{ } (_DefaultValue, dest);
	}

	template<class SrcType>
	bool Assign(const SrcType& src, DestType& dest, DirectAssignTag) const
	{
		dest = src;
		return true;
	}

	template<class SrcType>
	bool Assign(const SrcType& src, DestType& dest, CastAssignTag) const
	{
		dest = static_cast<DestType>(src);
		return true;
	}

	template<class SrcType>
	bool Assign(const SrcType& src, DestType& dest, ConvertAssignTag) const
	{
		if (!Epic::EON::Convert<SrcType, DestType>{ } (src, dest))
			return Assign(dest, SelectTag<DefaultType>::Type());

		return true;
	}
};

// DefaultAssign<std::vector<DestType>>
template<class DestType, class Alloc>
struct Epic::EON::detail::DefaultAssign<std::vector<DestType, Alloc>>
{
	using VectorType = std::vector<DestType, Alloc>;

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

	const VectorType _DefaultValue;

	DefaultAssign(const VectorType& defaultValue)
		: _DefaultValue(defaultValue) { }

	// Default Assign will assign 'dest' to '_DefaultValue'
	bool operator() (VectorType& dest) const
	{
		dest = _DefaultValue;
		return true;
	}

	// Named assign dispatches to the best method of assignment (ignoring name parameter)
	template<class SrcType>
	bool operator() (const Epic::EON::Name&, const SrcType& src, VectorType& dest) const
	{
		return Assign(src, dest, SelectTag<SrcType>::Type());
	}

	// Assign dispatches to the best method of assignment
	template<class SrcType>
	bool operator() (const SrcType& src, VectorType& dest) const
	{
		return Assign(src, dest, SelectTag<SrcType>::Type());
	}

private:
	template<class SrcType>
	bool Assign(const SrcType& src, VectorType& dest, DirectAssignTag) const
	{
		dest.emplace_back(src);
		return true;
	}

	template<class SrcType>
	bool Assign(const SrcType& src, VectorType& dest, CastAssignTag) const
	{
		dest.emplace_back(static_cast<DestType>(src));
		return true;
	}

	template<class SrcType>
	bool Assign(const SrcType& src, VectorType& dest, ConvertAssignTag) const
	{
		DestType value;
		if (Epic::EON::Convert<SrcType, DestType>{ } (src, value))
		{
			dest.emplace_back(std::move(value));
			return true;
		}

		return false;
	}
};

//////////////////////////////////////////////////////////////////////////////

// AssignIf<false>
template<class Function, class SrcType, class DestType, bool Enabled>
struct Epic::EON::detail::AssignIf
{
	static inline bool Apply(Function, const SrcType& src, DestType& dest)
	{
		return NoDefaultAssign<DestType>() (src, dest);
	}
};

// AssignIf<true>
template<class Function, class SrcType, class DestType>
struct Epic::EON::detail::AssignIf<Function, SrcType, DestType, true>
{
	static inline bool Apply(Function assignFn, const SrcType& src, DestType& dest)
	{
		return assignFn(src, dest);
	}
};

// NamedAssignIf<false>
template<class Function, class SrcType, class DestType, bool Enabled>
struct Epic::EON::detail::NamedAssignIf
{
	static inline bool Apply(Function, const Epic::EON::Name&, const SrcType& src, DestType& dest)
	{
		return NoDefaultAssign<DestType>() (src, dest);
	}
};

// NamedAssignIf<true>
template<class Function, class SrcType, class DestType>
struct Epic::EON::detail::NamedAssignIf<Function, SrcType, DestType, true>
{
	static inline bool Apply(Function assignFn, const Epic::EON::Name& name, const SrcType& src, DestType& dest)
	{
		return assignFn(name, src, dest);
	}
};

// DefaultAssignIf<false>
template<class Function, class DestType, bool Enabled>
struct Epic::EON::detail::DefaultAssignIf
{
	static inline bool Apply(Function, DestType& dest)
	{
		return NoDefaultAssign<DestType>() (dest);
	}
};

// DefaultAssignIf<true>
template<class Function, class DestType>
struct Epic::EON::detail::DefaultAssignIf<Function, DestType, true>
{
	static inline bool Apply(Function assignFn, DestType& dest)
	{
		return assignFn(dest);
	}
};

///////////////////////////////////////////////////

template<class DestType, class Function>
struct Epic::EON::detail::ObjectAssignVisitor : public boost::static_visitor<bool>
{
	// NOTE: These parameters are stored internally rather than by passing via
	//		 std::bind to help prevent warning C4503

	DestType& _Dest;
	Function _AssignFn;
	const Epic::EON::Name& _Name;

	ObjectAssignVisitor(DestType& dest, Function assignFn, const Epic::EON::Name& name)
		: _Dest{ dest }, _AssignFn{ assignFn }, _Name{ name } { }

	template<class SrcType>
	bool operator() (const SrcType& src) const
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
	Function _AssignFn;

	ArrayAssignVisitor(DestType& dest, Function assignFn)
		: _Dest{ dest }, _AssignFn{ assignFn } { }

	template<class SrcType>
	bool operator() (const SrcType& src) const
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
	Function _AssignFn;

	AssignVisitor(DestType& dest, Function assignFn)
		: _Dest{ dest }, _AssignFn{ assignFn } { }

	template<class SrcType>
	bool operator() (const SrcType& src) const
	{
		return AssignIf<Function, SrcType, DestType>::Apply(_AssignFn, src, _Dest);
	}

	bool operator() (const Epic::EON::Object& src) const
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

	bool operator() (const Epic::EON::Array& src) const
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
