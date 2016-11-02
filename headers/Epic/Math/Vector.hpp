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
#include <Epic/Math/VectorSwizzler.hpp>
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
	using ValueType = T;
	constexpr static size_t Size = S;

private:
	using Base::Values;

public:
	Vector() noexcept = default;
	Vector(const Type&) noexcept = default;
	Vector(Type&&) noexcept = default;

	template<class U>
	inline Vector(const Vector<U, Size>& vec)
	{
		ForEach<Size>([&](size_t n) { Values[n] = static_cast<T>(vec[n]); });
	}

	inline Vector(std::initializer_list<T> values) noexcept
	{
		std::copy
		(
			std::begin(values),
			std::next(std::begin(values), std::min(values.size(), Size)),
			std::begin(Values)
		);

		if (values.size() < Size)
			std::fill(std::next(std::begin(Values), values.size()), std::end(Values), T{ 0 });
	}

	inline explicit Vector(const T& value) noexcept
	{
		std::fill(std::begin(Values), std::end(Values), value);
	}

	template<class Arg, class... Args,
			 typename = std::enable_if_t<(detail::Span<Arg, Args...>::Value == Size)>>
	inline Vector(Arg&& arg, Args&&... args) noexcept
	{
		Construct(std::forward<Arg>(arg), std::forward<Args>(args)...);
	}

public:
	inline T& at(const size_t index) noexcept
	{
		assert(index >= 0 && index < Size);

		return Values[index];
	}

	inline const T& at(const size_t index) const noexcept
	{
		assert(index >= 0 && index < Size);

		return Values[index];
	}

	inline T& operator[] (const size_t index) noexcept
	{
		assert(index >= 0 && index < Size);

		return Values[index];
	}

	inline const T& operator[] (const size_t index) const noexcept
	{
		assert(index >= 0 && index < Size);

		return Values[index];
	}

	inline decltype(std::begin(Values)) begin() noexcept
	{
		return std::begin(Values);
	}

	constexpr decltype(std::begin(Values)) begin() const noexcept
	{
		return std::begin(Values);
	}

	inline decltype(std::end(Values)) end() noexcept
	{
		return std::end(Values);
	}

	constexpr decltype(std::end(Values)) end() const noexcept
	{
		return std::end(Values);
	}

	constexpr size_t size() const noexcept
	{
		return Size;
	}
	
	inline decltype(std::data(Values)) data() noexcept
	{
		return std::data(Values);
	}

	constexpr decltype(std::data(Values)) data() const noexcept
	{
		return std::data(Values);
	}

public:
	// Calculate the dot product of this Vector and 'vec'
	template<class U>
	inline T Dot(const Vector<U, Size>& vec) const noexcept
	{
		T result = T(0);
		ForEach<Size>([&](size_t n) { result += Values[n] * vec[n]; });
		return result;
	}

	// Calculate the squared length of this vector
	inline T MagnitudeSq() const noexcept
	{
		return this->Dot(*this);
	}

	// Calculate the length of this Vector
	inline T Magnitude() const noexcept
	{
		return { std::sqrt(MagnitudeSq()) };
	}

	// Convert this Vector to a unit vector
	inline Type& Normalize() noexcept
	{
		return *this /= Magnitude();
	}

	// Convert this Vector to a unit vector (returns unmodified vector if magnitude is 0)
	inline Type& NormalizeSafe() noexcept
	{
		const auto m = Magnitude();
		return (m == T(0)) ? (*this) : (*this /= m);
	}

	// Force all values to the range [minValue, maxValue]
	inline Type& Clamp(const T& minValue, const T& maxValue) noexcept
	{
		ForEach<Size>([&](size_t n) { Values[n] = std::min(std::max(minValue, Values[n]), maxValue); });
		return *this;
	}

	// Raise all values to the power 'pow'
	inline Type& Power(const T& pow) noexcept
	{
		ForEach<Size>([&](size_t n) { Values[n] = std::pow(Values[n], pow); });
		return *this;
	}

	// Calculate the projection length of this Vector onto unit vector 'axis'
	template<typename U>
	inline T ProjectionMagnitude(const Vector<U, Size>& axis) const noexcept
	{
		return { this->Dot(axis) / axis.MagnitudeSq() };
	}

	// Calculate the 2d cross product of this Vector and 'vec' (The z-component of the cross product of the vectors)
	template<typename U, typename EnabledForVector2 = std::enable_if_t<(Size == 2)>>
	constexpr T Cross(const Vector<U, 2>& vec) const noexcept
	{
		return { Values[0] * vec[1] - Values[1] * vec[0] };
	}

	// Calculate the 3d cross product of this Vector and 'vec'
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

	// Calculate the projection of this Vector onto unit vector 'axis'
	template<typename U>
	inline Type Project(const Vector<U, Size>& axis) const noexcept
	{
		Vector<T, Size> result(axis);
		result *= this->ProjectionMagnitude(axis);
		return result;
	}

	// Calculate the projection of this Vector onto non-unit vector 'axis'
	template<typename U>
	inline Type ProjectN(const Vector<U, Size>& axis) const noexcept
	{
		Vector<U, Size> result = Vector<U, Size>::NormalOf(axis);
		result *= this->Dot(result);
		return result;
	}

	// Calculate the reflection of this Vector off of the plane that is partially defined by unit vector 'normal'
	template<typename U>
	inline Type Reflect(const Vector<U, Size>& normal) const noexcept
	{
		Type n(normal);
		Type result(*this);

		n *= T(2) * result.Dot(n);
		result -= n;
		
		return result;
	}

	// Calculate the refracted vector of this Vector off of the plane that is partially defined by unit vector 'normal'
	// 'eta' is the ratio of the refraction indexes
	template<typename U, typename EnabledForFloatTypes = std::enable_if_t<std::is_floating_point<T>::value>>
	inline Type Refract(const Vector<U, Size>& normal, const T eta) const noexcept
	{
		Type I = Vector::NormalOf(*this);
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
	// Calculate the normalized vector of 'vec'
	static inline Type NormalOf(const Type& vec) noexcept
	{
		Type result(vec);
		return result.Normalize();
	}

	// Calculate the normalized vector of 'vec' (returns 'vec' if magnitude is 0)
	static inline Type SafeNormalOf(const Type& vec) noexcept
	{
		Type result(vec);
		return result.NormalizeSafe();
	}

	// Calculate the linear interpolation of vectors 'vecA' and 'vecB' 
	template<typename EnabledForFloatTypes = std::enable_if_t<std::is_floating_point<T>::value>>
	static inline Type MixOf(const Type& vecA, const Type& vecB, const T w = T(0.5)) noexcept
	{
		Vector<T, Size> result = A * (T(1) - w);
		result += B * w;
		return result;
	}

public:
	// Copy this Vector with negated values
	inline Type operator - () const noexcept
	{
		Type result;
		ForEach<Size>([&](size_t n) { result[n] = -Values[n]; });
		return result;
	}

	// Implicit conversion to T if this Vector contains only one value
	inline operator std::conditional_t<(Size == 1), T, struct OperationUnavailable>() const noexcept
	{
		return Values[0];
	}

public:
	#pragma region Assignment Operators
	#define CREATE_ASSIGNMENT_OPERATOR(Op)	\
																							\
	inline Type& operator Op (std::initializer_list<T> values) noexcept						\
	{																						\
		auto it = std::begin(values);														\
																							\
		ForEach<Size>([&](size_t index)														\
		{																					\
			if (it != std::end(values))														\
				m_Values[index] Op *it++;													\
		});																					\
																							\
		return *this;																		\
	}																						\
																							\
	inline Type& operator Op (const Type& vec) noexcept										\
	{																						\
		ForEach<Size>([&](size_t index) { Values[index] Op vec[index]; });					\
		return *this;																		\
	}																						\
																							\
	template<class U, size_t Sz>															\
	inline Type& operator Op (const Vector<U, Sz>& vec) noexcept							\
	{																						\
		Epic::TMP::ForEach2<																\
			Epic::TMP::MakeSequence<size_t, Size>,											\
			Epic::TMP::MakeSequence<size_t, Sz>>											\
		::Apply([&](size_t iThis, size_t iOther)											\
		{																					\
			Values[iThis] Op vec[iOther];													\
		});																					\
																							\
		return *this;																		\
	}																						\
																							\
	template<class VectorT, class TArray, size_t... Is>										\
	inline Type& operator Op (const VectorSwizzler<VectorT, TArray, Is...>& vec) noexcept	\
	{																						\
		Epic::TMP::ForEach2<																\
			Epic::TMP::MakeSequence<size_t, Size>,											\
			Epic::TMP::Sequence<size_t, Is...>>												\
		::Apply([&](size_t iThis, size_t iOther)											\
		{																					\
			Values[iThis] Op vec.m_Values[iOther];											\
		});																					\
																							\
		return *this;																		\
	}																						\
																							\
	inline Type& operator Op (const T& value) noexcept										\
	{																						\
		ForEach<Size>([&](size_t index) { Values[index] Op value; });						\
		return *this;																		\
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
	inline Type operator Op (std::initializer_list<T> values) const	noexcept						\
	{																								\
		Type result{ *this };																		\
		result Op= values;																			\
		return result;																				\
	}																								\
																									\
	inline Type operator Op (const Type& vec) const noexcept										\
	{																								\
		Type result{ *this };																		\
		result Op= vec;																				\
		return result;																				\
	}																								\
																									\
	template<class U, size_t Sz>																	\
	inline Type operator Op (const Vector<U, Sz>& vec) const noexcept								\
	{																								\
		Type result{ *this };																		\
		result Op= vec;																				\
		return result;																				\
	}																								\
																									\
	template<class VectorT, class TArray, size_t... Is>												\
	inline Type operator Op (const VectorSwizzler<VectorT, TArray, Is...>& vec) const noexcept		\
	{																								\
		Type result{ *this };																		\
		result Op= vec;																				\
		return result;																				\
	}																								\
																									\
	inline Type operator Op (const T& value) const noexcept											\
	{																								\
		Type result{ *this };																		\
		result Op= value;																			\
		return result;																				\
	}																								\
																									\
	friend inline Type operator Op (const T& value, const Type& vec) noexcept						\
	{																								\
		Type result{ vec };																			\
		result Op= value;																			\
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
	template<size_t N, class Function>
	inline void ForEach(Function fn) noexcept
	{
		Epic::TMP::ForEach<Epic::TMP::MakeSequence<size_t, N>>::Apply(fn);
	}

	template<size_t N, class Function>
	inline void ForEach(Function fn) const noexcept
	{
		Epic::TMP::ForEach<Epic::TMP::MakeSequence<size_t, N>>::Apply(fn);
	}

	template<class... Vals>
	inline void Construct(Vals&&... vals) noexcept
	{
		ConstructAt(0, std::forward<Vals>(vals)...);
	}

	inline void ConstructAt(size_t) noexcept
	{ 
		/* Do Nothing */
	}

	template<class Val, class... Vals>
	inline void ConstructAt(size_t offset, Val&& value, Vals&&... values) noexcept
	{
		PlaceAt(offset, std::forward<Val>(value));
		ConstructAt(offset + Epic::detail::Span<Val>::Value, std::forward<Vals>(values)...);
	}

	template<class Val>
	inline void PlaceAt(size_t offset, Val&& value) noexcept
	{
		Values[offset] = value;
	}

	template<class U, size_t Sz>
	inline void PlaceAt(size_t offset, Vector<U, Sz>& value) noexcept
	{
		ForEach<Sz>([&](size_t n) { Values[offset++] = value[n]; });
	}

	template<class U, size_t Sz>
	inline void PlaceAt(size_t offset, Vector<U, Sz>&& value) noexcept
	{
		ForEach<Sz>([&](size_t n) { Values[offset++] = value[n]; });
	}

	template<class VectorT, class TArray, size_t... Is>
	inline void PlaceAt(size_t offset, VectorSwizzler<VectorT, TArray, Is...>& value) noexcept
	{
		PlaceAt(offset, value.ToVector());
	}

	template<class VectorT, class TArray, size_t... Is>
	inline void PlaceAt(size_t offset, VectorSwizzler<VectorT, TArray, Is...>&& value) noexcept
	{
		PlaceAt(offset, value.ToVector());
	}

	template<class U, size_t N>
	inline void PlaceAt(size_t offset, const U(&value)[N]) noexcept
	{
		ForEach<N>([&](size_t n) { Values[offset++] = value[n]; });
	}

	template<class U, size_t Sz>
	void inline PlaceAt(size_t offset, std::array<U, Sz>& value) noexcept
	{
		ForEach<Sz>([&](size_t n) { Values[offset++] = value[n]; });
	}

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
		vec.ForEach<Sz>([&](size_t n) 
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
		vec.ForEach<Sz>([&](size_t n) 
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

		vec.ForEach<Sz>([&](size_t n) 
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

		vec.ForEach<Sz>([&](size_t n) 
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
