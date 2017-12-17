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

#include <Epic/Math/Constants.hpp>
#include <cassert>
#include <cmath>
#include <iostream>
#include <type_traits>
#include <utility>

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	template<class T = float>
	class Degree;

	template<class T = float>
	class Radian;
}

//////////////////////////////////////////////////////////////////////////////

// Conversions
namespace Epic
{
	namespace
	{
		// Convert a degree value to radians
		template<class T>
		constexpr T DegToRad(const T value) noexcept
		{
			return Epic::Pi<T> * value / T(180);
		}

		// Convert a radian value to degrees
		template<class T>
		constexpr T RadToDeg(const T value) noexcept
		{
			return T(180) * value / Epic::Pi<T>;
		}
	}
}

//////////////////////////////////////////////////////////////////////////////

// Radian
template<class T>
class Epic::Radian
{
public:
	using Type = Epic::Radian<T>;
	using value_type = T;

private:
	value_type m_Value;

public:
	Radian() noexcept = default;
	Radian(const Type&) noexcept = default;
	Radian(Type&&) noexcept = default;

	template<class U, typename = std::enable_if_t<std::is_convertible_v<U, T>>>
	constexpr Radian(U value) noexcept
		: m_Value(static_cast<T>(value)) { }

	template<class U, typename = std::enable_if_t<std::is_convertible_v<U, T>>>
	constexpr Radian(Degree<U> value) noexcept
		: m_Value(Epic::DegToRad(static_cast<T>(value.Value()))) { }

public:
	constexpr T Value() const noexcept
	{
		return m_Value;
	}

	constexpr T Sin() const noexcept
	{
		return static_cast<T>(std::sin(m_Value));
	}

	constexpr T Cos() const noexcept
	{
		return static_cast<T>(std::cos(m_Value));
	}

	constexpr std::pair<T, T> SinCos() const noexcept
	{
		return std::make_pair(Sin(), Cos());
	}

	constexpr T Tan() const noexcept
	{
		return static_cast<T>(std::tan(m_Value));
	}

	Type& Normalize(T min = T(0)) noexcept
	{
		m_Value = std::remainder(m_Value + min, Epic::TwoPi<T>);
		m_Value -= (m_Value < T(0)) ? min - Epic::TwoPi<T> : min;
		
		return *this;
	}

public:
	static Type NormalOf(Type value, T min = T(0)) noexcept
	{
		return value.Normalize(min);
	}

public:
	constexpr Type operator - () const
	{
		return { -m_Value };
	}

public:
	#pragma region Assignment Operators
	
	#define CREATE_ASSIGNMENT_OPERATOR(Op)	\
																					\
	Type& operator Op (value_type value) noexcept									\
	{																				\
		m_Value Op std::move(value);												\
		return *this;																\
	}																				\
																					\
	Type& operator Op (Type value) noexcept											\
	{																				\
		m_Value Op value.Value();													\
		return *this;																\
	}																				\
																					\
	template<class U, typename = std::enable_if_t<std::is_convertible_v<U, T>>>		\
	Type& operator Op (Radian<U> value) noexcept									\
	{																				\
		m_Value Op static_cast<T>(value.Value());									\
		return *this;																\
	}																				\
																					\
	template<class U, typename = std::enable_if_t<std::is_convertible_v<U, T>>>		\
	Type& operator Op (Degree<U> value) noexcept									\
	{																				\
		m_Value Op Epic::DegToRad(static_cast<T>(value.Value()));					\
		return *this;																\
	}

	CREATE_ASSIGNMENT_OPERATOR(= );
	CREATE_ASSIGNMENT_OPERATOR(+= );
	CREATE_ASSIGNMENT_OPERATOR(-= );
	CREATE_ASSIGNMENT_OPERATOR(*= );
	CREATE_ASSIGNMENT_OPERATOR(/= );

	#undef CREATE_ASSIGNMENT_OPERATOR

	#pragma endregion

public:
	#pragma region Arithmetic Operators
	#define CREATE_ARITHMETIC_OPERATOR(Op) 	\
																	\
	Type operator Op (value_type value) const noexcept				\
	{																\
		Type result{ *this };										\
		result Op= std::move(value);								\
		return result;												\
	}																\
																	\
	Type operator Op (Type value) const noexcept					\
	{																\
		Type result{ *this };										\
		result Op= std::move(value);								\
		return result;												\
	}																\
																	\
	template<class U>												\
	Type operator Op (Radian<U> value) const noexcept				\
	{																\
		Type result{ *this };										\
		result Op= std::move(value);								\
		return result;												\
	}																\
																	\
	template<class U>												\
	Type operator Op (Degree<U> value) const noexcept				\
	{																\
		Type result{ *this };										\
		result Op= std::move(value);								\
		return result;												\
	}																\
																	\
	friend Type operator Op (value_type value, Type rad) noexcept	\
	{																\
		Type result{ rad };											\
		result Op= std::move(value);								\
		return result;												\
	}

	CREATE_ARITHMETIC_OPERATOR(+);
	CREATE_ARITHMETIC_OPERATOR(-);
	CREATE_ARITHMETIC_OPERATOR(*);
	CREATE_ARITHMETIC_OPERATOR(/);

	#undef CREATE_ARITHMETIC_OPERATOR
	#pragma endregion

public:
	#pragma region Comparison Operators
	#define CREATE_COMPARISON_OPERATOR(Op)	\
																				\
	constexpr bool operator Op (value_type value) const noexcept				\
	{																			\
		return m_Value Op std::move(value);										\
	}																			\
																				\
	constexpr bool operator Op (Type value) const noexcept						\
	{																			\
		return m_Value Op value.Value();										\
	}																			\
																				\
	template<class U, typename = std::enable_if_t<std::is_convertible_v<U, T>>>	\
	constexpr bool operator Op (Radian<U> value) const noexcept					\
	{																			\
		return m_Value Op static_cast<T>(value.Value());						\
	}

	CREATE_COMPARISON_OPERATOR(< );
	CREATE_COMPARISON_OPERATOR(<= );
	CREATE_COMPARISON_OPERATOR(> );
	CREATE_COMPARISON_OPERATOR(>= );
	CREATE_COMPARISON_OPERATOR(== );
	CREATE_COMPARISON_OPERATOR(!= );
	
	#undef CREATE_COMPARISON_OPERATOR
	#pragma endregion

public:
	static const Type Zero;
	static const Type QuarterCircle;
	static const Type HalfCircle;
	static const Type ThreeQuarterCircle;
	static const Type Circle;
};

template<class T> 
const Epic::Radian<T> Epic::Radian<T>::Zero = T(0);

template<class T>
const Epic::Radian<T> Epic::Radian<T>::QuarterCircle = Epic::HalfPi<T>;

template<class T>
const Epic::Radian<T> Epic::Radian<T>::HalfCircle = Epic::Pi<T>;

template<class T>
const Epic::Radian<T> Epic::Radian<T>::ThreeQuarterCircle = Epic::Pi<T> + Epic::HalfPi<T>;

template<class T>
const Epic::Radian<T> Epic::Radian<T>::Circle = Epic::TwoPi<T>;

//////////////////////////////////////////////////////////////////////////////

// Degree
template<class T>
class Epic::Degree
{
public:
	using Type = Epic::Degree<T>;
	using value_type = T;

private:
	value_type m_Value;

public:
	Degree() noexcept = default;
	Degree(const Type&) noexcept = default;
	Degree(Type&&) noexcept = default;

	template<class U, typename = std::enable_if_t<std::is_convertible_v<U, T>>>
	constexpr Degree(U value) noexcept
		: m_Value(static_cast<T>(value)) { }

	template<class U, typename = std::enable_if_t<std::is_convertible_v<U, T>>>
	constexpr Degree(Radian<U> value) noexcept
		: m_Value(Epic::RadToDeg(static_cast<T>(value.Value()))) { }

public:
	constexpr T Value() const noexcept
	{
		return m_Value;
	}

	constexpr T Sin() const noexcept
	{
		return static_cast<T>(std::sin(Epic::DegToRad(m_Value)));
	}

	constexpr T Cos() const noexcept
	{
		return static_cast<T>(std::cos(Epic::DegToRad(m_Value)));
	}

	constexpr std::pair<T, T> SinCos() const noexcept
	{
		return std::make_pair(Sin(), Cos());
	}

	constexpr T Tan() const noexcept
	{
		return static_cast<T>(std::tan(Epic::DegToRad(m_Value)));
	}

	Type& Normalize(T min = T(0)) noexcept
	{
		m_Value = std::remainder(m_Value - min, T(360));
		if (m_Value < T(0)) m_Value += T(360);
		m_Value += min;

		return *this;
	}

public:
	static Type NormalOf(Type value, T min = T(0)) noexcept
	{
		return value.Normalize(min);
	}

public:
	constexpr Type operator - () const
	{
		return { -m_Value };
	}

public:
	#pragma region Assignment Operators
	
	#define CREATE_ASSIGNMENT_OPERATOR(Op)	\
																					\
	Type& operator Op (value_type value) noexcept									\
	{																				\
		m_Value Op std::move(value);												\
		return *this;																\
	}																				\
																					\
	Type& operator Op (Type value) noexcept											\
	{																				\
		m_Value Op value.Value();													\
		return *this;																\
	}																				\
																					\
	template<class U, typename = std::enable_if_t<std::is_convertible_v<U, T>>>		\
	Type& operator Op (Degree<U> value) noexcept									\
	{																				\
		m_Value Op static_cast<T>(value.Value());									\
		return *this;																\
	}																				\
																					\
	template<class U, typename = std::enable_if_t<std::is_convertible_v<U, T>>>		\
	Type& operator Op (Radian<U> value) noexcept									\
	{																				\
		m_Value Op Epic::RadToDeg(static_cast<T>(value.Value()));					\
		return *this;																\
	}

	CREATE_ASSIGNMENT_OPERATOR(= );
	CREATE_ASSIGNMENT_OPERATOR(+= );
	CREATE_ASSIGNMENT_OPERATOR(-= );
	CREATE_ASSIGNMENT_OPERATOR(*= );
	CREATE_ASSIGNMENT_OPERATOR(/= );
	
	#undef CREATE_ASSIGNMENT_OPERATOR

	#pragma endregion

public:
	#pragma region Arithmetic Operators
	#define CREATE_ARITHMETIC_OPERATOR(Op) 	\
																	\
	Type operator Op (value_type value) const noexcept				\
	{																\
		Type result{ *this };										\
		result Op= std::move(value);								\
		return result;												\
	}																\
																	\
	Type operator Op (Type value) const noexcept					\
	{																\
		Type result{ *this };										\
		result Op= std::move(value);								\
		return result;												\
	}																\
																	\
	template<class U>												\
	Type operator Op (Degree<U> value) const noexcept				\
	{																\
		Type result{ *this };										\
		result Op= std::move(value);								\
		return result;												\
	}																\
																	\
	template<class U>												\
	Type operator Op (Radian<U> value) const noexcept				\
	{																\
		Type result{ *this };										\
		result Op= std::move(value);								\
		return result;												\
	}																\
																	\
	friend Type operator Op (value_type value, Type deg) noexcept	\
	{																\
		Type result{ deg };											\
		result Op= std::move(value);								\
		return result;												\
	}

	CREATE_ARITHMETIC_OPERATOR(+);
	CREATE_ARITHMETIC_OPERATOR(-);
	CREATE_ARITHMETIC_OPERATOR(*);
	CREATE_ARITHMETIC_OPERATOR(/);

	#undef CREATE_ARITHMETIC_OPERATOR
	#pragma endregion

public:
	#pragma region Comparison Operators
	#define CREATE_COMPARISON_OPERATOR(Op)	\
																				\
	constexpr bool operator Op (value_type value) const noexcept				\
	{																			\
		return m_Value Op value;												\
	}																			\
																				\
	constexpr bool operator Op (Type value) const noexcept						\
	{																			\
		return m_Value Op value.Value();										\
	}																			\
																				\
	template<class U, typename = std::enable_if_t<std::is_convertible_v<U, T>>>	\
	constexpr bool operator Op (Degree<U> value) const noexcept					\
	{																			\
		return m_Value Op static_cast<T>(value.Value());						\
	}

	CREATE_COMPARISON_OPERATOR(< );
	CREATE_COMPARISON_OPERATOR(<= );
	CREATE_COMPARISON_OPERATOR(> );
	CREATE_COMPARISON_OPERATOR(>= );
	CREATE_COMPARISON_OPERATOR(== );
	CREATE_COMPARISON_OPERATOR(!= );
	
	#undef CREATE_COMPARISON_OPERATOR
	#pragma endregion

public:
	static const Type Zero;
	static const Type QuarterCircle;
	static const Type HalfCircle;
	static const Type ThreeQuarterCircle;
	static const Type Circle;
};

template<class T>
const Epic::Degree<T> Epic::Degree<T>::Zero = T(0);

template<class T>
const Epic::Degree<T> Epic::Degree<T>::QuarterCircle = T(90);

template<class T>
const Epic::Degree<T> Epic::Degree<T>::HalfCircle = T(180);

template<class T>
const Epic::Degree<T> Epic::Degree<T>::ThreeQuarterCircle = T(270);

template<class T>
const Epic::Degree<T> Epic::Degree<T>::Circle = T(360);

//////////////////////////////////////////////////////////////////////////////

// Comparison Operators
namespace Epic
{
	template<class T, class U>
	inline bool operator < (Degree<T> d, Radian<U> r) { return d < Degree<T>(std::move(r)); }

	template<class T, class U>
	inline bool operator < (Radian<T> r, Degree<U> d) { return r < Radian<T>(std::move(d)); }

	template<class T, class U>
	inline bool operator <= (Degree<T> d, Radian<U> r) { return d <= Degree<T>(std::move(r)); }

	template<class T, class U>
	inline bool operator <= (Radian<T> r, Degree<U> d) { return r <= Radian<T>(std::move(d)); }

	template<class T, class U>
	inline bool operator > (Degree<T> d, Radian<U> r) { return d > Degree<T>(std::move(r)); }

	template<class T, class U>
	inline bool operator > (Radian<T> r, Degree<U> d) { return r > Radian<T>(std::move(d)); }

	template<class T, class U>
	inline bool operator >= (Degree<T> d, Radian<U> r) { return d >= Degree<T>(std::move(r)); }

	template<class T, class U>
	inline bool operator >= (Radian<T> r, Degree<U> d) { return r >= Radian<T>(std::move(d)); }

	template<class T, class U>
	inline bool operator == (Degree<T> d, Radian<U> r) { return d == Degree<T>(std::move(r)); }

	template<class T, class U>
	inline bool operator == (Radian<T> r, Degree<U> d) { return r == Radian<T>(std::move(d)); }

	template<class T, class U>
	inline bool operator != (Degree<T> d, Radian<U> r) { return d != Degree<T>(std::move(r)); }

	template<class T, class U>
	inline bool operator != (Radian<T> r, Degree<U> d) { return r != Radian<T>(std::move(d)); }
}

//////////////////////////////////////////////////////////////////////////////

// Stream Operators
namespace Epic
{
	template<class Char, class Traits, class T>
	inline std::basic_ostream<Char, Traits>& operator << (std::basic_ostream<Char, Traits>& out, Epic::Radian<T> value)
	{
		return (out << value.Value());
	}

	template<class Char, class Traits, class T>
	inline std::basic_istream<Char, Traits>& operator >> (std::basic_istream<Char, Traits>& in, Epic::Radian<T>& value)
	{
		T a;
		in >> a;
		value = a;

		return in;
	}

	template<class Char, class Traits, class T>
	inline std::basic_ostream<Char, Traits>& operator << (std::basic_ostream<Char, Traits>& out, Epic::Degree<T> value)
	{
		return (out << value.Value());
	}

	template<class Char, class Traits, class T>
	inline std::basic_istream<Char, Traits>& operator >> (std::basic_istream<Char, Traits>& in, Epic::Degree<T>& value)
	{
		T a;
		in >> a;
		value = a;

		return in;
	}
}

//////////////////////////////////////////////////////////////////////////////

// Aliases
namespace Epic
{
	using RadianF = Radian<float>;
	using RadianD = Radian<double>;
	
	using Radianf = RadianF;
	using Radiand = RadianD;

	using DegreeF = Degree<float>;
	using DegreeD = Degree<double>;
	using DegreeI = Degree<int>;
	using DegreeL = Degree<long>;

	using Degreef = DegreeF;
	using Degreed = DegreeD;
	using Degreei = DegreeI;
	using Degreel = DegreeL;
}

//////////////////////////////////////////////////////////////////////////////

// UDLs
namespace Epic
{
	namespace UDL
	{
		constexpr Epic::DegreeF operator "" _deg(unsigned long long value)
		{
			return{ float(value) };
		}
		
		constexpr Epic::DegreeF operator "" _deg(long double value)
		{
			return{ float(value) };
		}

		constexpr Epic::DegreeD operator "" _degd(unsigned long long value)
		{
			return{ double(value) };
		}

		constexpr Epic::DegreeD operator "" _degd(long double value)
		{
			return{ double(value) };
		}

		constexpr Epic::RadianF operator "" _rad(long double value)
		{
			return{ float(value) };
		}

		constexpr Epic::RadianD operator "" _radd(long double value)
		{
			return{ double(value) };
		}
	}
}

//////////////////////////////////////////////////////////////////////////////

// Trigonometric Function Overloads
namespace std
{
	template<class T>
	constexpr T sin(Epic::Radian<T> value) noexcept
	{
		return value.Sin();
	}

	template<class T>
	constexpr T cos(Epic::Radian<T> value) noexcept
	{
		return value.Cos();
	}

	template<class T>
	constexpr T tan(Epic::Radian<T> value) noexcept
	{
		return value.Tan();
	}

	template<class T>
	constexpr T sin(Epic::Degree<T> value) noexcept
	{
		return value.Sin();
	}

	template<class T>
	constexpr T cos(Epic::Degree<T> value) noexcept
	{
		return value.Cos();
	}

	template<class T>
	constexpr T tan(Epic::Degree<T> value) noexcept
	{
		return value.Tan();
	}
}
