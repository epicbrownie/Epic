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
#include <tuple>
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
	using value_type = typename Base::value_type;
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
	template<class U, typename = std::enable_if_t<std::is_convertible_v<U, T>>>
	Quaternion(const Quaternion<U>& quat) noexcept
	{ 
		for (size_t i = 0; i < Size; ++i)
			Values[i] = static_cast<T>(quat.Values[i]);
	}

	// Constructs a quaternion from a list of convertible values.
	template<class U, typename = std::enable_if_t<std::is_convertible_v<U, T>>>
	Quaternion(const U(&values)[Size]) noexcept
	{ 
		for (size_t i = 0; i < Size; ++i)
			Values[i] = static_cast<T>(values[i]);
	}

	// Constructs with explicit values
	Quaternion(T xv, T yv, T zv, T wv) noexcept
	{
		Values[0] = std::move(xv);
		Values[1] = std::move(yv);
		Values[2] = std::move(zv);
		Values[3] = std::move(wv);
	}

	// Constructs an identity quaternion
	Quaternion(const IdentityTag&) noexcept
	{ 
		MakeIdentity();
	}

	// Constructs an X-axis rotation quaternion
	Quaternion(const XRotationTag&, Radian<T> phi) noexcept
	{
		MakeXRotation(std::move(phi));
	}

	// Constructs a Y-axis rotation quaternion
	Quaternion(const YRotationTag&, Radian<T> theta) noexcept
	{
		MakeYRotation(std::move(theta));
	}

	// Constructs a Z-axis rotation quaternion
	Quaternion(const ZRotationTag&, Radian<T> psi) noexcept
	{
		MakeZRotation(std::move(psi));
	}

	// Constructs a rotation quaternion from euler heading, pitch, and roll angles
	Quaternion(Radian<T> pitch, Radian<T> heading, Radian<T> roll) noexcept
	{
		MakeRotation(std::move(pitch), std::move(heading), std::move(roll));
	}

	// Constructs a rotation quaternion from an axis and angle
	Quaternion(T xv, T yv, T zv, Radian<T> angle) noexcept
	{
		MakeRotation(std::move(xv), std::move(yv), std::move(zv), std::move(angle));
	}

	// Constructs a rotation quaternion from an axis and angle
	Quaternion(Vector<T, 3> axis, Radian<T> angle) noexcept
	{
		MakeRotation(std::move(axis.x), std::move(axis.y), std::move(axis.z), std::move(angle));
	}

	// Constructs a rotation quaternion from an axis and angle
	Quaternion(Vector<T, 4> axis, Radian<T> angle) noexcept
	{
		MakeRotation(std::move(axis.x), std::move(axis.y), std::move(axis.z), std::move(angle));
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
	T at(size_t index) const noexcept
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
	T operator[] (size_t index) const noexcept
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

private:
	template<size_t S>
	inline void _Transform(Vector<T, S>& vec) const noexcept
	{
		// vec.xyz = (*this * Quaternion(vec.xyz, 0) * ConjugateOf(*this)).xyz

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
		const auto s = vec;

		vec.x = (T(1) - (t5 + t6)) * s.x + (t7 - t12) * s.y + (t8 + t11) * s.z;
		vec.y = (t7 + t12) * s.x + (T(1) - (t4 + t6)) * s.y + (t9 - t10) * s.z;
		vec.z = (t8 - t11) * s.x + (t9 + t10) * s.y + (T(1) - (t4 + t5)) * s.z;
	}

public:
	// Rotates a vector3 by this quaternion.
	void Transform(Vector<T, 3>& vec) const noexcept
	{
		_Transform(vec);
	}

	// Rotates a vector4 by this quaternion.
	void Transform(Vector<T, 4>& vec) const noexcept
	{
		_Transform(vec);
	}

public:
	// Sets values explicitly
	Type& Reset(T xv, T yv, T zv, T wv) noexcept
	{
		return *this = { std::move(xv), std::move(yv), std::move(zv), std::move(wv) };
	}

	// Sets this quaternion to an identity quaternion
	Type& MakeIdentity() noexcept
	{
		const auto z = T(0);
		return *this = { z, z, z, T(1) };
	}

	// Sets this quaternion to an X-axis rotation quaternion
	Type& MakeXRotation(Radian<T> phi) noexcept
	{
		const auto[sPhi, cPhi] = (phi / T(2)).SinCos();
		return *this = { sPhi, T(0), T(0), cPhi };
	}

	// Sets this quaternion to a Y-axis rotation quaternion
	Type& MakeYRotation(Radian<T> theta) noexcept
	{
		const auto[sTheta, cTheta] = (theta / T(2)).SinCos();
		return *this = { T(0), sTheta, T(0), cTheta };
	}

	// Sets this quaternion to a Z-axis rotation quaternion
	Type& MakeZRotation(Radian<T> psi) noexcept
	{
		const auto[sPsi, cPsi] = (psi / T(2)).SinCos();
		return *this = { T(0), T(0), sPsi, cPsi };
	}

	// Sets this quaternion to a rotation quaternion using euler pitch, heading, and roll angles
	Type& MakeRotation(Radian<T> pitch, Radian<T> heading, Radian<T> roll) noexcept
	{
		const auto[sp, cp] = (pitch / T(2)).SinCos();
		const auto[sh, ch] = (heading / T(2)).SinCos();
		const auto[sr, cr] = (roll / T(2)).SinCos();

		return *this = 
		{
			(cr * ch * sp) - (sr * sh * cp)
			(cr * sh * cp) + (sr * ch * sp)
			(sr * ch * cp) - (cr * sh * sp)
			(cr * ch * cp) + (sr * sh * sp)
		};
	}

	// Sets this quaternion to a rotation quaternion using an axis and an angle
	Type& MakeRotation(T xv, T yv, T zv, Radian<T> angle) noexcept
	{
		auto t = xv * xv + yv * yv + zv * zv;
		if (t == T(0))
			return MakeIdentity();

		const auto[sAngle, cAngle] = (angle / T(2)).SinCos();
		t = sAngle / static_cast<T>(std::sqrt(t));

		return *this = { xv * t, yv * t, zv * t, cAngle };
	}

	// Sets this quaternion to a rotation quaternion using an axis and an angle
	Type& MakeRotation(Vector<T, 3> axis, Radian<T> angle) noexcept
	{
		return MakeRotation(std::move(axis.x), std::move(axis.y), std::move(axis.z), std::move(angle));
	}

	// Sets this quaternion to a rotation quaternion using an axis and an angle
	Type& MakeRotation(Vector<T, 4> axis, Radian<T> angle) noexcept
	{
		return MakeRotation(std::move(axis.x), std::move(axis.y), std::move(axis.z), std::move(angle));
	}

public:
	// Calculates the dot product of this quaternion and 'quat'
	constexpr T Dot(Quaternion quat) const noexcept
	{
		return (Values[0] * quat.Values[0]) + 
			   (Values[1] * quat.Values[1]) + 
			   (Values[2] * quat.Values[2]) +
			   (Values[3] * quat.Values[3]);
	}

	// Calculates the squared length of this quaternion
	constexpr T MagnitudeSq() const noexcept
	{
		return Dot(*this);
	}

	// Calculates the length of this quaternion
	T Magnitude() const noexcept
	{
		return { static_cast<T>(std::sqrt(MagnitudeSq())) };
	}

	// Converts this quaternion to a unit quaternion
	Type& Normalize() noexcept
	{
		return *this /= Magnitude();
	}

	// Converts this quaternion to a unit quaternion.
	// Returns unmodified quaternion if magnitude is 0.
	Type& NormalizeSafe() noexcept
	{
		const auto m = Magnitude();
		return (m == T(0)) ? (*this) : (*this /= m);
	}

	// Multiplies this quaternion with another. (Q' = Q * quat)
	Type& Concatenate(Type quat) noexcept
	{
		const auto tx = Values[0];
		const auto ty = Values[1];
		const auto tz = Values[2];
		
		Values[0] = (ty * quat.Values[2]) - (tz * quat.Values[1]) + (Values[3] * quat.Values[0]) + (tx * quat.Values[3]);
		Values[1] = (tz * quat.Values[0]) - (tx * quat.Values[2]) + (Values[3] * quat.Values[1]) + (ty * quat.Values[3]);
		Values[2] = (tx * quat.Values[1]) - (ty * quat.Values[0]) + (Values[3] * quat.Values[2]) + (tz * quat.Values[3]);
		Values[3] = (Values[3] * quat.Values[3]) - ((tx * quat.Values[0]) + (ty * quat.Values[1]) + (tz * quat.Values[2]));

		return *this;
	}

	// Transforms this quaternion into its conjugate
	Type& Conjugate() noexcept
	{
		Values[0] = -Values[0];
		Values[1] = -Values[1];
		Values[2] = -Values[2];

		return *this;
	}

	// Inverts this quaternion.
	Type& Invert() noexcept
	{
		return (Conjugate() /= MagnitudeSq());
	}

public:
	// Calculates log(Q) = v*a, where Q = [x*sin(a), y*sin(a), z*sin(a), cos(a)]
	Type Log() const noexcept
	{
		const T a = static_cast<T>(std::acos(Values[3]));
		const T sina = static_cast<T>(std::sin(a));
		const T z = T(0);

		Type result(z, z, z, z);

		if (sina > z)
		{
			result.Values[0] = a * Values[0] / sina;
			result.Values[1] = a * Values[1] / sina;
			result.Values[2] = a * Values[2] / sina;
		}

		return result;
	}

	// Calculates e^Q = exp(v*a) = [x*sin(a), y*sin(a), z*sin(a), cos(a)]
	Type Exp() const noexcept
	{
		const T a = static_cast<T>(
			std::sqrt((Values[0] * Values[0]) +
					  (Values[1] * Values[1]) +
					  (Values[2] * Values[2])));

		const T sina = static_cast<T>(std::sin(a));
		const T cosa = static_cast<T>(std::cos(a));
		const T z = T(0);

		Type result(z, z, z, cosa);

		if (a > z)
		{
			result.Values[0] = sina * Values[0] / a;
			result.Values[1] = sina * Values[1] / a;
			result.Values[2] = sina * Values[2] / a;
		}

		return result;
	}

public:
	// Calculates the axis of rotation
	Vector<T, 3> Axis() const noexcept
	{
		T t = T(1) - Values[3] * Values[3];
		if (t <= T(0)) return Identity;

		t = T(1) / static_cast<T>(std::sqrt(t));
		return Vector<T, 3>(Values[0] * t, Values[1] * t, Values[2] * t);
	}

	// Calculates the angle of rotation
	Radian<T> Angle() const noexcept
	{
		return static_cast<T>(std::acos(Values[3])) * T(2);
	}

	// Calculates the pitch (X-axis) Euler angle of this quaternion
	Radian<T> Pitch() const noexcept
	{
		const T y = T(2) * (Values[1] * Values[2] + Values[3] * Values[0]);
		const T x = (Values[3] * Values[3]) - (Values[0] * Values[0]) - 
					(Values[1] * Values[1]) + (Values[2] * Values[2]);

		if(y == T(0) && x == T(0))
			return Radian<T>(static_cast<T>(std::atan2(Values[0], Values[3])) * T(2));

		return Radian<T>(static_cast<T>(std::atan2(y, x)));
	}

	// Calculates the heading (Y-axis) Euler angle of this quaternion
	Radian<T> Heading() const noexcept
	{
		return Radian<T>(static_cast<T>(std::asin(T(-2) * (Values[0] * Values[2] - Values[3] * Values[1]))));
	}

	// Calculates the roll (Z-axis) Euler angle of this quaternion
	Radian<T> Roll() const noexcept
	{
		const T x = (Values[3] * Values[3]) + (Values[0] * Values[0]) - 
			(Values[1] * Values[1]) - (Values[2] * Values[2]);
		const T y = T(2) * (Values[0] * Values[1] + Values[3] * Values[2]);

		return Radian<T>(static_cast<T>(std::atan2(y, x)));
	}

	// Calculates the pitch, heading, and roll (XYZ-axes) angles of this quaternion
	std::tuple<Radian<T>, Radian<T>, Radian<T>> Euler() const noexcept
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
		const T oneish = T(1) - Epsilon<T>;

		if (std::abs(r31) > oneish)
		{
			const T r12 = T(-2) * (Values[0] * Values[1] - Values[3] * Values[2]);
			const T r13 = T(-2) * (Values[0] * Values[2] + Values[3] * Values[1]);

			return std::make_tuple
			(
				Radian<T>(T(0)),
				Radian<T>(-HalfPi<T> * r31 / std::abs(r31)),
				Radian<T>(static_cast<T>(std::atan2(r12, r31 * r13)))
			);
		}
		
		return std::make_tuple
		(
			Radian<T>(static_cast<T>(std::atan2(r32, r33))),
			Radian<T>(static_cast<T>(std::asin(-r31))),
			Radian<T>(static_cast<T>(std::atan2(r21, r11)))
		);
	}

	// Calculates the pitch, heading, and roll (XYZ-axes) angles of this quaternion
	void Euler(Radian<T>& pitch, Radian<T>& heading, Radian<T>& roll) const noexcept
	{
		const auto[p, h, r] = Euler();

		pitch = std::move(p);
		heading = std::move(h);
		roll = std::move(r);
	}

	// Calculates the pitch, heading, and roll (XYZ-axes) angles of this quaternion
	void Euler(Degree<T>& pitch, Degree<T>& heading, Degree<T>& roll) const noexcept
	{
		const auto[p, h, r] = Euler();

		pitch = p;
		heading = h;
		roll = r;
	}

public:
	// Calculates the normalized quaternion of 'quat'
	static Type NormalOf(Type quat) noexcept
	{
		return quat.Normalize();
	}

	// Calculates the normalized quaternion of 'quat'
	// Returns a copy of 'quat' if magnitude is 0
	static Type SafeNormalOf(Type quat) noexcept
	{
		return quat.NormalizeSafe();
	}

	// Calculates the concatenation of 'q' and 'r'
	static Type ConcatenationOf(Type q, Type r) noexcept
	{
		return q.Concatenate(std::move(r));
	}

	// Calculates the conjugate of 'quat'
	static Type ConjugateOf(Type quat) noexcept
	{
		return quat.Conjugate();
	}

	// Calculates the inverse of 'quat'
	static Type InverseOf(Type quat) noexcept
	{
		return quat.Invert();
	}

public:
	// Calculates the linear interpolation of normalized quaternions 'from' and 'to'
	static auto Lerp(Type from, Type to, T t) noexcept
	{
		if constexpr (std::is_floating_point_v<T>::value)
			return NormalOf((std::move(from) * (T(1) - t)) + (std::move(to) * t));
		else
			static_assert(false, "Cannot interpolate non-floating-point Quaternion types.");
	}

	// Calculates the spherical linear interpolation of normalized quaternions 'from' and 'to'.
	// Reduces spinning by checking if 'from' and 'to' are more than 90 deg apart.
	static auto SlerpSR(Type from, Type to, T t) noexcept
	{
		if constexpr (std::is_floating_point_v<T>::value)
		{
			Type qt = std::move(to);
			auto dot = from.Dot(qt);

			// When from and to > 90 deg apart, perform spin reduction
			if (dot < T(0))
			{
				qt.Conjugate(); // A quaternion's inverse is its conjugate if it's normalized
				dot = -dot;
			}

			// Use linear interpolation when sin(acos(dot)) close to 0
			if (dot > T(1) - Epsilon<T>)
				return Lerp(std::move(from), std::move(to), std::move(t));

			// Spherical interpolation
			Radian<T> theta = static_cast<T>(acos(dot));
			Radian<T> thetaFrom = theta.Value() * (T(1) - t);
			Radian<T> thetaTo = theta.Value() * t;

			return ((std::move(from) * thetaFrom.Sin()) + (std::move(qt) * thetaTo.Sin())) / theta.Sin();
		}
		else
			static_assert(false, "Cannot interpolate non-floating-point Quaternion types.");
	}

	// Calculates the spherical linear interpolation of normalized quaternions 'from' and 'to'
	static auto Slerp(Type from, Type to, T t) noexcept
	{
		if constexpr (std::is_floating_point_v<T>::value)
		{
			auto dot = from.Dot(to);

			// Use linear interpolation when sin(acos(dot)) close to 0
			if (dot > T(1) - Epsilon<T>)
				return Lerp(std::move(from), std::move(to), std::move(t));

			Radian<T> theta = acos(dot);
			Radian<T> thetaFrom = theta.Value() * (T(1) - t);
			Radian<T> thetaTo = theta.Value() * t;

			return ((std::move(from) * thetaFrom.Sin()) + (std::move(to) * thetaTo.Sin())) / theta.Sin();
		}
		else
			static_assert(false, "Cannot interpolate non-floating-point Quaternion types.");
	}

	// Calculates the spherical cubic interpolation of normalized quaternions 'from', 'to', 'a', and 'b'
	static auto Squad(Type from, Type to, Type a, Type b, T t) noexcept
	{
		return Slerp(Slerp(std::move(from), std::move(to), t), Slerp(std::move(a), std::move(b), t), T(2) * t * (T(1) - t));
	}

public:
	// Calculates a conjugate quaternion
	Type operator - () const noexcept
	{
		return Type::ConjugateOf(*this);
	}

	// Calculates an inverted quaternion
	Type operator ~ () const noexcept
	{
		return Type::InverseOf(*this);
	}

public:
	#pragma region Assignment Operators

	// Sets this quaternion to an identity quaternion
	Type& operator = (const IdentityTag&) noexcept
	{
		return MakeIdentity();
	}

	// Concatenates this quaternion with 'quat'
	Type& operator *= (Type quat) noexcept
	{
		return Concatenate(std::move(quat));
	}

	// Concatenates this quaternion with the inverse of 'quat'
	Type& operator /= (Type quat) noexcept
	{
		// Concatenate(Type::InverseOf(quat))

		const auto invMag = T(1) / quat.MagnitudeSq();

		const auto tqx = -quat.Values[0] * invMag;
		const auto tqy = -quat.Values[1] * invMag;
		const auto tqz = -quat.Values[2] * invMag;
		const auto tqw = quat.Values[3] * invMag;

		return *this =
		{
			(Values[1] * tqz) - (Values[2] * tqy) + (Values[3] * tqx) + (Values[0] * tqw),
			(Values[2] * tqx) - (Values[0] * tqz) + (Values[3] * tqy) + (Values[1] * tqw),
			(Values[0] * tqy) - (Values[1] * tqx) + (Values[3] * tqz) + (Values[2] * tqw),
			(Values[3] * tqw) - ((Values[0] * tqx) + (Values[1] * tqy) + (Values[2] * tqz))
		};
	}

	//////

	Type& operator *= (T value) noexcept
	{
		for (size_t i = 0; i < Size; ++i)
			Values[i] *= value;

		return *this;
	}

	Type& operator /= (T value) noexcept
	{
		for (size_t i = 0; i < Size; ++i)
			Values[i] /= value;

		return *this;
	}

	//////

	template<class U, typename = std::enable_if_t<std::is_convertible_v<U, T>>>
	Type& operator = (const U(&values)[Size]) noexcept
	{
		for (size_t i = 0; i < Size; ++i)
			Values[i] = static_cast<T>(values[i]);

		return *this;
	}

	template<class U, typename = std::enable_if_t<std::is_convertible_v<U, T>>>
	Type& operator += (const U(&values)[Size]) noexcept
	{
		for (size_t i = 0; i < Size; ++i)
			Values[i] += static_cast<T>(values[i]);

		return *this;
	}

	template<class U, typename = std::enable_if_t<std::is_convertible_v<U, T>>>
	Type& operator -= (const U(&values)[Size]) noexcept
	{
		for (size_t i = 0; i < Size; ++i)
			Values[i] -= static_cast<T>(values[i]);

		return *this;
	}

	Type& operator = (Type quat) noexcept
	{
		for (size_t i = 0; i < Size; ++i)
			Values[i] = quat.Values[i];

		return *this;
	}

	Type& operator += (Type quat) noexcept
	{
		for (size_t i = 0; i < Size; ++i)
			Values[i] += quat.Values[i];

		return *this;
	}

	Type& operator -= (Type quat) noexcept
	{
		for (size_t i = 0; i < Size; ++i)
			Values[i] -= quat.Values[i];

		return *this;
	}
	
	#pragma endregion

public:
	#pragma region Arithmetic Operators

	// Returns the concatenation of this quaternion and 'quat'
	Type operator * (Type quat) const noexcept
	{
		return Type(*this) *= std::move(quat);
	}

	// Returns the concatenation of this quaternion and the inverse of 'quat'
	Type operator / (Type quat) const noexcept
	{
		return Type(*this) /= std::move(quat);
	}

	//////

	Type operator * (T value) const noexcept
	{
		return Type(*this) *= std::move(value);
	}

	Type operator / (T value) const noexcept
	{
		return Type(*this) /= std::move(value);
	}

	//////

	template<class U, typename = std::enable_if_t<std::is_convertible_v<U, T>>>
	Type operator + (const U(&values)[Size]) const noexcept
	{
		return Type(*this) += values;
	}

	template<class U, typename = std::enable_if_t<std::is_convertible_v<U, T>>>
	Type operator - (const U(&values)[Size]) const noexcept
	{
		return Type(*this) -= values;
	}

	Type operator + (Type quat) const noexcept
	{
		return Type(*this) += std::move(quat);
	}

	Type operator - (Type quat) const noexcept
	{
		return Type(*this) -= std::move(quat);
	}

	#pragma endregion
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
		return !(quatA == quatB);
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
	inline Vector<T, S>& Epic::Vector<T, S>::operator *= (const Quaternion<T>& quat) noexcept
	{
		if constexpr (S == 3 || S == 4)
			quat.Transform(*this);
		else
			static_assert(false, "Operation is only available for Vectors of size 3 or 4.");

		return *this;
	}

	template<class T, size_t S>
	inline Vector<T, S> operator * (Vector<T, S> vec, const Quaternion<T>& quat) noexcept
	{
		if constexpr (S == 3 || S == 4)
		{
			auto result = std::move(vec);
			quat.Transform(result);
			return result;
		}
		else
			static_assert(false, "Operation is only available for Vectors of size 3 or 4.");
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
