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
#include <cstdint>
#include <cmath>
#include <iostream>
#include <type_traits>

//////////////////////////////////////////////////////////////////////////////

// Vector
template<class T, std::size_t S>
class Epic::Vector : public Epic::detail::VectorHelper<T, S>::BaseType
{
public:
	using Base = typename Epic::detail::VectorHelper<T, S>::BaseType;
	using Type = Epic::Vector<T, S>;

public:
	template<class, std::size_t>
	friend class Epic::Vector;

public:
	using ValueType = T;
	constexpr static std::size_t Size = S;

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
	template<class U>
	Vector(const Vector<U, Size>& vec) noexcept
	{
		ForEach([&](std::size_t n) { Values[n] = static_cast<T>(vec[n]); });
	}

	// Constructs a vector from a list of values.
	template<class U>
	Vector(const U(&values)[Size]) noexcept
	{
		ForEach([&](std::size_t n) { Values[n] = values[n]; });
	}

	// Constructs a vector whose values are all set to a value
	Vector(const T& value) noexcept
	{
		ForEach([&](std::size_t n) { Values[n] = value; });
	}

	// Constructs a vector from a span of values
	template<class Arg, class... Args,
			 typename = std::enable_if_t<(detail::Span<Arg, Args...>::Value == Size)>>
	Vector(const Arg& arg, const Args&... args) noexcept
	{
		Construct(arg, args...);
	}

	// Constructs a vector whose values are all set to 0
	Vector(const ZeroesTag&) noexcept
		: Vector(T(0))
	{ }

	// Constructs a vector whose values are all set to 1
	Vector(const OnesTag&) noexcept
		: Vector(T(1))
	{ }

	// Constructs an origin vector whose last element is 1
	Vector(const IdentityTag&) noexcept
	{
		const auto z = T(0);

		ForEachN<Size - 1>([&](std::size_t n) { Values[n] = z; });
		Values[Size - 1] = T(1);
	}

	#pragma endregion

public:
	#pragma region Range Accessors

	// Accesses the element at 'index'
	T& at(std::size_t index) noexcept
	{
		assert(index >= 0 && index < Size);
		return Values[index];
	}

	// Accesses the element at 'index'
	const T& at(std::size_t index) const noexcept
	{
		assert(index >= 0 && index < Size);
		return Values[index];
	}

	// Accesses the element at 'index'
	T& operator[] (std::size_t index) noexcept
	{
		assert(index >= 0 && index < Size);
		return Values[index];
	}

	// Accesses the element at 'index'
	const T& operator[] (std::size_t index) const noexcept
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
	auto begin() const noexcept -> decltype(Values.cbegin())
	{
		return Values.cbegin();
	}

	// Retrieves an iterator to one past the last element
	auto end() const noexcept -> decltype(Values.cend())
	{
		return Values.cend();
	}

	// Retrieves the number of elements
	constexpr std::size_t size() const noexcept
	{
		return Size;
	}
	
	// Retrieves a pointer to the underlying element data
	T* data() noexcept
	{
		return Values.data();
	}

	// Retrieves a pointer to the underlying element data
	constexpr const T* data() const noexcept
	{
		return Values.data();
	}

	#pragma endregion

public:
	// Sets a span of values explicitly
	template<class Arg, class... Args, typename = std::enable_if_t<(detail::Span<Arg, Args...>::Value == Size)>>
	Type& Reset(const Arg& arg, const Args&... args) noexcept
	{
		Construct(arg, args...);
		return *this;
	}

	// Fills this vector with 'value'
	constexpr Type& Fill(const T& value) noexcept
	{
		ForEach([&](std::size_t n) { Values[n] = value; });
		return *this;
	}

	// Sets this vector to an origin vector whose last element is 1
	constexpr Type& Origin() noexcept
	{
		const auto z = T(0);

		ForEachN<Size - 1>([&](std::size_t n) { Values[n] = z; });
		Values[Size - 1] = T(1);

		return *this;
	}

	// Forces all values to the range [minValue, maxValue]
	constexpr Type& Clamp(const T& minValue, const T& maxValue) noexcept
	{
		ForEach([&](std::size_t n) { Values[n] = std::min(std::max(minValue, Values[n]), maxValue); });
		return *this;
	}

	// Calculates the 2d cross product of this vector and 'vec' (The z-component of the cross product of the vectors)
	template<typename U, typename EnabledForVector2 = std::enable_if_t<(Size == 2)>>
	constexpr T Cross(const Vector<U, 2>& vec) const noexcept
	{
		return { Values[0] * vec[1] - Values[1] * vec[0] };
	}

	// Calculates the 3d cross product of this vector and 'vec'
	template<typename U, typename EnabledForVector3OrLarger = std::enable_if_t<(Size >= 3)>>
	constexpr Type Cross(const Vector<U, Size>& vec) const noexcept
	{
		return 
		{
			Values[1] * vec[2] - Values[2] * vec[1],
			Values[2] * vec[0] - Values[0] * vec[2],
			Values[0] * vec[1] - Values[1] * vec[0]
		};
	}

	// Calculates the dot product of this vector and 'vec'
	template<class U>
	constexpr T Dot(const Vector<U, Size>& vec) const noexcept
	{
		T result = T(0);
		ForEach([&](std::size_t n) { result += Values[n] * vec[n]; });
		return result;
	}

	// Calculates the squared length of this vector
	constexpr T MagnitudeSq() const noexcept
	{
		return Dot(*this);
	}

	// Calculates the projection length of this vector onto unit vector 'axis'
	constexpr T ProjectionMagnitude(const Type& axis) const noexcept
	{
		return { Dot(axis) / axis.MagnitudeSq() };
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

	// Raises all values to the power 'pow'
	Type& Power(const T& exp) noexcept
	{
		ForEach([&](std::size_t n) { Values[n] = static_cast<T>(std::pow(Values[n], exp)); });
		return *this;
	}

	// Raises all values to the power 'pow'
	Type& Power(const Type& exp) noexcept
	{
		ForEach([&] (std::size_t n) { Values[n] = static_cast<T>(std::pow(Values[n], exp[n])); });
		return *this;
	}

	// Calculates the projection of this vector onto unit vector 'axis'
	Type Project(const Type& axis) const noexcept
	{
		Type result = axis;
		result *= ProjectionMagnitude(axis);
		return result;
	}

	// Calculates the projection of this vector onto non-unit vector 'axis'
	Type ProjectN(const Type& axis) const noexcept
	{
		auto result = Type::NormalOf(axis);
		result *= this->Dot(result);
		return result;
	}

	// Calculates the reflection of this vector off of the plane that is partially defined by unit vector 'normal'
	Type Reflect(const Type& normal) const noexcept
	{
		Type n = normal;
		Type result = *this;

		n *= T(2) * result.Dot(n);
		result -= n;
		
		return result;
	}

	// Calculates the refracted vector of this vector off of the plane that is partially defined by unit vector 'normal'
	// 'eta' is the ratio of the refraction indexes
	Type Refract(const Type& normal, const T eta) const noexcept
	{
		Type I = Type::NormalOf(*this);
		Type N = normal;

		const T NdotI = N.Dot(I);
		const T k = T(1) - (eta * eta * (T(1) - NdotI * NdotI));

		if (k < T(0))
			return{ 0 };
		
		I *= eta;
		N *= eta * NdotI + T(std::sqrt(k));
		I -= N;

		return I;
	}

public:
	// Calculates the linear interpolation of vectors 'vecA' and 'vecB' 
	static Type MixOf(const Type& vecA, const Type& vecB, const T w = T(0.5)) noexcept
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
		ForEach([&](std::size_t n) { result[n] = -Values[n]; });
		return result;
	}

	// Implicitly converts to T (only available if this vector has just 1 element)
	operator std::conditional_t<(Size == 1), T, detail::MathOperationUnavailable>() const noexcept
	{
		return Values[0];
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
		ForEach([&](std::size_t index) { Values[index] Op values[index]; });				\
		return *this;																		\
	}																						\
																							\
	Type& operator Op (const Type& vec) noexcept											\
	{																						\
		ForEach([&](std::size_t index) { Values[index] Op vec[index]; });					\
		return *this;																		\
	}																						\
																							\
	template<class U>																		\
	Type& operator Op (const Vector<U, Size>& vec) noexcept									\
	{																						\
		ForEach([&](std::size_t i) { Values[i] Op vec[i]; });								\
		return *this;																		\
	}																						\
																							\
	template<class U, std::size_t US, std::size_t... Is,									\
		typename = std::enable_if_t<(sizeof...(Is) == Size)>>								\
	Type& operator Op (const Swizzler<U, US, Is...>& vec) noexcept					\
	{																						\
		Epic::TMP::ForEach2<																\
			Epic::TMP::MakeSequence<std::size_t, Size>,										\
			Epic::TMP::Sequence<std::size_t, Is...>>										\
		::Apply([&](std::size_t i, std::size_t j) { Values[i] Op vec.m_Values[j]; });		\
																							\
		return *this;																		\
	}																						\
																							\
	Type& operator Op (const T& value) noexcept												\
	{																						\
		ForEach([&](std::size_t index) { Values[index] Op value; });						\
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
	template<class U,																		\
		typename = std::enable_if_t<std::is_integral_v<T> && std::is_integral_v<U>>>		\
	Type& operator Op (const U(&values)[Size]) noexcept										\
	{																						\
		ForEach([&](std::size_t index) { Values[index] Op values[index]; });				\
		return *this;																		\
	}																						\
																							\
	template<typename = std::enable_if_t<std::is_integral_v<T>>>							\
	Type& operator Op (const Type& vec) noexcept											\
	{																						\
		ForEach([&](std::size_t index) { Values[index] Op vec[index]; });					\
		return *this;																		\
	}																						\
																							\
	template<class U,																		\
		typename = std::enable_if_t<std::is_integral_v<T> && std::is_integral_v<U>>>		\
	Type& operator Op (const Vector<U, Size>& vec) noexcept									\
	{																						\
		ForEach([&](std::size_t i) { Values[i] Op vec[i]; });								\
		return *this;																		\
	}																						\
																							\
	template<class U, std::size_t US, std::size_t... Is,									\
		typename = std::enable_if_t<(sizeof...(Is) == Size) &&								\
									std::is_integral_v<T> && std::is_integral_v<U>>>		\
	Type& operator Op (const Swizzler<U, US, Is...>& vec) noexcept					\
	{																						\
		Epic::TMP::ForEach2<																\
			Epic::TMP::MakeSequence<std::size_t, Size>,										\
			Epic::TMP::Sequence<std::size_t, Is...>>										\
		::Apply([&](std::size_t i, std::size_t j) { Values[i] Op vec.m_Values[j]; });		\
																							\
		return *this;																		\
	}																						\
																							\
	template<typename = std::enable_if_t<std::is_integral<T>::value>>						\
	Type& operator Op (const T& value) noexcept												\
	{																						\
		ForEach([&](std::size_t index) { Values[index] Op value; });						\
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
	template<class U>																				\
	Type operator Op (const Vector<U, Size>& vec) const noexcept									\
	{																								\
		Type result{ *this };																		\
		result Op= vec;																				\
		return result;																				\
	}																								\
																									\
	template<class U, std::size_t US, std::size_t... Is,											\
		typename = std::enable_if_t<(sizeof...(Is) == Size)>>										\
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

private:
	#pragma region Iteration Helpers
	
	template<class Function>
	void ForEach(Function fn) noexcept
	{
		Epic::TMP::ForEach<Epic::TMP::MakeSequence<std::size_t, Size>>::Apply(fn);
	}

	template<class Function>
	void ForEach(Function fn) const noexcept
	{
		Epic::TMP::ForEach<Epic::TMP::MakeSequence<std::size_t, Size>>::Apply(fn);
	}

	template<std::size_t N, class Function>
	void ForEachN(Function fn) noexcept
	{
		Epic::TMP::ForEach<Epic::TMP::MakeSequence<std::size_t, N>>::Apply(fn);
	}

	template<std::size_t N, class Function>
	void ForEachN(Function fn) const noexcept
	{
		Epic::TMP::ForEach<Epic::TMP::MakeSequence<std::size_t, N>>::Apply(fn);
	}

	#pragma endregion

	#pragma region Construction Helpers

	template<class... Vals>
	void Construct(const Vals&... vals) noexcept
	{
		ConstructAt(0, vals...);
	}

	void ConstructAt(std::size_t) noexcept
	{ 
		/* Do Nothing */
	}

	template<class Val, class... Vals>
	void ConstructAt(std::size_t offset, const Val& value, const Vals&... values) noexcept
	{
		PlaceAt(offset, value);
		ConstructAt(offset + Epic::detail::Span<Val>::Value, values...);
	}

	template<class U, std::size_t Sz>
	void PlaceAt(std::size_t offset, const Vector<U, Sz>& value) noexcept
	{
		ForEachN<Sz>([&](std::size_t n) { Values[offset++] = value[n]; });
	}

	template<class U, std::size_t US, std::size_t... Is>
	void PlaceAt(std::size_t offset, const Swizzler<U, US, Is...>& value) noexcept
	{
		PlaceAt(offset, value.ToVector());
	}

	template<class U, std::size_t N>
	void PlaceAt(std::size_t offset, const U(&value)[N]) noexcept
	{
		ForEachN<N>([&](std::size_t n) { Values[offset++] = value[n]; });
	}

	template<class U, std::size_t Sz>
	void PlaceAt(std::size_t offset, const std::array<U, Sz>& value) noexcept
	{
		ForEachN<Sz>([&](std::size_t n) { Values[offset++] = value[n]; });
	}

	template<class Val>
	void PlaceAt(std::size_t offset, const Val& value) noexcept
	{
		Values[offset] = value;
	}

	#pragma endregion

public:
	template<class U, std::size_t Sz>
	friend bool operator == (const Vector<U, Sz>& vecA, const Vector<U, Sz>& vecB) noexcept;

	template<class U, std::size_t Sz>
	friend bool operator != (const Vector<U, Sz>& vecA, const Vector<U, Sz>& vecB) noexcept;

	template<class U, std::size_t Sz>
	friend std::ostream& operator << (std::ostream& stream, const Vector<U, Sz>& vec);

	template<class U, std::size_t Sz>
	friend std::wostream& operator << (std::wostream& stream, const Vector<U, Sz>& vec);

	template<class U, std::size_t Sz>
	friend std::istream& operator >> (std::istream& stream, Vector<U, Sz>& vec);

	template<class U, std::size_t Sz>
	friend std::wistream& operator >> (std::wistream& stream, Vector<U, Sz>& vec);
};

//////////////////////////////////////////////////////////////////////////////

// Friend Operators
namespace Epic
{
	template<class U, std::size_t Sz>
	bool operator == (const Vector<U, Sz>& vecA, const Vector<U, Sz>& vecB) noexcept
	{
		bool result = true;

		for (std::size_t i = 0; i < Sz; ++i)
			result &= (vecA[i] == vecB[i]);

		return result;
	}

	template<class U, std::size_t Sz>
	bool operator != (const Vector<U, Sz>& vecA, const Vector<U, Sz>& vecB) noexcept
	{
		bool result = true;

		for (std::size_t i = 0; i < Sz; ++i)
			result &= (vecA[i] != vecB[i]);

		return result;
	}

	template<class U, std::size_t Sz>
	std::ostream& operator << (std::ostream& stream, const Vector<U, Sz>& vec)
	{
		stream << '[';
		vec.ForEach([&](std::size_t n) 
		{ 
			if (n > 0) 
				stream << ", "; 
			stream << vec[n]; 
		});
		stream << ']';

		return stream;
	}

	template<class U, std::size_t Sz>
	std::wostream& operator << (std::wostream& stream, const Vector<U, Sz>& vec)
	{
		stream << L'[';
		vec.ForEach([&](std::size_t n) 
		{ 
			if (n > 0)
				stream << L", ";
			stream << vec[n]; 
		});
		stream << L']';

		return stream;
	}

	template<class U, std::size_t Sz>
	std::istream& operator >> (std::istream& stream, Vector<U, Sz>& vec)
	{
		if (stream.peek() == '[')
			stream.ignore(1);

		vec.ForEach([&](std::size_t n) 
		{ 
			if (n > 0 && stream.peek() == ',')
				stream.ignore(1);
			stream >> vec[n]; 
		});
		
		if (stream.peek() == ']')
			stream.ignore(1);

		return stream;
	}

	template<class U, std::size_t Sz>
	std::wistream& operator >> (std::wistream& stream, Vector<U, Sz>& vec)
	{
		if (stream.peek() == L'[')
			stream.ignore(1);

		vec.ForEach([&](std::size_t n) 
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
