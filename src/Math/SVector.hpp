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

#include <Epic/Math/detail/SVectorFwd.hpp>
#include <Epic/Math/detail/VectorHelpers.hpp>
#include <Epic/Math/detail/MathHelpers.hpp>
#include <Epic/Math/Swizzler.hpp>
#include <cassert>

//////////////////////////////////////////////////////////////////////////////

// SVector
template<class T, size_t S>
class Epic::SVector : public Epic::detail::SVectorHelper<T, S>::BaseType
{
public:
	using Base = typename Epic::detail::SVectorHelper<T, S>::BaseType;
	using Type = Epic::SVector<T, S>;

public:
	using value_type = T;
	using VectorType = Epic::Vector<T, S>;

	constexpr static size_t Size = S;

private:
	using Base::Values;

public:
	SVector() noexcept = delete;
	SVector(const Type&) noexcept = delete;
	Type& operator = (const Type&) = delete;

public:
	const T& operator[] (size_t index) const noexcept
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
		template<class T, size_t S>
		constexpr Epic::SVector<T, S>& Swizzle(Epic::Vector<T, S>& v)
		{
			static_assert(sizeof(Epic::SVector<T, S>) == sizeof(Epic::Vector<T, S>));
			return reinterpret_cast<Epic::SVector<T, S>&>(v);
		}

		template<class T, size_t S>
		constexpr const Epic::SVector<T, S>& Swizzle(const Epic::Vector<T, S>& v)
		{
			static_assert(sizeof(Epic::SVector<T, S>) == sizeof(Epic::Vector<T, S>));
			return reinterpret_cast<const Epic::SVector<T, S>&>(v);
		}
	}
}
