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

#include <Epic/Math/detail/QuaternionFwd.hpp>
#include <Epic/Math/detail/QuaternionBase.hpp>
#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>
#include <type_traits>

//////////////////////////////////////////////////////////////////////////////

// Quaternion
template<class T>
class Epic::Quaternion : public Epic::detail::QuaternionBase<T>
{
public:
	using Base = Epic::detail::QuaternionBase<T>;
	using Type = Epic::Quaternion<T>;

public:
	using ValueType = T;

public:
	Quaternion() noexcept = default;
	Quaternion(const Type&) noexcept = default;
	Quaternion(Type&&) noexcept = default;

	template<class U>
	Quaternion(Quaternion<U>&& other)
};
