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
	Vector() = default;
	Vector(const Type&) = default;
	Vector(Type&&) = default;

	Vector(std::initializer_list<T> values)
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

	explicit Vector(const T& value)
	{
		std::fill(std::begin(Values), std::end(Values), value);
	}

	template<class Arg, class... Args, 
		typename = std::enable_if_t<(detail::Span<Arg, Args...>::Value <= Size) && (detail::Span<Arg, Args...>::Value > 1)>>
	Vector(Arg&& arg, Args&&... args)
	{
		Construct(std::forward<Arg>(arg), std::forward<Args>(args)...);

		if (Epic::detail::Span<Arg, Args...>::Value < Size)
		{
			std::fill
			(
				std::next(std::begin(Values), Epic::detail::Span<Arg, Args...>::Value),
				std::end(Values),
				T{ 0 }
			);
		}
	}

public:
	T& at(size_t index)
	{
		assert(index >= 0 && index < Size);

		return Values[index];
	}

	const T& at(size_t index) const
	{
		assert(index >= 0 && index < Size);

		return Values[index];
	}

	T& operator[] (size_t index)
	{
		assert(index >= 0 && index < Size);

		return Values[index];
	}

	const T& operator[] (size_t index) const
	{
		assert(index >= 0 && index < Size);

		return Values[index];
	}

	decltype(std::begin(Values)) begin()
	{
		return std::begin(Values);
	}

	constexpr decltype(std::begin(Values)) begin() const
	{
		return std::begin(Values);
	}

	decltype(std::end(Values)) end()
	{
		return std::end(Values);
	}

	constexpr decltype(std::end(Values)) end() const
	{
		return std::end(Values);
	}

	constexpr size_t size() const
	{
		return Size;
	}
	
	decltype(std::data(Values)) data()
	{
		return std::data(Values);
	}

	constexpr decltype(std::data(Values)) data() const
	{
		return std::data(Values);
	}

public:
	// Calculate the dot product of this Vector and 'vec'
	template<class U>
	inline T Dot(const Vector<U, Size>& vec) const
	{
		T result = T(0);

		Epic::TMP::ForEach<Epic::TMP::MakeSequence<size_t, Size>>
			::Apply([&](size_t n) { result += Values[n] * vec[n]; });

		return result;
	}

	// Calculate the squared length of this vector
	inline T MagnitudeSq() const
	{
		return this->Dot(*this);
	}

	// Calculate the length of this Vector
	inline T Magnitude() const
	{
		return { std::sqrt(MagnitudeSq()) };
	}

	// Convert this Vector to a unit vector
	inline Type& Normalize()
	{
		return *this /= Magnitude();
	}

	// Force all values to the range [minV, maxV]
	inline Type& Clamp(const T& minV, const T& maxV)
	{
		Epic::TMP::ForEach<Epic::TMP::MakeSequence<size_t, Size>>
			::Apply([&](size_t n) { Values[n] = std::min(std::max(minV, Values[n]), maxV); });

		return *this;
	}

	// Raise all values to the power 'pow'
	inline Type& Power(const T& pow)
	{
		Epic::TMP::ForEach<Epic::TMP::MakeSequence<size_t, Size>>
			::Apply([&](size_t n) { Values[n] = std::pow(Values[n], pow); });

		return *this;
	}

	// Calculate the projection length of this Vector onto unit vector 'axis'
	template<typename U>
	inline T ProjectionMagnitude(const Vector<U, Size>& axis) const
	{
		return { this->Dot(axis) / axis.MagnitudeSq() };
	}

	// Calculate the 2d cross product of this Vector and 'vec' (The z-component of the cross product of the vectors)
	template<typename U, typename EnabledForVector2 = std::enable_if_t<(Size == 2)>>
	inline T Cross(const Vector<U, 2>& vec) const
	{
		return { Values[0] * vec[1] - Values[1] * vec[0] };
	}

	// Calculate the 3d cross product of this Vector and 'vec'
	template<typename U, typename EnabledForVector3OrLarger = std::enable_if_t<(Size >= 3)>>
	inline Type Cross(const Vector<U, Size>& vec) const
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
	inline Type Project(const Vector<U, Size>& axis) const
	{
		Vector<T, Size> result(axis);
		result *= this->ProjectionMagnitude(axis);
		return result;
	}

	// Calculate the projection of this Vector onto non-unit vector 'axis'
	template<typename U>
	inline Type ProjectN(const Vector<U, Size>& axis) const
	{
		Vector<U, Size> result = Vector<U, Size>::NormalOf(axis);
		result *= this->Dot(result);
		return result;
	}

	// Calculate the reflection of this Vector off of the plane that is partially defined by unit vector 'normal'
	template<typename U>
	inline Type Reflect(const Vector<U, Size>& normal) const
	{
		Vector<T, Size> n(normal);
		Vector<T, Size> result(*this);

		n *= T(2) * result.Dot(n);
		result -= n;
		
		return result;
	}

	// Calculate the refracted vector of this Vector off of the plane that is partially defined by unit vector 'normal'
	// 'eta' is the ratio of the refraction indexes
	template<typename U, typename EnabledForFloatTypes = std::enable_if_t<std::is_floating_point<T>::value>>
	inline Type Refract(const Vector<U, Size>& normal, const T eta) const
	{
		Vector<T, Size> I = Vector::NormalOf(*this);
		Vector<T, Size> N = normal;

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
	static inline Type NormalOf(const Type& vec)
	{
		Type result(vec);
		return result.Normalize();
	}

	// Calculate the linear interpolation of vectors 'vecA' and 'vecB' 
	template<typename EnabledForFloatTypes = std::enable_if_t<std::is_floating_point<T>::value>>
	static inline Type MixOf(const Type& vecA, const Type& vecB, const T w = T(0.5))
	{
		Vector<T, Size> result = A * (T(1) - w);
		result += B * w;
		return result;
	}

public:
	// Copy this Vector with negated values
	inline Type operator - (void) const
	{
		Type result;

		Epic::TMP::ForEach<Epic::TMP::MakeSequence<size_t, Size>>
			::Apply([&](size_t n) { result[n] = -Values[n]; });

		return result;
	}

	// Implicit conversion to T if this Vector contains only one value
	inline operator std::conditional_t<(Size == 1), T, struct OperationUnavailable>() const
	{
		return Values[0];
	}

public:
	#pragma region Assignment Operators
	#define CREATE_ASSIGNMENT_OPERATOR(Op)	\
																									\
	Type& operator Op (std::initializer_list<T> values)												\
	{																								\
		auto it = std::begin(values);																\
																									\
		Epic::TMP::ForEach<Epic::TMP::MakeSequence<size_t, Size>>::Apply(							\
			[&](size_t index)																		\
			{																						\
				if (it != std::end(values))															\
					m_Values[index] Op *it++;														\
			});																						\
																									\
		return *this;																				\
	}																								\
																									\
	template<class U, size_t Sz>																	\
	Type& operator Op (const Vector<U, Sz>& vec)													\
	{																								\
		Epic::TMP::ForEach2<																		\
			Epic::TMP::MakeSequence<size_t, Size>,													\
			Epic::TMP::MakeSequence<size_t, Sz>>													\
		::Apply([&](size_t iThis, size_t iOther) { Values[iThis] Op vec[iOther]; });				\
																									\
		return *this;																				\
	}																								\
																									\
	template<class VectorType, class TArray, size_t... Indices>										\
	Type& operator Op (const VectorSwizzler<VectorType, TArray, Indices...>& vec)					\
	{																								\
		Epic::TMP::ForEach2<																		\
			Epic::TMP::MakeSequence<size_t, Size>,													\
			Epic::TMP::Sequence<size_t, Indices...>>												\
		::Apply([&](size_t iThis, size_t iOther) { Values[iThis] Op vec.m_Values[iOther]; });		\
																									\
		return *this;																				\
	}																								\
																									\
	Type& operator Op (const T& value)																\
	{																								\
		Epic::TMP::ForEach<Epic::TMP::MakeSequence<size_t, Size>>									\
			::Apply([&](size_t index) { Values[index] Op value; });									\
																									\
		return *this;																				\
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
	inline Type operator Op (std::initializer_list<T> values) const									\
	{																								\
		Type result{ *this };																		\
		result Op= values;																			\
		return result;																				\
	}																								\
																									\
	template<class U, size_t Sz>																	\
	inline Type operator Op (const Vector<U, Sz>& vec) const										\
	{																								\
		Type result{ *this };																		\
		result Op= vec;																				\
		return result;																				\
	}																								\
																									\
	template<class VectorType, class TArray, size_t... Indices>										\
	inline Type operator Op (const VectorSwizzler<VectorType, TArray, Indices...>& vec) const		\
	{																								\
		Type result{ *this };																		\
		result Op= vec;																				\
		return result;																				\
	}																								\
																									\
	inline Type operator Op (const T& value)														\
	{																								\
		Type result{ *this };																		\
		result Op= value;																			\
		return result;																				\
	}																								\
																									\
	friend Type operator Op (const T& value, const Type& vec)										\
	{																								\
		Type result{ vec };																			\
		vec Op= value;																				\
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
	template<class... Vals>
	void Construct(Vals&&... vals)
	{
		ConstructAt(0, std::forward<Vals>(vals)...);
	}

	void ConstructAt(size_t) 
	{ 
		/* Do Nothing */
	}

	template<class Val, class... Vals>
	void ConstructAt(size_t offset, Val&& value, Vals&&... values)
	{
		PlaceAt(offset, std::forward<Val>(value));
		ConstructAt(offset + Epic::detail::Span<Val>::Value, std::forward<Vals>(values)...);
	}

	template<class Val>
	void PlaceAt(size_t offset, Val&& value)
	{
		Values[offset] = value;
	}

	template<class U, size_t Sz>
	void PlaceAt(size_t offset, Vector<U, Sz>& value)
	{
		Epic::TMP::ForEach<Epic::TMP::MakeSequence<size_t, Sz>>
			::Apply([&](size_t n) { Values[offset++] = value[n]; });
	}

	template<class U, size_t Sz>
	void PlaceAt(size_t offset, Vector<U, Sz>&& value)
	{
		Epic::TMP::ForEach<Epic::TMP::MakeSequence<size_t, Sz>>
			::Apply([&](size_t n) { Values[offset++] = value[n]; });
	}

	template<class VectorType, class TArray, size_t... Indices>
	void PlaceAt(size_t offset, VectorSwizzler<VectorType, TArray, Indices...>& value)
	{
		PlaceAt(offset, value.ToVector());
	}

	template<class VectorType, class TArray, size_t... Indices>
	void PlaceAt(size_t offset, VectorSwizzler<VectorType, TArray, Indices...>&& value)
	{
		PlaceAt(offset, value.ToVector());
	}

	template<class U, size_t N>
	void PlaceAt(size_t offset, const U(&value)[N])
	{
		Epic::TMP::ForEach<Epic::TMP::MakeSequence<size_t, N>>
			::Apply([&](size_t n) { Values[offset++] = value[n]; });
	}

	template<class U, size_t Sz>
	void PlaceAt(size_t offset, std::array<U, Sz>& value)
	{
		Epic::TMP::ForEach<Epic::TMP::MakeSequence<size_t, Sz>>
			::Apply([&](size_t n) { Values[offset++] = value[n]; });
	}

public:
	template<class U, size_t Sz>
	friend bool operator == (const Vector<U, Sz>& vecA, const Vector<U, Sz>& vecB);

	template<class U, size_t Sz>
	friend bool operator != (const Vector<U, Sz>& vecA, const Vector<U, Sz>& vecB);

	template<class U, size_t Sz>
	friend std::ostream& operator<< (std::ostream& stream, const Vector<U, Sz>& vec);

	template<class U, size_t Sz>
	friend std::wostream& operator<< (std::wostream& stream, const Vector<U, Sz>& vec);
};

//////////////////////////////////////////////////////////////////////////////

// Friend Operators
namespace Epic
{
	template<class U, size_t Sz>
	inline bool operator == (const Vector<U, Sz>& vecA, const Vector<U, Sz>& vecB)
	{
		bool result = true;

		for (size_t i = 0; i < Sz; ++i)
			result &= (vecA[i] == vecB[i]);

		return result;
	}

	template<class U, size_t Sz>
	inline bool operator != (const Vector<U, Sz>& vecA, const Vector<U, Sz>& vecB)
	{
		bool result = true;

		for (size_t i = 0; i < Sz; ++i)
			result &= (vecA[i] != vecB[i]);

		return result;
	}

	template<class U, size_t Sz>
	inline std::ostream& operator<< (std::ostream& stream, const Vector<U, Sz>& vec)
	{
		stream << '[';
		Epic::TMP::ForEach<Epic::TMP::MakeSequence<size_t, Sz>>::Apply(
			[&] (size_t n, const Vector<U, Sz>& v) 
			{ 
				if (n > 0)
					stream << ", "; 
				stream << vec[n]; 
			}, vec);
		stream << ']';

		return stream;
	}

	template<class U, size_t Sz>
	inline std::wostream& operator<< (std::wostream& stream, const Vector<U, Sz>& vec)
	{
		stream << L'[';
		Epic::TMP::ForEach<Epic::TMP::MakeSequence<size_t, Sz>>::Apply(
			[&] (size_t n, const Vector<U, Sz>& v) 
			{ 
				if (n > 0)
					stream << L", ";
				stream << vec[n]; 
			}, vec);
		stream << L']';

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
