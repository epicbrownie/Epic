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
#include <Epic/EON/Convert.hpp>
#include <Epic/EON/Error.hpp>
#include <Epic/EON/detail/Traits.hpp>
#include <algorithm>
#include <iterator>
#include <type_traits>
#include <variant>

//////////////////////////////////////////////////////////////////////////////

namespace Epic::EON::detail
{
	template<class T, class Converter>
	class ConversionVisitor;

	class InheritVisitor;
	class MergeVisitor;
	class FilterVisitor;
	class TypeNameVisitor;
}

//////////////////////////////////////////////////////////////////////////////

// ConversionVisitor
template<class T, class Converter>
class Epic::EON::detail::ConversionVisitor
{
private:
	T& m_To;
	Converter m_ConvertFn;
	const EONObject& m_GlobalScope;

public:
	ConversionVisitor() = delete;
	ConversionVisitor(T& to, Converter convertFn, const EONObject& scope)
		: m_To(to), m_ConvertFn(convertFn), m_GlobalScope(scope) { }

private:
	using Traits = EONTraits<T>;

	struct FailTag { };
	struct ScalarTag{ };
	struct ArrayTag{ };
	struct SetTag{ };
	struct MapTag{ };
	
	using MakeArrayTag = 
		std::conditional_t<Traits::IsVectorLike || Traits::IsSetLike, ArrayTag, 
		std::conditional_t<Traits::IsIndexableScalar, ScalarTag,
		FailTag>>;

	using MakeObjectTag = std::conditional_t<Traits::IsMapLike, MapTag, FailTag>;
	
	using MakeScalarTag =
		std::conditional_t<!Traits::IsContainer, ScalarTag,
		std::conditional_t<Traits::IsVectorLike, ArrayTag,
		std::conditional_t<Traits::IsSetLike, SetTag,
		FailTag>>>;

private:
	template<class S>
	bool DoScalarConversion(const S&, FailTag) { return false; }

	template<class S> 
	bool DoScalarConversion(const S& v, ScalarTag)
	{
		return ConvertIf(m_ConvertFn, m_To, v);
	}

	template<class S>
	bool DoScalarConversion(const S& v, ArrayTag)
	{
		using Item = typename T::value_type;
		static_assert(std::is_default_constructible_v<Item>, "Item must be default constructible");

		Item item;

		if (!ConvertIf(m_ConvertFn, item, v))
			return false;

		m_To.emplace_back(std::move(item));
		
		return true;
	}

	template<class S>
	bool DoScalarConversion(const S& v, SetTag)
	{
		using Item = typename T::key_type;
		static_assert(std::is_default_constructible_v<Item>, "Item must be default constructible");

		Item item;

		if (!ConvertIf(m_ConvertFn, item, v))
			return false;

		m_To.emplace(std::move(item));

		return true;
	}

	bool DoArrayConversion(const EONArray&, FailTag) { return false; }

	bool DoArrayConversion(const EONArray& v, ScalarTag)
	{
		static_assert(std::is_default_constructible_v<T>, "Container must be default constructible");

		T item;

		for (decltype(v.Members.size()) i=0; i<v.Members.size(); ++i)
		{
			if (i >= std::size(item))
				break;

			const auto& vm = v.Members[i];
			typename T::value_type value;

			if (!std::visit(ConversionVisitor<typename T::value_type, Converter>(value, m_ConvertFn, m_GlobalScope), vm.Data))
				return false;

			item[i] = value;
		}

		m_To = std::move(item);

		return true;
	}

	bool DoArrayConversion(const EONArray& v, ArrayTag)
	{
		static_assert(std::is_default_constructible_v<T>, "Container must be default constructible");

		T items;

		for (const auto& vm : v.Members)
		{
			if (!std::visit(ConversionVisitor<T, Converter>(items, m_ConvertFn, m_GlobalScope), vm.Data))
				return false;
		}

		m_To = std::move(items);

		return true;
	}

	bool DoObjectConversion(const EONObject&, FailTag) { return false; }

	bool DoObjectConversion(const EONObject& v, MapTag)
	{
		static_assert(std::is_default_constructible_v<T>, "Container must be default constructible");
		static_assert(std::is_default_constructible_v<typename T::key_type>, "K must be default constructible");
		static_assert(std::is_default_constructible_v<typename T::mapped_type>, "V must be default constructible");

		T items;

		for (const auto& vm : v.Members)
		{
			typename T::key_type key;
			if (!ConvertIf(m_ConvertFn, key, vm.Name))
				return false;

			typename T::mapped_type value;
			if (!std::visit(ConversionVisitor<typename T::mapped_type, Converter>(value, m_ConvertFn, m_GlobalScope), vm.Value.Data))
				return false;

			items.emplace(std::move(key), std::move(value));
		}

		m_To = std::move(items);

		return true;
	}

public:
	template<class Scalar>
	bool operator () (const Scalar& v)
	{
		return DoScalarConversion(v.Value, MakeScalarTag());
	}

	bool operator () (const EONArray& v)
	{
		return DoArrayConversion(v, MakeArrayTag());
	}

	bool operator () (const EONObject& v)
	{
		return DoObjectConversion(v, MakeObjectTag());
	}
};

//////////////////////////////////////////////////////////////////////////////

// InheritVisitor
class Epic::EON::detail::InheritVisitor
{
public:
	InheritVisitor() = default;

private:
	template<class From, class To>
	void Inherit(To&, const From&) const
	{
		throw InvalidValueException("Incompatible types.");
	}
	
	template<class Type>
	void Inherit(Type& to, const Type& from) const
	{
		to.Value = from.Value;
	}

	void Inherit(EONObject& to, const EONObject& from) const
	{
		std::copy(std::begin(from.Members),
				  std::end(from.Members),
				  std::inserter(to.Members, std::begin(to.Members)));
	}

	void Inherit(EONArray& to, const EONArray& from) const
	{
		std::copy(std::begin(from.Members),
				  std::end(from.Members),
				  std::inserter(to.Members, std::begin(to.Members)));
	};

	void Inherit(EONString& to, const EONString& from) const
	{
		to.Value.insert(0, from.Value);
	}

public:
	template<class From, class To>
	void operator() (To& to, const From& from) const
	{
		Inherit(to, from);
	}
};

//////////////////////////////////////////////////////////////////////////////

// MergeVisitor
class Epic::EON::detail::MergeVisitor
{
private:
	EONVariant& _To;
	const EONVariant& _From;

public:
	MergeVisitor(EONVariant& to, const EONVariant& from)
		: _From{ from }, _To{ to } { }

private:
	template<class From, class To>
	void Merge(To&, const From&)
	{
		_To = _From;
	}

	void Merge(EONObject& to, const EONObject& from)
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

					MergeVisitor vsMerger(vto.Value, vfrom.Value);
					std::visit(vsMerger, vto.Value.Data, vfrom.Value.Data);

					break;
				}
			}

			if (!isDuplicate)
				to.Members.emplace_back(vfrom);
		}
	};

public:
	template<class From, class To>
	void operator() (To& to, const From& from)
	{
		Merge<From, To>(to, from);
	}
};

//////////////////////////////////////////////////////////////////////////////

// FilterVisitor
class Epic::EON::detail::FilterVisitor
{
private:
	std::size_t _Filter;

public:
	explicit FilterVisitor(eEONVariantType filter) noexcept
		: _Filter(std::size_t(filter)) { }

public:	
	bool operator() (const EONInteger& v) { return (_Filter & std::size_t(eEONVariantType::Integer)) != 0; }
	bool operator() (const EONFloat& v) { return (_Filter & std::size_t(eEONVariantType::Float)) != 0; }
	bool operator() (const EONBoolean& v) { return (_Filter & std::size_t(eEONVariantType::Boolean)) != 0; }
	bool operator() (const EONString& v) { return (_Filter & std::size_t(eEONVariantType::String)) != 0; }
	bool operator() (const EONArray& v) { return (_Filter & std::size_t(eEONVariantType::Array)) != 0; }
	bool operator() (const EONObject& v) { return (_Filter & std::size_t(eEONVariantType::Object)) != 0; }
};

//////////////////////////////////////////////////////////////////////////////

// TypeNameVisitor
class Epic::EON::detail::TypeNameVisitor
{
public:
	STLString<char> operator() (const EONInteger& v) { return "INTEGER"; }
	STLString<char> operator() (const EONFloat& v) { return "FLOAT"; }
	STLString<char> operator() (const EONBoolean& v) { return "BOOLEAN"; }
	STLString<char> operator() (const EONString& v) { return "STRING"; }
	STLString<char> operator() (const EONArray& v) { return "ARRAY"; }
	STLString<char> operator() (const EONObject& v) { return "OBJECT"; }
};
