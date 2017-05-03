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
#include <Epic/Math/detail/MathHelpers.hpp>
#include <Epic/Math/Angle.hpp>
#include <Epic/Math/Constants.hpp>
#include <Epic/Math/Vector.hpp>
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
	template<class U>
	friend class Epic::Quaternion;

public:
	using ValueType = T;
	constexpr static size_t Size = 4;

private:
	using Base::Values;

public:
	#pragma region Constructors

	// Constructs a quaternion with default initialized values
	Quaternion() noexcept = default;

	// Copy-constructs a quaternion
	Quaternion(const Type&) noexcept = default;

	// Move-constructs a quaternion
	Quaternion(Type&&) noexcept = default;

	// Copy-converts a quaternion
	template<class U>
	inline Quaternion(const Quaternion<U>& quat) noexcept
	{
		Values[0] = T(quat[0]);
		Values[1] = T(quat[1]);
		Values[2] = T(quat[2]);
		Values[3] = T(quat[3]);
	}

	// Constructs a quaternion from a list of values.
	template<class U>
	inline Quaternion(const U(&values)[4]) noexcept
	{
		Values[0] = values[0];
		Values[1] = values[1];
		Values[2] = values[2];
		Values[3] = values[3];
	}

	// Constructs with explicit values
	inline Quaternion(const T xv, const T yv, const T zv, const T wv) noexcept
	{
		Values[0] = xv;
		Values[1] = yv;
		Values[2] = zv;
		Values[3] = wv;
	}

	// Constructs an identity quaternion
	inline Quaternion(const IdentityTag&) noexcept
	{ 
		MakeIdentity();
	}

	// Constructs an X-axis rotation quaternion
	inline Quaternion(const XRotationTag&, const Radian<T>& phi) noexcept
	{
		MakeXRotation(phi);
	}

	// Constructs a Y-axis rotation quaternion
	inline Quaternion(const YRotationTag&, const Radian<T>& theta) noexcept
	{
		MakeYRotation(theta);
	}

	// Constructs a Z-axis rotation quaternion
	inline Quaternion(const ZRotationTag&, const Radian<T>& psi) noexcept
	{
		MakeZRotation(psi);
	}

	// Constructs a rotation quaternion from euler heading, pitch, and roll angles
	inline Quaternion(const Radian<T>& pitch, const Radian<T>& heading, const Radian<T>& roll) noexcept
	{
		MakeRotation(pitch, heading, roll);
	}

	// Constructs a rotation quaternion from an axis and angle
	inline Quaternion(const T& xv, const T& yv, const T& zv, const Radian<T>& angle) noexcept
	{
		MakeRotation(xv, yv, zv, angle);
	}

	// Constructs a rotation quaternion from an axis and angle
	template<size_t S, typename EnabledForVector3OrGreater = std::enable_if_t<(S >= 3)>>
	inline Quaternion(const Vector<T, S>& axis, const Radian<T>& angle) noexcept
	{
		MakeRotation(axis[0], axis[1], axis[2], angle);
	}

	#pragma endregion

public:
	#pragma region Range Accessors

	// Accesses the element at 'index'
	inline T& at(const size_t index) noexcept
	{
		assert(index >= 0 && index < Size);

		return Values[index];
	}

	// Accesses the element at 'index'
	inline const T& at(const size_t index) const noexcept
	{
		assert(index >= 0 && index < Size);

		return Values[index];
	}

	// Accesses the element at 'index'
	inline T& operator[] (const size_t index) noexcept
	{
		assert(index >= 0 && index < Size);

		return Values[index];
	}

	// Accesses the element at 'index'
	inline const T& operator[] (const size_t index) const noexcept
	{
		assert(index >= 0 && index < Size);

		return Values[index];
	}

	// Retrieves an iterator to the first element
	inline decltype(Values.begin()) begin() noexcept
	{
		return Values.begin();
	}

	// Retrieves an iterator to the first element
	constexpr decltype(Values.begin()) begin() const noexcept
	{
		return Values.begin();
	}

	// Retrieves an iterator to one past the last element
	constexpr decltype(Values.end()) end() const noexcept
	{
		return Values.end();
	}

	// Retrieves the number of elements
	constexpr size_t size() const noexcept
	{
		return Size;
	}

	// Retrieves a pointer to the underlying element data
	inline T* data() noexcept
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
	// Rotates a vector by this quaternion.
	template<size_t S, typename EnabledForVector3Or4 = std::enable_if_t<(S == 3) || (S == 4)>>
	inline void Transform(Vector<T, S>& vec) const noexcept
	{
		// vec.xyz = (*this * Quaternion{vec.xyz, 0} * ConjugateOf(*this)).xyz

		const auto t1 = Values[0] * T(2);
		const auto t2 = Values[1] * T(2);
		const auto t3 = Values[2] * T(2);
		const auto t4 = Values[0] * t1;
		const auto t5 = Values[1] * t2;
		const auto t6 = Values[2] * t3;
		const auto t7 = Values[0] * t2;
		const auto t8 = Values[0] * t3;
		const auto t9 = Values[1] * t3;
		const auto t10 = Values[3] * t1;
		const auto t11 = Values[3] * t2;
		const auto t12 = Values[3] * t3;

		const auto src = vec;
		vec[0] = (T(1) - (t5 + t6)) * src[0] + (t7 - t12) * src[1] + (t8 + t11) * src[2];
		vec[1] = (t7 + t12) * src[0] + (T(1) - (t4 + t6)) * src[1] + (t9 - t10) * src[2];
		vec[2] = (t8 - t11) * src[0] + (t9 + t10) * src[1] + (T(1) - (t4 + t5)) * src[2];
	}

public:
	// Sets values explicitly
	inline Type& Reset(const T xv, const T yv, const T zv, const T wv) noexcept
	{
		Values[0] = xv;
		Values[1] = yv;
		Values[2] = zv;
		Values[3] = wv;

		return *this;
	}

	// Sets this quaternion to an identity quaternion
	inline Type& MakeIdentity() noexcept
	{
		Values[0] = Values[1] = Values[2] = T(0);
		Values[3] = T(1);

		return *this;
	}

	// Sets this quaternion to an X-axis rotation quaternion
	inline Type& MakeXRotation(const Radian<T>& phi) noexcept
	{
		const Radian<T> a = phi / T(2);

		Values[0] = a.Sin();
		Values[1] = T(0);
		Values[2] = T(0);
		Values[3] = a.Cos();

		return *this;
	}

	// Sets this quaternion to a Y-axis rotation quaternion
	inline Type& MakeYRotation(const Radian<T>& theta) noexcept
	{
		const Radian<T> a = theta / T(2);

		Values[0] = T(0);
		Values[1] = a.Sin();
		Values[2] = T(0);
		Values[3] = a.Cos();

		return *this;
	}

	// Sets this quaternion to a Z-axis rotation quaternion
	inline Type& MakeZRotation(const Radian<T>& psi) noexcept
	{
		const Radian<T> a = psi / T(2);

		Values[0] = T(0);
		Values[1] = T(0);
		Values[2] = a.Sin();
		Values[3] = a.Cos();

		return *this;
	}

	// Sets this quaternion to a rotation quaternion using euler pitch, heading, and roll angles
	inline Type& MakeRotation(const Radian<T>& pitch, const Radian<T>& heading, const Radian<T>& roll) noexcept
	{
		const auto pitch_half = pitch / T(2);
		const auto heading_half = heading / T(2);
		const auto roll_half = roll / T(2);

		const auto cos_halfx = pitch_half.Cos();
		const auto cos_halfy = heading_half.Cos();
		const auto cos_halfz = roll_half.Cos();

		const auto sin_halfx = pitch_half.Sin();
		const auto sin_halfy = heading_half.Sin();
		const auto sin_halfz = roll_half.Sin();

		Values[0] = (cos_halfz * cos_halfy * sin_halfx) - (sin_halfz * sin_halfy * cos_halfx);
		Values[1] = (cos_halfz * sin_halfy * cos_halfx) + (sin_halfz * cos_halfy * sin_halfx);
		Values[2] = (sin_halfz * cos_halfy * cos_halfx) - (cos_halfz * sin_halfy * sin_halfx);
		Values[3] = (cos_halfz * cos_halfy * cos_halfx) + (sin_halfz * sin_halfy * sin_halfx);

		return *this;
	}

	// Sets this quaternion to a rotation quaternion using an axis and an angle
	inline Type& MakeRotation(const T& xv, const T& yv, const T& zv, const Radian<T>& angle) noexcept
	{
		auto t = xv * xv + yv * yv + zv * zv;

		if (t == T(0))
			return MakeIdentity();

		const auto a = angle / T(2);
		t = a.Sin() / std::sqrt(t);

		Values[0] = xv * t;
		Values[1] = yv * t;
		Values[2] = zv * t;
		Values[3] = a.Cos();

		return *this;
	}

	// Sets this quaternion to a rotation quaternion using an axis and an angle
	template<size_t S, typename EnabledForVector3OrGreater = std::enable_if_t<(S >= 3)>>
	inline Type& MakeRotation(const Vector<T, S>& axis, const Radian<T>& angle) noexcept
	{
		return MakeRotation(axis[0], axis[1], axis[2], angle);
	}

public:
	// Calculates the dot product of this quaternion and 'quat'
	inline T Dot(const Quaternion& quat) const noexcept
	{
		return (Values[0] * quat[0]) + 
			(Values[1] * quat[1]) + 
			(Values[2] * quat[2]) +
			(Values[3] * quat[3]);
	}

	// Calculates the squared length of this quaternion
	inline T MagnitudeSq() const noexcept
	{
		return this->Dot(*this);
	}

	// Calculates the length of this quaternion
	inline T Magnitude() const noexcept
	{
		return{ std::sqrt(MagnitudeSq()) };
	}

	// Converts this quaternion to a unit quaternion
	inline Type& Normalize() noexcept
	{
		return *this /= Magnitude();
	}

	// Converts this quaternion to a unit quaternion.
	// Returns unmodified quaternion if magnitude is 0.
	inline Type& NormalizeSafe() noexcept
	{
		const auto m = Magnitude();
		return (m == T(0)) ? (*this) : (*this /= m);
	}

	// Multiplies this quaternion with another. (Q' = Q * quat)
	inline Type& Concatenate(const Quaternion& quat) noexcept
	{
		const auto tx = Values[0];
		const auto ty = Values[1];
		const auto tz = Values[2];

		Values[0] = (ty * quat[2]) - (tz * quat[1]) + (Values[3] * quat[0]) + (tx * quat[3]);
		Values[1] = (tz * quat[0]) - (tx * quat[2]) + (Values[3] * quat[1]) + (ty * quat[3]);
		Values[2] = (tx * quat[1]) - (ty * quat[0]) + (Values[3] * quat[2]) + (tz * quat[3]);
		Values[3] = (Values[3] * quat[3]) - ((tx * quat[0]) + (ty * quat[1]) + (tz * quat[2]));

		return *this;
	}

	// Transforms this quaternion into its conjugate
	inline Type& Conjugate() noexcept
	{
		Values[0] = -Values[0];
		Values[1] = -Values[1];
		Values[2] = -Values[2];

		return *this;
	}

	// Inverts this quaternion.
	inline Type& Invert() noexcept
	{
		return (Conjugate() /= MagnitudeSq());
	}

public:
	// Calculates log(Q) = v*a, where Q = [x*sin(a), y*sin(a), z*sin(a), cos(a)]
	Type Log() const noexcept
	{
		const T z = T(0);
		const T a{ std::acos(Values[3]) };
		const T sina{ std::sin(a) };

		Type result(z, z, z, z);

		if (sina > z)
		{
			result[0] = a * Values[0] / sina;
			result[1] = a * Values[1] / sina;
			result[2] = a * Values[2] / sina;
		}

		return result;
	}

	// Calculates e^Q = exp(v*a) = [x*sin(a), y*sin(a), z*sin(a), cos(a)]
	Type Exp() const noexcept
	{
		const T z = T(0);
		const T a{ std::sqrt((Values[0] * Values[0]) + 
			(Values[1] * Values[1]) + 
							 (Values[2] * Values[2])) };
		const T sina{ std::sin(a) };
		const T cosa{ std::cos(a) };

		Type result(z, z, z, cosa);

		if (a > z)
		{
			result[0] = sina * Values[0] / a;
			result[1] = sina * Values[1] / a;
			result[2] = sina * Values[2] / a;
		}

		return result;
	}

public:
	// Calculates the axis of rotation
	inline Vector<T, 3> Axis() const noexcept
	{
		T t = T(1) - Values[3] * Values[3];
		if (t <= T(0))
			return Identity;

		t = T(1) / std::sqrt(t);
		return{ Values[0] * t, Values[1] * t, Values[2] * t };
	}

	// Calculates the angle of rotation
	inline Radian<T> Angle() const noexcept
	{
		return std::acos(Values[3]) * T(2);
	}

	// Calculates the pitch (X-axis) Euler angle of this quaternion
	inline Radian<T> Pitch() const noexcept
	{
		const T y = T(2) * (Values[1] * Values[2] + Values[3] * Values[0]);
		const T x = (Values[3] * Values[3]) - (Values[0] * Values[0]) - 
					(Values[1] * Values[1]) + (Values[2] * Values[2]);

		if(y == T(0) && x == T(0))
			return Radian<T>(static_cast<T>(T(2) * std::atan2(Values[0], Values[3])));

		return Radian<T>(static_cast<T>(std::atan2(y, x)));
	}

	// Calculates the heading (Y-axis) Euler angle of this quaternion
	inline Radian<T> Heading() const noexcept
	{
		return Radian<T>(static_cast<T>(std::asin(T(-2) * (Values[0] * Values[2] - Values[3] * Values[1]))));
	}

	// Calculates the roll (Z-axis) Euler angle of this quaternion
	inline Radian<T> Roll() const noexcept
	{
		const T x = (Values[3] * Values[3]) + (Values[0] * Values[0]) - 
			(Values[1] * Values[1]) - (Values[2] * Values[2]);
		const T y = T(2) * (Values[0] * Values[1] + Values[3] * Values[2]);

		return Radian<T>(static_cast<T>(std::atan2(y, x)));
	}

	// Calculates the pitch, heading, and roll (XYZ-axes) angles of this quaternion
	void Euler(Radian<T>& pitch, Radian<T>& heading, Radian<T>& roll) const noexcept
	{
		const T sqx = Values[0] * Values[0];
		const T sqy = Values[1] * Values[1];
		const T sqz = Values[2] * Values[2];
		const T sqw = Values[3] * Values[3];

		const T r11 = sqw + sqx - sqy - sqz;
		const T r21 = T(2) * (Values[0] * Values[1] + Values[3] * Values[2]);
		const T r31 = T(2) * (Values[0] * Values[2] - Values[3] * Values[1]);
		const T r32 = T(2) * (Values[1] * Values[2] + Values[3] * Values[0]);
		const T r33 = sqw - sqx - sqy + sqz;
		const T one = T(1) - Epsilon<T>;

		if (std::abs(r31) > one)
		{
			const T r12 = T(2) * (Values[0] * Values[1] - Values[3] * Values[2]);
			const T r13 = T(2) * (Values[0] * Values[2] + Values[3] * Values[1]);

			pitch = T(0);
			heading = -Epic::HalfPi<T> * r31 / std::abs(r31);
			roll = T(std::atan2(-r12, -r31 * r13));
		}
		else
		{
			pitch = T(std::atan2(r32, r33));
			heading = T(std::asin(-r31));
			roll = T(std::atan2(r21, r11));
		}
	}

	// Calculates the pitch, heading, and roll (XYZ-axes) angles of this quaternion
	void Euler(Degree<T>& pitch, Degree<T>& heading, Degree<T>& roll) const noexcept
	{
		Radian<T> p, h, r;
		Euler(p, h, r);

		pitch = p;
		heading = h;
		roll = r;
	}

public:
	// Calculates the normalized quaternion of 'quat'
	static inline Type NormalOf(const Type& quat) noexcept
	{
		Type result = quat;
		result.Normalize();

		return result;
	}

	// Calculates the normalized quaternion of 'quat'
	// Returns a copy of 'quat' if magnitude is 0
	static inline Type SafeNormalOf(const Type& quat) noexcept
	{
		Type result = quat;
		result.NormalizeSafe();

		return result;
	}

	// Calculates the concatenation of 'q' and 'r'
	static inline Type ConcatenationOf(const Type& q, const Type& r) noexcept
	{
		Type result = q;
		result.Concatenate(r);

		return result;
	}

	// Calculates the conjugate of 'quat'
	static inline Type ConjugateOf(const Type& quat) noexcept
	{
		Type result = quat;
		result.Conjugate();

		return result;
	}

	// Calculates the inverse of 'quat'
	static inline Type InverseOf(const Type& quat) noexcept
	{
		Type result = quat;
		result.Invert();

		return result;
	}

public:
	// Calculates the linear interpolation of normalized quaternions 'from' and 'to'
	template<typename EnabledForFloatingPoint = std::enable_if_t<std::is_floating_point<T>::value>>
	static inline Type Lerp(const Type& from, const Type& to, const T t) noexcept
	{
		return NormalOf((from * (T(1) - t)) + (to * t));
	}

	// Calculates the spherical linear interpolation of normalized quaternions 'from' and 'to'.
	// Reduces spinning by checking if 'from' and 'to' are more than 90 deg apart.
	template<typename EnabledForFloatingPoint = std::enable_if_t<std::is_floating_point<T>::value>>
	static Type SlerpSR(const Type& from, const Type& to, const T t) noexcept
	{
		Type qt = to;
		auto dot = from.Dot(to);

		// When from and to > 90 deg apart, perform spin reduction
		if (dot < T(0))
		{
			// Invert 'to'.  Since it's normalized, its conjugate will suffice 
			dot = -dot;
			qt.Conjugate();
		}

		// Linear interpolation when sin(acos(dot)) close to 0
		if(dot > T(1) - Epsilon<T>)
			return Lerp(from, to, t);

		// Spherical interpolation
		Radian<T> theta = acos(dot);
		Radian<T> thetaFrom = theta.Value() * (T(1) - t);
		Radian<T> thetaTo = theta.Value() * t;

		return ( (from * thetaFrom.Sin()) + (qt * thetaTo.Sin()) ) / theta.Sin();
	}

	// Calculates the spherical linear interpolation of normalized quaternions 'from' and 'to'
	template<typename EnabledForFloatingPoint = std::enable_if_t<std::is_floating_point<T>::value>>
	static inline Type Slerp(const Type& from, const Type& to, const T t) noexcept
	{
		auto dot = from.Dot(to);

		// Linear interpolation when sin(acos(dot)) close to 0
		if(dot > T(1) - Epsilon<T>)
			return Lerp(from, to, t);

		Radian<T> theta = acos(dot);
		Radian<T> thetaFrom = theta.Value() * (T(1) - t);
		Radian<T> thetaTo = theta.Value() * t;

		return ( (from * thetaFrom.Sin()) + (to * thetaTo.Sin()) ) / theta.Sin();
	}

	// Calculates the spherical cubic interpolation of normalized quaternions 'from', 'to', 'a', and 'b'
	template<typename EnabledForFloatingPoint = std::enable_if_t<std::is_floating_point<T>::value>>
	static inline Type Squad(const Type& from, const Type& to, const Type& a, const Type& b, const T t) noexcept
	{
		return Slerp(Slerp(from, to, t), Slerp(a, b, t), T(2) * t * (T(1) - t));
	}

public:
	// Calculates a conjugate quaternion
	inline Type operator - () const noexcept
	{
		return Type::ConjugateOf(*this);
	}

	// Calculates an inverted quaternion
	inline Type operator ~ () const noexcept
	{
		return Type::InverseOf(*this);
	}

public:
	#pragma region Assignment Operators

	// Sets this quaternion to an identity quaternion
	inline Type& operator = (const IdentityTag&) noexcept
	{
		return MakeIdentity();
	}

	// Concatenates this quaternion with 'quat'
	inline Type& operator *= (const Type& quat) noexcept
	{
		return Concatenate(quat);
	}

	// Concatenates this quaternion with the inverse of 'quat'
	inline Type& operator /= (const Type& quat) noexcept
	{
		// Concatenate(Type::InverseOf(quat))

		const auto magSq = quat.MagnitudeSq();

		const auto tqx = -quat[0] / magSq;
		const auto tqy = -quat[1] / magSq;
		const auto tqz = -quat[2] / magSq;
		const auto tqw = quat[3] / magSq;

		const auto tx = Values[0];
		const auto ty = Values[1];
		const auto tz = Values[2];
		const auto tw = Values[3];

		Values[0] = (ty * tqz) - (tz * tqy) + (tw * tqx) + (tx * tqw);
		Values[1] = (tz * tqx) - (tx * tqz) + (tw * tqy) + (ty * tqw);
		Values[2] = (tx * tqy) - (ty * tqx) + (tw * tqz) + (tz * tqw);
		Values[3] = (tw * tqw) - ((tx * tqx) + (ty * tqy) + (tz * tqz));

		return *this;
	}

	//////

	#define CREATE_SCALAR_ASSIGNMENT_OPERATOR(Op)											\
																							\
	inline Type& operator Op (const T& value) noexcept										\
	{																						\
		Values[0] Op value;																	\
		Values[1] Op value;																	\
		Values[2] Op value;																	\
		Values[3] Op value;																	\
																							\
		return *this;																		\
	}

	CREATE_SCALAR_ASSIGNMENT_OPERATOR(*= );
	CREATE_SCALAR_ASSIGNMENT_OPERATOR(/= );

	#undef CREATE_SCALAR_ASSIGNMENT_OPERATOR

	//////

	#define CREATE_ASSIGNMENT_OPERATOR(Op)													\
																							\
	template<class U>																		\
	inline Type& operator Op (const U(&values)[Size]) noexcept								\
	{																						\
		for (size_t i = 0; i < Size; ++i)													\
			Values[i] Op values[i];															\
																							\
		return *this;																		\
	}																						\
																							\
	inline Type& operator Op (const Type& quat) noexcept									\
	{																						\
		for (size_t i = 0; i < Size; ++i)													\
			Values[i] Op quat[i];															\
																							\
		return *this;																		\
	}

	CREATE_ASSIGNMENT_OPERATOR(= );
	CREATE_ASSIGNMENT_OPERATOR(+= );
	CREATE_ASSIGNMENT_OPERATOR(-= );

	#undef CREATE_ASSIGNMENT_OPERATOR

	#pragma endregion

public:
	#pragma region Arithmetic Operators

	// Returns the concatenation of this quaternion and 'quat'
	inline Type operator * (const Type& quat) const noexcept
	{
		Type result = *this;
		result *= quat;
		return result;
	}

	// Returns the concatenation of this quaternion and the inverse of 'quat'
	inline Type operator / (const Type& quat) const noexcept
	{
		Type result = *this;
		result /= quat;
		return result;
	}

	//////

	#define CREATE_SCALAR_ARITHMETIC_OPERATOR(Op) 										\
																						\
	inline Type operator Op (const T& value) const noexcept								\
	{																					\
		Type result = *this;															\
		result Op= value;																\
		return result;																	\
	}																					\
																						\
	friend inline Type operator Op (const T& value, const Type& quat) noexcept			\
	{																					\
		return																			\
		{																				\
			value Op quat[0],															\
			value Op quat[1],															\
			value Op quat[2],															\
			value Op quat[3]															\
		};																				\
	}

	CREATE_SCALAR_ARITHMETIC_OPERATOR(*);
	CREATE_SCALAR_ARITHMETIC_OPERATOR(/);

	#undef CREATE_SCALAR_ARITHMETIC_OPERATOR

	//////

	#define CREATE_ARITHMETIC_OPERATOR(Op) 	\
																									\
	template<class U>																				\
	inline Type operator Op (const U(&values)[Size]) const noexcept									\
	{																								\
		Type result = *this;																		\
		result Op= values;																			\
		return result;																				\
	}																								\
																									\
	inline Type operator Op (const Type& quat) const noexcept										\
	{																								\
		Type result = *this;																		\
		result Op= quat;																			\
		return result;																				\
	}

	CREATE_ARITHMETIC_OPERATOR(+);
	CREATE_ARITHMETIC_OPERATOR(-);

	#undef CREATE_ARITHMETIC_OPERATOR

	#pragma endregion

public:
	template<class U>
	friend bool operator == (const Quaternion<U>& quatA, const Quaternion<U>& quatB) noexcept;

	template<class U>
	friend bool operator != (const Quaternion<U>& quatA, const Quaternion<U>& quatB) noexcept;

	template<class U>
	friend std::ostream& operator << (std::ostream& stream, const Quaternion<U>& quat);

	template<class U>
	friend std::wostream& operator << (std::wostream& stream, const Quaternion<U>& quat);

	template<class U>
	friend std::istream& operator >> (std::istream& stream, Quaternion<U>& quat);

	template<class U>
	friend std::wistream& operator >> (std::wistream& stream, Quaternion<U>& quat);
};

//////////////////////////////////////////////////////////////////////////////

// Friend Operators
namespace Epic
{
	template<class U>
	inline bool operator == (const Quaternion<U>& quatA, const Quaternion<U>& quatB) noexcept
	{
		return (quatA[0] == quatB[0]) && (quatA[1] == quatB[1]) && (quatA[2] == quatB[2]) && (quatA[3] == quatB[3]);
	}

	template<class U>
	inline bool operator != (const Quaternion<U>& quatA, const Quaternion<U>& quatB) noexcept
	{
		return !((quatA[0] == quatB[0]) && (quatA[1] == quatB[1]) && (quatA[2] == quatB[2]) && (quatA[3] == quatB[3]));
	}

	template<class U>
	inline std::ostream& operator << (std::ostream& stream, const Quaternion<U>& quat)
	{
		stream << '[' 
			<< quat[0] << ", "
			<< quat[1] << ", "
			<< quat[2] << ", "
			<< quat[3]
			<< ']';

		return stream;
	}

	template<class U>
	inline std::wostream& operator << (std::wostream& stream, const Quaternion<U>& quat)
	{
		stream << L'['
			<< quat[0] << L", "
			<< quat[1] << L", "
			<< quat[2] << L", "
			<< quat[3]
			<< L']';

		return stream;
	}

	template<class U>
	inline std::istream& operator >> (std::istream& stream, Quaternion<U>& quat)
	{
		if (stream.peek() == '[')
			stream.ignore(1);

		stream >> quat[0];
		if (stream.peek() == ',') stream.ignore(1);
		stream >> quat[1];
		if (stream.peek() == ',') stream.ignore(1);
		stream >> quat[2];
		if (stream.peek() == ',') stream.ignore(1);
		stream >> quat[3];

		if (stream.peek() == ']')
			stream.ignore(1);

		return stream;
	}

	template<class U>
	inline std::wistream& operator >> (std::wistream& stream, Quaternion<U>& quat)
	{
		if (stream.peek() == L'[')
			stream.ignore(1);

		stream >> quat[0];
		if (stream.peek() == L',') stream.ignore(1);
		stream >> quat[1];
		if (stream.peek() == L',') stream.ignore(1);
		stream >> quat[2];
		if (stream.peek() == L',') stream.ignore(1);
		stream >> quat[3];

		if (stream.peek() == L']')
			stream.ignore(1);

		return stream;
	}
}

//////////////////////////////////////////////////////////////////////////////

// Vector * Quaternion operators
namespace Epic
{
	template<class T, size_t S>
	inline Vector<T, S>& Epic::Vector<T, S>::operator *= (const Epic::Quaternion<T>& quat) noexcept
	{
		quat.Transform(*this);
		return *this;
	}

	template<class T, size_t S>
	inline auto operator * (const Vector<T, S>& vec, const Quaternion<T>& quat) noexcept
	{
		auto result = vec;
		quat.Transform(result);
		return result;
	}
}

//////////////////////////////////////////////////////////////////////////////

// Aliases
namespace Epic
{
	using Quaternionf = Quaternion<float>;
	using Quaterniond = Quaternion<double>;

	using QuaternionF = Quaternionf;
	using QuaternionD = Quaterniond;
}
