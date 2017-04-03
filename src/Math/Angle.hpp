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
#include <cmath>
#include <iostream>

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
		template<typename T>
		constexpr T DegToRad(const T value) noexcept
		{
			return Epic::Pi<T> * value / T(180);
		}

		// Convert a radian value to degrees
		template<typename T>
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
	using ValueType = T;

private:
	ValueType m_Value;

public:
	Radian() = default;
	Radian(const Type&) = default;
	Radian(Type&&) = default;

	constexpr Radian(const T value) noexcept
		: m_Value(value) { }

	template<typename U>
	constexpr Radian(const Degree<U>& value) noexcept
		: m_Value(Epic::DegToRad(T(value.Value()))) { }

public:
	constexpr T Value() const noexcept
	{
		return m_Value;
	}

	constexpr T Sin() const noexcept
	{
		return std::sin(m_Value);
	}

	constexpr T Cos() const noexcept
	{
		return std::cos(m_Value);
	}

	constexpr T Tan() const noexcept
	{
		return std::tan(m_Value);
	}

	Type& Clamp() noexcept
	{
		while (m_Value > Epic::TwoPi<T>)
			m_Value -= Epic::TwoPi<T>;

		while (m_Value < -Epic::TwoPi<T>)
			m_Value += Epic::TwoPi<T>;

		return *this;
	}

public:
	constexpr Type operator - () const
	{
		return{ -m_Value };
	}

public:
#pragma region Assignment Operators
#define CREATE_ASSIGNMENT_OPERATOR(Op)	\
																		\
	inline Type& operator Op (const ValueType& value) noexcept			\
	{																	\
		m_Value Op value;												\
		return *this;													\
	}																	\
																		\
	inline Type& operator Op (const Type& value) noexcept				\
	{																	\
		m_Value Op value.Value();										\
		return *this;													\
	}																	\
																		\
	template<typename U>												\
	inline Type& operator Op (const Radian<U>& value) noexcept			\
	{																	\
		m_Value Op T(value.Value());									\
		return *this;													\
	}																	\
																		\
	template<typename U>												\
	inline Type& operator Op (const Degree<U>& value) noexcept			\
	{																	\
		m_Value Op Epic::DegToRad(T(value.Value()));					\
		return *this;													\
	}

	CREATE_ASSIGNMENT_OPERATOR(= );
	CREATE_ASSIGNMENT_OPERATOR(+= );
	CREATE_ASSIGNMENT_OPERATOR(-= );
	CREATE_ASSIGNMENT_OPERATOR(*= );
	CREATE_ASSIGNMENT_OPERATOR(/= );

	// The following assignment operators will fail for non-integral types
	CREATE_ASSIGNMENT_OPERATOR(|= );
	CREATE_ASSIGNMENT_OPERATOR(&= );
	CREATE_ASSIGNMENT_OPERATOR(^= );
	CREATE_ASSIGNMENT_OPERATOR(%= );
	CREATE_ASSIGNMENT_OPERATOR(<<= );
	CREATE_ASSIGNMENT_OPERATOR(>>= );

#undef CREATE_ASSIGNMENT_OPERATOR
#pragma endregion

public:
#pragma region Arithmetic Operators
#define CREATE_ARITHMETIC_OPERATOR(Op) 	\
																						\
	inline Type operator Op (const ValueType& value) const noexcept						\
	{																					\
		Type result{ *this };															\
		result Op= value;																\
		return result;																	\
	}																					\
																						\
	inline Type operator Op (const Type& value) const noexcept							\
	{																					\
		Type result{ *this };															\
		result Op= value;																\
		return result;																	\
	}																					\
																						\
	template<class U>																	\
	inline Type operator Op (const Radian<U>& value) const noexcept						\
	{																					\
		Type result{ *this };															\
		result Op= value;																\
		return result;																	\
	}																					\
																						\
	template<class U>																	\
	inline Type operator Op (const Degree<U>& value) const noexcept						\
	{																					\
		Type result{ *this };															\
		result Op= value;																\
		return result;																	\
	}																					\
																						\
	friend inline Type operator Op (const ValueType& value, const Type& rad) noexcept	\
	{																					\
		Type result{ rad };																\
		result Op= value;																\
		return result;																	\
	}

	CREATE_ARITHMETIC_OPERATOR(+);
	CREATE_ARITHMETIC_OPERATOR(-);
	CREATE_ARITHMETIC_OPERATOR(*);
	CREATE_ARITHMETIC_OPERATOR(/ );

	// The following arithmetic operators are only defined for integral types
	CREATE_ARITHMETIC_OPERATOR(| );
	CREATE_ARITHMETIC_OPERATOR(&);
	CREATE_ARITHMETIC_OPERATOR(^);
	CREATE_ARITHMETIC_OPERATOR(%);
	CREATE_ARITHMETIC_OPERATOR(<< );
	CREATE_ARITHMETIC_OPERATOR(>> );

#undef CREATE_ARITHMETIC_OPERATOR
#pragma endregion

public:
#pragma region Comparison Operators
#define CREATE_COMPARISON_OPERATOR(Op)	\
																		\
	constexpr bool operator Op (const ValueType& value) const noexcept	\
	{																	\
		return m_Value Op value;										\
	}																	\
																		\
	constexpr bool operator Op (const Type& value) const noexcept		\
	{																	\
		return m_Value Op value.Value();								\
	}																	\
																		\
	template<typename U>												\
	constexpr bool operator Op (const Radian<U>& value) const noexcept	\
	{																	\
		return m_Value Op value.Value();								\
	}

	CREATE_COMPARISON_OPERATOR(<);
	CREATE_COMPARISON_OPERATOR(<= );
	CREATE_COMPARISON_OPERATOR(>);
	CREATE_COMPARISON_OPERATOR(>= );
	CREATE_COMPARISON_OPERATOR(== );
	CREATE_COMPARISON_OPERATOR(!= );

#undef CREATE_COMPARISON_OPERATOR
#pragma endregion

public:
	static const Radian Zero;
	static const Radian QuarterCircle;
	static const Radian HalfCircle;
	static const Radian ThreeQuarterCircle;
	static const Radian Circle;
};

template<class T>
decltype(Epic::Radian<T>::Zero) Epic::Radian<T>::Zero(T(0));

template<class T>
decltype(Epic::Radian<T>::QuarterCircle) Epic::Radian<T>::QuarterCircle(Epic::HalfPi<T>);

template<class T>
decltype(Epic::Radian<T>::HalfCircle) Epic::Radian<T>::HalfCircle(Epic::Pi<T>);

template<class T>
decltype(Epic::Radian<T>::ThreeQuarterCircle) Epic::Radian<T>::ThreeQuarterCircle(Epic::Pi<T> +Epic::HalfPi<T>);

template<class T>
decltype(Epic::Radian<T>::Circle) Epic::Radian<T>::Circle(Epic::TwoPi<T>);

//////////////////////////////////////////////////////////////////////////////

// Degree
template<class T>
class Epic::Degree
{
public:
	using Type = Epic::Degree<T>;
	using ValueType = T;

private:
	ValueType m_Value;

public:
	Degree() noexcept = default;
	Degree(const Type&) noexcept = default;
	Degree(Type&&) noexcept = default;

	constexpr Degree(const T value) noexcept
		: m_Value(value) { }

	template<typename U>
	constexpr Degree(const Radian<U>& value) noexcept
		: m_Value(Epic::RadToDeg(T(value.Value()))) { }

public:
	constexpr T Value() const noexcept
	{
		return m_Value;
	}

	constexpr T Sin() const noexcept
	{
		return std::sin(Epic::DegToRad(m_Value));
	}

	constexpr T Cos() const noexcept
	{
		return std::cos(Epic::DegToRad(m_Value));
	}

	constexpr T Tan() const noexcept
	{
		return std::tan(Epic::DegToRad(m_Value));
	}

	Type& Clamp() noexcept
	{
		while (m_Value > T(360))
			m_Value -= T(360);

		while (m_Value < -T(360))
			m_Value += T(360);

		return *this;
	}

public:
	constexpr Type operator - () const
	{
		return{ -m_Value };
	}

public:
#pragma region Assignment Operators
#define CREATE_ASSIGNMENT_OPERATOR(Op)	\
																		\
	Type& operator Op (const ValueType& value) noexcept					\
	{																	\
		m_Value Op value;												\
		return *this;													\
	}																	\
																		\
	Type& operator Op (const Type& value) noexcept						\
	{																	\
		m_Value Op T(value.Value());									\
		return *this;													\
	}																	\
																		\
	template<typename U>												\
	Type& operator Op (const Degree<U>& value) noexcept					\
	{																	\
		m_Value Op T(value.Value());									\
		return *this;													\
	}																	\
																		\
	template<typename U>												\
	Type& operator Op (const Radian<U>& value) noexcept					\
	{																	\
		m_Value Op Epic::RadToDeg(T(value.Value()));					\
		return *this;													\
	}

	CREATE_ASSIGNMENT_OPERATOR(= );
	CREATE_ASSIGNMENT_OPERATOR(+= );
	CREATE_ASSIGNMENT_OPERATOR(-= );
	CREATE_ASSIGNMENT_OPERATOR(*= );
	CREATE_ASSIGNMENT_OPERATOR(/= );

	// The following assignment operators will fail for non-integral types
	CREATE_ASSIGNMENT_OPERATOR(|= );
	CREATE_ASSIGNMENT_OPERATOR(&= );
	CREATE_ASSIGNMENT_OPERATOR(^= );
	CREATE_ASSIGNMENT_OPERATOR(%= );
	CREATE_ASSIGNMENT_OPERATOR(<<= );
	CREATE_ASSIGNMENT_OPERATOR(>>= );

#undef CREATE_ASSIGNMENT_OPERATOR
#pragma endregion

public:
#pragma region Arithmetic Operators
#define CREATE_ARITHMETIC_OPERATOR(Op) 	\
																						\
	inline Type operator Op (const ValueType& value) const noexcept						\
	{																					\
		Type result{ *this };															\
		result Op= value;																\
		return result;																	\
	}																					\
																						\
	inline Type operator Op (const Type& value) const noexcept							\
	{																					\
		Type result{ *this };															\
		result Op= value;																\
		return result;																	\
	}																					\
																						\
	template<class U>																	\
	inline Type operator Op (const Degree<U>& value) const noexcept						\
	{																					\
		Type result{ *this };															\
		result Op= value;																\
		return result;																	\
	}																					\
																						\
	template<class U>																	\
	inline Type operator Op (const Radian<U>& value) const noexcept						\
	{																					\
		Type result{ *this };															\
		result Op= value;																\
		return result;																	\
	}																					\
																						\
	friend inline Type operator Op (const ValueType& value, const Type& deg) noexcept	\
	{																					\
		Type result{ deg };																\
		result Op= value;																\
		return result;																	\
	}

	CREATE_ARITHMETIC_OPERATOR(+);
	CREATE_ARITHMETIC_OPERATOR(-);
	CREATE_ARITHMETIC_OPERATOR(*);
	CREATE_ARITHMETIC_OPERATOR(/ );

	// The following arithmetic operators are only defined for integral types
	CREATE_ARITHMETIC_OPERATOR(| );
	CREATE_ARITHMETIC_OPERATOR(&);
	CREATE_ARITHMETIC_OPERATOR(^);
	CREATE_ARITHMETIC_OPERATOR(%);
	CREATE_ARITHMETIC_OPERATOR(<< );
	CREATE_ARITHMETIC_OPERATOR(>> );

#undef CREATE_ARITHMETIC_OPERATOR
#pragma endregion

public:
#pragma region Comparison Operators
#define CREATE_COMPARISON_OPERATOR(Op)	\
																				\
	constexpr bool operator Op (const ValueType& value) const noexcept			\
	{																			\
		return m_Value Op value;												\
	}																			\
																				\
	constexpr bool operator Op (const Type& value) const noexcept				\
	{																			\
		return m_Value Op value.Value();										\
	}																			\
																				\
	template<typename U>														\
	constexpr bool operator Op (const Degree<U>& value) const noexcept			\
	{																			\
		return m_Value Op value.Value();										\
	}

	CREATE_COMPARISON_OPERATOR(<);
	CREATE_COMPARISON_OPERATOR(<= );
	CREATE_COMPARISON_OPERATOR(>);
	CREATE_COMPARISON_OPERATOR(>= );
	CREATE_COMPARISON_OPERATOR(== );
	CREATE_COMPARISON_OPERATOR(!= );

#undef CREATE_COMPARISON_OPERATOR
#pragma endregion

public:
	static const Degree Zero;
	static const Degree QuarterCircle;
	static const Degree HalfCircle;
	static const Degree ThreeQuarterCircle;
	static const Degree Circle;
};

template<class T>
decltype(Epic::Degree<T>::Zero) Epic::Degree<T>::Zero(T(0));

template<class T>
decltype(Epic::Degree<T>::QuarterCircle) Epic::Degree<T>::QuarterCircle(T(90));

template<class T>
decltype(Epic::Degree<T>::HalfCircle) Epic::Degree<T>::HalfCircle(T(180));

template<class T>
decltype(Epic::Degree<T>::ThreeQuarterCircle) Epic::Degree<T>::ThreeQuarterCircle(T(270));

template<class T>
decltype(Epic::Degree<T>::Circle) Epic::Degree<T>::Circle(T(360));

//////////////////////////////////////////////////////////////////////////////

// Stream Operators
namespace Epic
{
	template<class Char, class Traits, class T>
	inline std::basic_ostream<Char, Traits>& operator << (std::basic_ostream<Char, Traits>& out, const Epic::Radian<T>& value)
	{
		out << value.Value();
		return out;
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
	inline std::basic_ostream<Char, Traits>& operator << (std::basic_ostream<Char, Traits>& out, const Epic::Degree<T>& value)
	{
		out << value.Value();
		return out;
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

	using DegreeF = Degree<float>;
	using DegreeD = Degree<double>;
	using DegreeI = Degree<int>;
	using DegreeL = Degree<long>;
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
	constexpr T sin(const Epic::Radian<T>& value) noexcept
	{
		return value.Sin();
	}

	template<class T>
	constexpr T cos(const Epic::Radian<T>& value) noexcept
	{
		return value.Cos();
	}

	template<class T>
	constexpr T tan(const Epic::Radian<T>& value) noexcept
	{
		return value.Tan();
	}

	template<class T>
	constexpr T sin(const Epic::Degree<T>& value) noexcept
	{
		return value.Sin();
	}

	template<class T>
	constexpr T cos(const Epic::Degree<T>& value) noexcept
	{
		return value.Cos();
	}

	template<class T>
	constexpr T tan(const Epic::Degree<T>& value) noexcept
	{
		return value.Tan();
	}
}
