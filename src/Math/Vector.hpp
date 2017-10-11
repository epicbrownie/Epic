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
#include <Epic/Math/detail/QuaternionFwd.hpp>
#include <Epic/Math/detail/VectorHelpers.hpp>
#include <Epic/Math/detail/MathHelpers.hpp>
#include <Epic/Math/Swizzler.hpp>
#include <Epic/TMP/Sequence.hpp>
#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>
#include <type_traits>

//////////////////////////////////////////////////////////////////////////////

// Vector
template<class T, size_t S>
class Epic::Vector : public Epic::detail::VectorHelper<T, S>::BaseType
{
public:
	using Base = typename Epic::detail::VectorHelper<T, S>::BaseType;
	using Type = Epic::Vector<T, S>;

public:
	template<class, size_t>
	friend class Epic::Vector;

public:
	using value_type = T;
	constexpr static size_t Size = S;

private:
	using Base::Values;

public:
	#pragma region Constructors

	// Constructs a vector with default initialized values
	Vector() noexcept = default;

	// Copy-constructs a vector
	Vector(const Type&) noexcept = default;

	// Move-constructs a vector
	Vector(Type&&) noexcept = default;

	// Copy-converts a vector
	template<class U, typename = std::enable_if_t<std::is_convertible_v<U, T>>>
	explicit Vector(const Vector<U, Size>& vec) noexcept
	{
		ForEach([&](size_t n) { Values[n] = static_cast<T>(vec[n]); });
	}

	// Constructs a vector from a list of values.
	template<class U, typename = std::enable_if_t<std::is_convertible_v<U, T>>>
	explicit Vector(const U(&values)[Size]) noexcept
	{
		ForEach([&](size_t n) { Values[n] = static_cast<T>(values[n]); });
	}

	// Constructs a vector from a span of values
	template<class Arg, class... Args, typename = std::enable_if_t<(detail::Span<Arg, Args...>::value == Size)>>
	Vector(const Arg& arg, const Args&... args) noexcept
	{
		Construct(arg, args...);
	}

	// Constructs a vector whose values are all set to a value
	template<class U, typename = std::enable_if_t<std::is_convertible_v<U, T>>>
	Vector(const U& value) noexcept
	{
		const T cv = static_cast<T>(value);
		ForEach([&] (size_t n) { Values[n] = cv; });
	}
	
	// Constructs a vector whose values are all set to 0
	Vector(const ZeroesTag&) noexcept
		: Type(T(0))
	{ }

	// Constructs a vector whose values are all set to 1
	Vector(const OnesTag&) noexcept
		: Vector(T(1))
	{ }

	// Constructs an origin vector whose last element is 1
	Vector(const IdentityTag&) noexcept
	{
		const auto z = T(0);

		ForEachN<Size - 1>([&] (size_t n) { Values[n] = z; });
		Values[Size - 1] = T(1);
	}

	#pragma endregion

public:
	#pragma region Range Accessors

	// Retrieves the number of elements
	constexpr size_t size() const noexcept
	{
		return Size;
	}
	
	// Retrieves a pointer to the underlying element data
	constexpr const T* data() const noexcept
	{
		return Values.data();
	}

	// Retrieves a pointer to the underlying element data
	T* data() noexcept
	{
		return Values.data();
	}

	// Accesses the element at 'index'
	T& at(size_t index) noexcept
	{
		assert(index >= 0 && index < Size);
		return Values[index];
	}

	// Accesses the element at 'index'
	const T& at(size_t index) const noexcept
	{
		assert(index >= 0 && index < Size);
		return Values[index];
	}

	// Accesses the element at 'index'
	T& operator[] (size_t index) noexcept
	{
		assert(index >= 0 && index < Size);
		return Values[index];
	}

	// Accesses the element at 'index'
	const T& operator[] (size_t index) const noexcept
	{
		assert(index >= 0 && index < Size);
		return Values[index];
	}

	// Retrieves an iterator to the first element
	auto begin() noexcept -> decltype(Values.begin())
	{
		return Values.begin();
	}

	// Retrieves an iterator to the first element
	auto begin() const noexcept -> decltype(Values.begin())
	{
		return Values.begin();
	}

	// Retrieves an iterator to one past the last element
	auto end() const noexcept -> decltype(Values.end())
	{
		return Values.end();
	}

	#pragma endregion

public:
	// Sets a span of values explicitly
	template<class Arg, class... Args, typename = std::enable_if_t<(detail::Span<Arg, Args...>::value == Size)>>
	Type& Reset(const Arg& arg, const Args&... args) noexcept
	{
		Construct(arg, args...);
		return *this;
	}

	// Fills this vector with 'value'
	constexpr Type& Fill(const T& value) noexcept
	{
		ForEach([&] (size_t n) { Values[n] = value; });
		return *this;
	}

	// Sets this vector to an origin vector whose last element is 1
	constexpr Type& Origin() noexcept
	{
		const auto z = T(0);

		ForEachN<Size - 1>([&] (size_t n) { Values[n] = z; });
		Values[Size - 1] = T(1);

		return *this;
	}

	// Forces all values to the range [minValue, maxValue]
	constexpr Type& Clamp(const T& minValue, const T& maxValue) noexcept
	{
		ForEach([&] (size_t n) { Values[n] = std::min(std::max(minValue, Values[n]), maxValue); });
		return *this;
	}

	// Calculates the cross product of this vector and 'vec'
	constexpr auto Cross(const Vector<T, Size>& vec) const noexcept
	{
		if constexpr (Size == 1)
			return detail::MathOperationUnavailable();

		if constexpr (Size == 2)
			return T{ Values[0] * vec[1] - Values[1] * vec[0] };

		if constexpr (Size == 3)
			return Type
			{
				Values[1] * vec[2] - Values[2] * vec[1],
				Values[2] * vec[0] - Values[0] * vec[2],
				Values[0] * vec[1] - Values[1] * vec[0]
			};

		if constexpr (Size > 3)
		{
			Type result;
			result[0] = Values[1] * vec[2] - Values[2] * vec[1];
			result[1] = Values[2] * vec[0] - Values[0] * vec[2];
			result[2] = Values[0] * vec[1] - Values[1] * vec[0];
			return result;
		}
	}

	// Calculates the dot product of this vector and 'vec'
	constexpr T Dot(const Vector<T, Size>& vec) const noexcept
	{
		if constexpr (Size == 1)
			return Values[0] * vec[0];

		if constexpr (Size == 2)
			return Values[0] * vec[0] + Values[1] * vec[1];

		if constexpr (Size == 3)
			return Values[0] * vec[0] + Values[1] * vec[1] + Values[2] * vec[2];

		if constexpr (Size == 4)
			return Values[0] * vec[0] + Values[1] * vec[1] + Values[2] * vec[2] + Values[3] * vec[3];

		if constexpr (Size > 4)
		{
			T result = T(0);
			ForEach([&] (size_t n) { result += Values[n] * vec[n]; });
			return result;
		}
	}

	// Calculates the squared length of this vector
	constexpr T MagnitudeSq() const noexcept
	{
		return Dot(*this);
	}

	// Calculates the projection length of this vector onto unit vector 'axis'
	constexpr T ProjectionMagnitude(const Type& axis) const noexcept
	{
		return { this->Dot(axis) / axis.MagnitudeSq() };
	}
	
	// Calculates the length of this vector
	T Magnitude() const noexcept
	{
		return { static_cast<T>(std::sqrt(MagnitudeSq())) };
	}

	// Converts this vector to a unit vector
	Type& Normalize() noexcept
	{
		return *this /= Magnitude();
	}

	// Converts this vector to a unit vector 
	// Returns unmodified vector if magnitude is 0.
	Type& NormalizeSafe() noexcept
	{
		const auto m = Magnitude();
		return (m == T(0)) ? (*this) : (*this /= m);
	}

	// Raises all values to the power 'exp'
	Type& Power(const T& exp) noexcept
	{
		ForEach([&] (size_t n) { Values[n] = static_cast<T>(std::pow(Values[n], exp)); });
		return *this;
	}

	// Raises all values to the powers 'exp'
	Type& Power(const Type& exp) noexcept
	{
		ForEach([&] (size_t n) { Values[n] = static_cast<T>(std::pow(Values[n], exp[n])); });
	}

	// Calculates the projection of this vector onto unit vector 'axis'
	Type Project(const Type& axis) const noexcept;

	// Calculates the projection of this vector onto non-unit vector 'axis'
	Type ProjectN(const Type& axis) const noexcept;

	// Calculates the reflection of this vector off of the plane that is partially defined by unit vector 'normal'
	Type Reflect(const Type& normal) const noexcept;

	// Calculates the refracted vector of this vector off of the plane that is partially defined by unit vector 'normal'
	// 'eta' is the ratio of the refraction indexes
	Type Refract(const Type& normal, const T eta) const noexcept;

public:
	// Calculates the linear interpolation of vectors 'vecA' and 'vecB' 
	static auto MixOf(const Type& vecA, const Type& vecB, const T w = T(0.5)) noexcept
	{
		return vecA + ((vecB - vecA) * w);
	}

	// Calculates the normalized vector of 'vec'
	static Type NormalOf(const Type& vec) noexcept
	{
		return Type(vec).Normalize();
	}

	// Calculates the normalized vector of 'vec'
	// Returns a copy of 'vec' if magnitude is 0
	static Type SafeNormalOf(const Type& vec) noexcept
	{
		return Type(vec).NormalizeSafe();
	}

	// Calculates the orthonormalized vector of 'vecA' and 'vecB'
	static Type OrthoNormalOf(const Type& vecA, const Type& vecB) noexcept
	{
		return NormalOf(vecA - vecB * vecB.Dot(vecA));
	}

public:
	// Transforms this vector by the Quaternion 'quat'
	Type& operator *= (const Quaternion<T>& quat) noexcept;

	// Copies this vector with negated values
	Type operator - () const noexcept
	{
		Type result;
		ForEach([&](size_t n) { result[n] = -Values[n]; });
		return result;
	}

	// Implicitly converts to T (only available if this vector has just 1 element)
	operator std::conditional_t<(Size == 1), T, detail::MathOperationUnavailable>() const noexcept
	{
		if constexpr (Size == 1)
			return Values[0];
		else
		{
			assert(false && "Implicit cast to T is only available for vectors of size 1.");
			return { };
		}
	}

public:
	#pragma region Assignment Operators

	// Sets this vector to an origin vector whose last element is 1
	Type& operator = (const IdentityTag&) noexcept
	{
		return Origin();
	}

	//////

	#define CREATE_ASSIGNMENT_OPERATOR(Op)	\
																							\
	template<class U>																		\
	Type& operator Op (const U(&values)[Size]) noexcept										\
	{																						\
		ForEach([&](size_t index) { Values[index] Op values[index]; });				\
		return *this;																		\
	}																						\
																							\
	Type& operator Op (const Type& vec) noexcept											\
	{																						\
		ForEach([&](size_t index) { Values[index] Op vec[index]; });					\
		return *this;																		\
	}																						\
																							\
	template<class U, size_t Sz>														\
	Type& operator Op (const Vector<U, Sz>& vec) noexcept									\
	{																						\
		Epic::TMP::ForEach2<																\
			Epic::TMP::MakeSequence<size_t, Size>,										\
			Epic::TMP::MakeSequence<size_t, Sz>>										\
		::Apply([&](size_t iThis, size_t iOther)									\
		{																					\
			Values[iThis] Op vec[iOther];													\
		});																					\
																							\
		return *this;																		\
	}																						\
																							\
	template<class U, size_t US, size_t... Is,									\
		typename = std::enable_if_t<(sizeof...(Is) == Size)>>								\
	Type& operator Op (const Swizzler<U, US, Is...>& vec) noexcept					\
	{																						\
		Epic::TMP::ForEach2<																\
			Epic::TMP::MakeSequence<size_t, Size>,										\
			Epic::TMP::Sequence<size_t, Is...>>										\
		::Apply([&](size_t iThis, size_t iOther)									\
		{																					\
			Values[iThis] Op vec.m_Values[iOther];											\
		});																					\
																							\
		return *this;																		\
	}																						\
																							\
	Type& operator Op (const T& value) noexcept												\
	{																						\
		ForEach([&](size_t index) { Values[index] Op value; });						\
		return *this;																		\
	}

	CREATE_ASSIGNMENT_OPERATOR(= );
	CREATE_ASSIGNMENT_OPERATOR(+= );
	CREATE_ASSIGNMENT_OPERATOR(-= );
	CREATE_ASSIGNMENT_OPERATOR(*= );
	CREATE_ASSIGNMENT_OPERATOR(/= );

	#undef CREATE_ASSIGNMENT_OPERATOR

	//////

	#define CREATE_LOGIC_ASSIGNMENT_OPERATOR(Op)	\
																							\
	template<class U>																		\
	Type& operator Op (const U(&values)[Size]) noexcept										\
	{																						\
		if constexpr (std::is_integral<T>::value &&	std::is_integral<U>::value)				\
			ForEach([&](size_t index) { Values[index] Op values[index]; });			\
		else																				\
			assert(false && "Operation unavailable for non-integral types");				\
		return *this;																		\
	}																						\
																							\
	Type& operator Op (const Type& vec) noexcept											\
	{																						\
		if constexpr (std::is_integral<T>::value)											\
			ForEach([&](size_t index) { Values[index] Op vec[index]; });				\
		else																				\
			assert(false && "Operation unavailable for non-integral types");				\
		return *this;																		\
	}																						\
																							\
	template<class U>																		\
	Type& operator Op (const Vector<U, Size>& vec) noexcept									\
	{																						\
		if constexpr (std::is_integral<T>::value &&	std::is_integral<U>::value)				\
		{																					\
			ForEach([&](size_t i) { Values[i] Op vec[i]; });							\
		}																					\
		else																				\
			assert(false && "Operation unavailable for non-integral types");				\
																							\
		return *this;																		\
	}																						\
																							\
	template<class U, size_t US, size_t... Is,									\
		typename = std::enable_if_t<(sizeof...(Is) == Size)>>								\
	Type& operator Op (const Swizzler<U, US, Is...>& vec) noexcept							\
	{																						\
		if constexpr (std::is_integral<T>::value &&											\
					  std::is_integral<U>::value)											\
		{																					\
			Epic::TMP::ForEach2<															\
				Epic::TMP::MakeSequence<size_t, Size>,									\
				Epic::TMP::Sequence<size_t, Is...>>									\
			::Apply([&](size_t iThis, size_t iOther)								\
			{																				\
				Values[iThis] Op vec.m_Values[iOther];										\
			});																				\
		}																					\
		else																				\
			assert(false && "Operation unavailable for non-integral types");				\
																							\
		return *this;																		\
	}																						\
																							\
	Type& operator Op (const T& value) noexcept												\
	{																						\
		if constexpr (std::is_integral<T>::value)											\
			ForEach([&](size_t index) { Values[index] Op value; });					\
		else																				\
			assert(false && "Operation unavailable for non-integral types");				\
		return *this;																		\
	}

	CREATE_LOGIC_ASSIGNMENT_OPERATOR(|= );
	CREATE_LOGIC_ASSIGNMENT_OPERATOR(&= );
	CREATE_LOGIC_ASSIGNMENT_OPERATOR(^= );
	CREATE_LOGIC_ASSIGNMENT_OPERATOR(%= );
	CREATE_LOGIC_ASSIGNMENT_OPERATOR(<<= );
	CREATE_LOGIC_ASSIGNMENT_OPERATOR(>>= );

	#undef CREATE_LOGIC_ASSIGNMENT_OPERATOR

	#pragma endregion

	#pragma region Arithmetic Operators

	#define CREATE_ARITHMETIC_OPERATOR(Op) 	\
																									\
	template<class U>																				\
	Type operator Op (const U(&values)[Size]) const	noexcept										\
	{																								\
		Type result{ *this };																		\
		result Op= values;																			\
		return result;																				\
	}																								\
																									\
	Type operator Op (const Type& vec) const noexcept												\
	{																								\
		Type result{ *this };																		\
		result Op= vec;																				\
		return result;																				\
	}																								\
																									\
	template<class U, size_t Sz>																\
	Type operator Op (const Vector<U, Sz>& vec) const noexcept										\
	{																								\
		Type result{ *this };																		\
		result Op= vec;																				\
		return result;																				\
	}																								\
																									\
	template<class U, size_t US, size_t... Is>											\
	Type operator Op (const Swizzler<U, US, Is...>& vec) const noexcept						\
	{																								\
		Type result{ *this };																		\
		result Op= vec;																				\
		return result;																				\
	}																								\
																									\
	Type operator Op (const T& value) const noexcept												\
	{																								\
		Type result{ *this };																		\
		result Op= value;																			\
		return result;																				\
	}																								\
																									\
	friend Type operator Op (const T& value, const Type& vec) noexcept								\
	{																								\
		Type result{ value };																		\
		result Op= vec;																				\
		return result;																				\
	}

	CREATE_ARITHMETIC_OPERATOR(+);
	CREATE_ARITHMETIC_OPERATOR(-);
	CREATE_ARITHMETIC_OPERATOR(*);
	CREATE_ARITHMETIC_OPERATOR(/);

	// The following arithmetic operators are only defined for integral types
	CREATE_ARITHMETIC_OPERATOR(|);
	CREATE_ARITHMETIC_OPERATOR(&);
	CREATE_ARITHMETIC_OPERATOR(^);
	CREATE_ARITHMETIC_OPERATOR(%);
	CREATE_ARITHMETIC_OPERATOR(<<);
	CREATE_ARITHMETIC_OPERATOR(>>);

	#undef CREATE_ARITHMETIC_OPERATOR

	#pragma endregion

private:
	#pragma region Iteration Helpers

	template<class Function>
	void ForEach(Function fn) noexcept
	{
		if constexpr (Size == 1) { fn(0); }
		else if constexpr (Size == 2) { fn(0); fn(1); }
		else if constexpr (Size == 3) { fn(0); fn(1); fn(2); }
		else if constexpr (Size == 4) { fn(0); fn(1); fn(2); fn(3); }
		else { Epic::TMP::ForEachN<Size>::Apply(fn); }
	}

	template<class Function>
	void ForEach(Function fn) const noexcept
	{
		if constexpr (Size == 1) { fn(0); }
		else if constexpr (Size == 2) { fn(0); fn(1); }
		else if constexpr (Size == 3) { fn(0); fn(1); fn(2); }
		else if constexpr (Size == 4) { fn(0); fn(1); fn(2); fn(3); }
		else { Epic::TMP::ForEachN<Size>::Apply(fn); }
	}

	template<size_t N, class Function>
	void ForEachN(Function fn) noexcept
	{
		if constexpr (N == 1) { fn(0); }
		else if constexpr (N == 2) { fn(0); fn(1); }
		else if constexpr (N == 3) { fn(0); fn(1); fn(2); }
		else if constexpr (N == 4) { fn(0); fn(1); fn(2); fn(3); }
		else { Epic::TMP::ForEachN<N>::Apply(fn); }
	}

	template<size_t N, class Function>
	void ForEachN(Function fn) const noexcept
	{
		if constexpr (N == 1) { fn(0); }
		else if constexpr (N == 2) { fn(0); fn(1); }
		else if constexpr (N == 3) { fn(0); fn(1); fn(2); }
		else if constexpr (N == 4) { fn(0); fn(1); fn(2); fn(3); }
		else { Epic::TMP::ForEachN<N>::Apply(fn); }
	}

	#pragma endregion

	#pragma region Construction Helpers

	template<class... Vals>
	void Construct(const Vals&... vals) noexcept
	{
		ConstructAt(0, vals...);
	}

	void ConstructAt(size_t) noexcept
	{ 
		/* Do Nothing */
	}

	template<class Val, class... Vals>
	void ConstructAt(size_t offset, const Val& value, const Vals&... values) noexcept
	{
		PlaceAt(offset, value);
		ConstructAt(offset + Epic::detail::Span<Val>::value, values...);
	}

	template<class U, size_t Sz, typename = std::enable_if_t<std::is_convertible_v<U, T>>>
	void PlaceAt(size_t offset, const Vector<U, Sz>& value) noexcept
	{
		ForEachN<Sz>([&](size_t n) { Values[offset++] = static_cast<T>(value[n]); });
	}

	template<class U, size_t US, size_t... Is>
	void PlaceAt(size_t offset, const Swizzler<U, US, Is...>& value) noexcept
	{
		PlaceAt(offset, value.ToVector());
	}

	template<class U, size_t N, typename = std::enable_if_t<std::is_convertible_v<U, T>>>
	void PlaceAt(size_t offset, const U(&value)[N]) noexcept
	{
		ForEachN<N>([&](size_t n) { Values[offset++] = static_cast<T>(value[n]); });
	}

	template<class U, size_t Sz, typename = std::enable_if_t<std::is_convertible_v<U, T>>>
	void PlaceAt(size_t offset, const std::array<U, Sz>& value) noexcept
	{
		ForEachN<Sz>([&](size_t n) { Values[offset++] = static_cast<T>(value[n]); });
	}

	template<class Val, typename = std::enable_if_t<std::is_convertible_v<Val, T>>>
	void PlaceAt(size_t offset, const Val& value) noexcept
	{
		Values[offset] = static_cast<T>(value);
	}

	#pragma endregion

public:
	template<class U, size_t Sz>
	friend bool operator == (const Vector<U, Sz>& vecA, const Vector<U, Sz>& vecB) noexcept;

	template<class U, size_t Sz>
	friend bool operator != (const Vector<U, Sz>& vecA, const Vector<U, Sz>& vecB) noexcept;

	template<class U, size_t Sz>
	friend std::ostream& operator << (std::ostream& stream, const Vector<U, Sz>& vec);

	template<class U, size_t Sz>
	friend std::wostream& operator << (std::wostream& stream, const Vector<U, Sz>& vec);

	template<class U, size_t Sz>
	friend std::istream& operator >> (std::istream& stream, Vector<U, Sz>& vec);

	template<class U, size_t Sz>
	friend std::wistream& operator >> (std::wistream& stream, Vector<U, Sz>& vec);
};

//////////////////////////////////////////////////////////////////////////////

// Implementation
namespace Epic
{
	template<class T, size_t S>
	Vector<T, S> Vector<T, S>::Project(const Type& axis) const noexcept
	{
		Type result(axis);
		result *= this->ProjectionMagnitude(axis);
		return result;
	}

	template<class T, size_t S>
	Vector<T, S> Vector<T, S>::ProjectN(const Type& axis) const noexcept
	{
		Type result = Type::NormalOf(axis);
		result *= this->Dot(result);
		return result;
	}

	template<class T, size_t S>
	Vector<T, S> Vector<T, S>::Reflect(const Type& normal) const noexcept
	{
		Type n = normal;
		Type result = *this;

		n *= T(2) * result.Dot(n);
		result -= n;

		return result;
	}

	template<class T, size_t S>
	Vector<T, S> Vector<T, S>::Refract(const Type& normal, const T eta) const noexcept
	{
		Type I = Vector::NormalOf(*this);
		Type N = normal;

		const T NdotI = N.Dot(I);
		const T k = T(1) - (eta * eta * (T(1) - NdotI * NdotI));

		if (k < T(0))
			return{ T(0) };

		I *= eta;
		N *= eta * NdotI + T(std::sqrt(k));
		I -= N;

		return I;
	}
}

//////////////////////////////////////////////////////////////////////////////

// Friend Operators
namespace Epic
{
	template<class U, size_t Sz>
	inline bool operator == (const Vector<U, Sz>& vecA, const Vector<U, Sz>& vecB) noexcept
	{
		bool result = true;

		for (size_t i = 0; i < Sz; ++i)
			result &= (vecA[i] == vecB[i]);

		return result;
	}

	template<class U, size_t Sz>
	inline bool operator != (const Vector<U, Sz>& vecA, const Vector<U, Sz>& vecB) noexcept
	{
		bool result = true;

		for (size_t i = 0; i < Sz; ++i)
			result &= (vecA[i] != vecB[i]);

		return result;
	}

	template<class U, size_t Sz>
	inline std::ostream& operator << (std::ostream& stream, const Vector<U, Sz>& vec)
	{
		stream << '[';
		vec.ForEach([&](size_t n) 
		{ 
			if (n > 0) 
				stream << ", "; 
			stream << vec[n]; 
		});
		stream << ']';

		return stream;
	}

	template<class U, size_t Sz>
	inline std::wostream& operator << (std::wostream& stream, const Vector<U, Sz>& vec)
	{
		stream << L'[';
		vec.ForEach([&](size_t n) 
		{ 
			if (n > 0)
				stream << L", ";
			stream << vec[n]; 
		});
		stream << L']';

		return stream;
	}

	template<class U, size_t Sz>
	inline std::istream& operator >> (std::istream& stream, Vector<U, Sz>& vec)
	{
		if (stream.peek() == '[')
			stream.ignore(1);

		vec.ForEach([&](size_t n) 
		{ 
			if (n > 0 && stream.peek() == ',')
				stream.ignore(1);
			stream >> vec[n]; 
		});
		
		if (stream.peek() == ']')
			stream.ignore(1);

		return stream;
	}

	template<class U, size_t Sz>
	inline std::wistream& operator >> (std::wistream& stream, Vector<U, Sz>& vec)
	{
		if (stream.peek() == L'[')
			stream.ignore(1);

		vec.ForEach([&](size_t n) 
		{ 
			if (n > 0 && stream.peek() == L',')
				stream.ignore(1);
			stream >> vec[n]; 
		});
		
		if (stream.peek() == L']')
			stream.ignore(1);

		return stream;
	}
}

//////////////////////////////////////////////////////////////////////////////

// Aliases
namespace Epic
{
	template<class T = float> using Vector2 = Vector<T, 2>;
	template<class T = float> using Vector3 = Vector<T, 3>;
	template<class T = float> using Vector4 = Vector<T, 4>;
	template<class T = float> using Point2 = Vector<T, 2>;
	template<class T = float> using Point3 = Vector<T, 3>;
	template<class T = float> using Point4 = Vector<T, 4>;
	template<class T = float> using Direction2 = Vector<T, 2>;
	template<class T = float> using Direction3 = Vector<T, 3>;
	template<class T = float> using Direction4 = Vector<T, 4>;
	template<class T = float> using TexCoord2 = Vector<T, 2>;
	template<class T = float> using TexCoord3 = Vector<T, 3>;
	template<class T = float> using TexCoord4 = Vector<T, 4>;
	template<class T = float> using Normal2 = Vector<T, 2>;
	template<class T = float> using Normal3 = Vector<T, 3>;
	template<class T = float> using Normal4 = Vector<T, 4>;
	template<class T = float> using Size2 = Vector<T, 2>;
	template<class T = float> using Size3 = Vector<T, 3>;
	template<class T = float> using Color3 = Vector<T, 3>;
	template<class T = float> using Color4 = Vector<T, 4>;

	using Vector2f = Vector<float, 2>;
	using Vector2d = Vector<double, 2>;
	using Vector2i = Vector<int, 2>;
	using Vector2l = Vector<long, 2>;
	using Vector3f = Vector<float, 3>;
	using Vector3d = Vector<double, 3>;
	using Vector3i = Vector<int, 3>;
	using Vector3l = Vector<long, 3>;
	using Vector4f = Vector<float, 4>;
	using Vector4d = Vector<double, 4>;
	using Vector4i = Vector<int, 4>;
	using Vector4l = Vector<long, 4>;

	using Point2f = Vector<float, 2>;
	using Point2d = Vector<double, 2>;
	using Point2i = Vector<int, 2>;
	using Point2l = Vector<long, 2>;
	using Point3f = Vector<float, 3>;
	using Point3d = Vector<double, 3>;
	using Point3i = Vector<int, 3>;
	using Point3l = Vector<long, 3>;
	using Point4f = Vector<float, 4>;
	using Point4d = Vector<double, 4>;
	using Point4i = Vector<int, 4>;
	using Point4l = Vector<long, 4>;

	using Direction2f = Vector<float, 2>;
	using Direction2d = Vector<double, 2>;
	using Direction3f = Vector<float, 3>;
	using Direction3d = Vector<double, 3>;
	using Direction4f = Vector<float, 4>;
	using Direction4d = Vector<double, 4>;

	using TexCoord2f = Vector<float, 2>;
	using TexCoord2d = Vector<double, 2>;
	using TexCoord3f = Vector<float, 3>;
	using TexCoord3d = Vector<double, 3>;
	using TexCoord4f = Vector<float, 4>;
	using TexCoord4d = Vector<double, 4>;

	using Normal2f = Vector<float, 2>;
	using Normal2d = Vector<double, 2>;
	using Normal3f = Vector<float, 3>;
	using Normal3d = Vector<double, 3>;
	using Normal4f = Vector<float, 4>;
	using Normal4d = Vector<double, 4>;

	using Size2f = Vector<float, 2>;
	using Size2d = Vector<double, 2>;
	using Size2i = Vector<int, 2>;
	using Size2l = Vector<long, 2>;
	using Size3f = Vector<float, 3>;
	using Size3d = Vector<double, 3>;
	using Size3i = Vector<int, 3>;
	using Size3l = Vector<long, 3>;

	using Color3f = Vector<float, 3>;
	using Color3i = Vector<int, 3>;
	using Color4f = Vector<float, 4>;
	using Color4i = Vector<int, 4>;
}
