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

#include <Epic/Math/detail/VectorFwd.hpp>
#include <Epic/Math/detail/VectorHelpers.hpp>
#include <Epic/Math/detail/MathHelpers.hpp>
#include <Epic/Math/Swizzler.hpp>
#include <cstddef>

//////////////////////////////////////////////////////////////////////////////

// SVector
template<class T, std::size_t S>
class Epic::SVector : public Epic::detail::SVectorHelper<T, S>::BaseType
{
public:
	using Base = typename Epic::detail::SVectorHelper<T, S>::BaseType;
	using Type = Epic::SVector<T, S>;

public:
	using ValueType = T;
	constexpr static std::size_t Size = S;

	using VectorType = Epic::Vector<T, S>;

private:
	using Base::Values;

public:
	SVector() noexcept = delete;
	SVector(const Type&) noexcept = delete;
	Type& operator = (const Type&) = delete;

public:
	// Explicitly converts to vector
	VectorType& ToVector() noexcept
	{
		return reinterpret_cast<VectorType&>(*this);
	}

	// Explicitly converts to vector
	const VectorType& ToVector() const noexcept
	{
		return reinterpret_cast<VectorType&>(*this);
	}

	// Explicitly converts to Vector
	VectorType& operator() () noexcept
	{
		return ToVector();
	}

	// Explicitly converts to Vector
	const VectorType& operator() () const noexcept
	{
		return ToVector();
	}

	// Implicitly converts to Vector
	operator VectorType& () noexcept
	{
		return ToVector();
	}

	// Implicitly converts to Vector
	operator const VectorType& () const noexcept
	{
		return ToVector();
	}

public:
	const T& operator[] (std::size_t index) const noexcept
	{
		assert(index >= 0 && index < Size);
		return Values[index];
	}
};

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	namespace
	{
		template<class T, std::size_t S>
		inline Epic::SVector<T, S>& Swizzle(Epic::Vector<T, S>& v)
		{
			static_assert(sizeof(Epic::SVector<T, S>) == sizeof(Epic::Vector<T, S>));
			return reinterpret_cast<Epic::SVector<T, S>&>(v);
		}

		template<class T, std::size_t S>
		inline const Epic::SVector<T, S>& Swizzle(const Epic::Vector<T, S>& v)
		{
			static_assert(sizeof(Epic::SVector<T, S>) == sizeof(Epic::Vector<T, S>));
			return reinterpret_cast<const Epic::SVector<T, S>&>(v);
		}
	}
}

