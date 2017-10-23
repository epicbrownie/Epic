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

#include <Epic/Math/detail/MatrixFwd.hpp>
#include <Epic/Math/detail/MatrixBase.hpp>
#include <Epic/Math/detail/VectorHelpers.hpp>
#include <Epic/Math/detail/MathHelpers.hpp>
#include <Epic/Math/detail/QuaternionFwd.hpp>
#include <Epic/Math/Angle.hpp>
#include <Epic/Math/Vector.hpp>
#include <cassert>

//////////////////////////////////////////////////////////////////////////////

// Matrix
template<class T, size_t S>
class Epic::Matrix : public Epic::detail::MatrixBase<T, S>
{
public:
	using Base = Epic::detail::MatrixBase<T, S>;
	using Type = Epic::Matrix<T, S>;

public:
	template<class, size_t>
	friend class Epic::Matrix;

public:
	using value_type = typename Base::value_type;
	constexpr static size_t Size = Base::Size;

	using ColumnType = typename Base::ColumnType;
	constexpr static size_t ColumnCount = Base::ColumnCount;

private:
	using Base::Columns;
	using Base::Values;

public:
	#pragma region Constructors

	// Constructs a matrix with default initialized values
	Matrix() noexcept = default;

	// Copy-constructs a matrix
	Matrix(const Type&) noexcept = default;

	// Move-constructs a matrix
	Matrix(Type&&) noexcept = default;

	// Copy-converts a matrix
	template<class U, size_t Sz, typename = std::enable_if_t<std::is_convertible_v<U, T>>>
	explicit Matrix(const Matrix<U, Sz>& mat) noexcept
	{
		if constexpr (Sz < ColumnCount)
		{
			// 'mat' is smaller than this matrix
			MakeIdentity();

			TMP::ForEachN<Sz>::Apply([&] (size_t c)
			{
				for (size_t r = 0; r < Sz; ++r)
					Values[(c * ColumnType::Size) + r] = 
						static_cast<T>(mat.Values[(c * Matrix<U, Sz>::ColumnType::Size) + r]);
			});
		}
		else if constexpr (Sz > ColumnCount)
		{
			// 'mat' is larger than this matrix
			TMP::ForEachN<ColumnCount>::Apply([&] (size_t c)
			{
				for (size_t r = 0; r < ColumnType::Size; ++r)
					Values[(c * ColumnType::Size) + r] = 
						static_cast<T>(mat.Values[(c * Matrix<U, Sz>::ColumnType::Size) + r]);
			});
		}
		else
		{
			// 'mat' is the same size as this matrix
			TMP::ForEachN<Size>::Apply([&] (size_t n)
			{ 
				Values[n] = static_cast<T>(mat[n]);
			});
		}
	}

	// Constructs a matrix from a list of values.
	template<class U>
	explicit Matrix(const U(&values)[Size]) noexcept
	{
		TMP::ForEachN<Size>::Apply([&] (size_t n)
		{ 
			Values[n] = static_cast<T>(values[n]); 
		});
	}

	// Constructs a matrix whose values are all set to a value
	template<class U, typename = std::enable_if_t<std::is_convertible_v<U, T>>>
	Matrix(const U& value) noexcept
	{
		const T cv = static_cast<T>(value);
		TMP::ForEachN<Size>::Apply([&] (size_t n) { Values[n] = cv; });
	}

	// Constructs a matrix from a span of values
	template<class Arg, class... Args, 
		typename = std::enable_if_t<(detail::Span<Arg, Args...>::value == Size)>>
	Matrix(const Arg& arg, const Args&... args) noexcept
	{
		Construct(arg, args...);
	}

	// Constructs a rotation matrix from a quaternion
	template<class U, typename EnabledFor3x3OrGreater = std::enable_if_t<(S >= 3)>>
	explicit Matrix(const Quaternion<U>& q) noexcept
	{
		MakeRotation(q);
	}

	// Constructs a matrix whose values are all set to 0
	Matrix(const ZeroesTag&) noexcept
		: Type(T(0))
	{ }

	// Constructs a matrix whose values are all set to 1
	Matrix(const OnesTag&) noexcept
		: Type(T(1))
	{ }

	// Constructs an identity matrix
	Matrix(const IdentityTag&) noexcept
		: Type(T(0))
	{
		const auto cv1 = T(1);

		TMP::ForEachN<ColumnCount>::Apply([&] (size_t n)
		{ 
			Values[ColumnType::Size * n + n] = cv1; 
		});
	}

	// Constructs a 2D TRS matrix
	template<typename = std::enable_if_t<(S == 3)>>
	Matrix(const Vector2<T>& vT, const Radian<T>& psi, const Vector2<T>& vS) noexcept
	{
		MakeTRS(vT, psi, vS);
	}

	// Constructs a 3D TRS matrix
	template<typename = std::enable_if_t<(S == 4)>>
	Matrix(const Vector3<T>& vT, const Quaternion<T>& qR, const Vector3<T>& vS) noexcept
	{
		MakeTRS(vT, qR, vS);
	}

	// Constructs a translation matrix from a span of values
	template<class Arg, class... Args, 
		typename = std::enable_if_t<(detail::Span<Arg, Args...>::value <= ColumnType::Size)>>
	Matrix(const TranslationTag&, const Arg& arg, const Args&... args) noexcept
	{
		MakeTranslation(arg, args...);
	}

	// Constructs a scale matrix from a span of values
	template<class Arg, class... Args,
		typename = std::enable_if_t<(detail::Span<Arg, Args...>::value <= ColumnType::Size)>>
	Matrix(const ScaleTag&, const Arg& arg, const Args&... args) noexcept
	{
		MakeScale(arg, args...);
	}

	// Constructs an X-axis rotation matrix
	template<typename EnabledFor3x3OrGreater = std::enable_if_t<(S >= 3)>>
	Matrix(const XRotationTag&, const Radian<T>& phi) noexcept
	{
		MakeXRotation(phi);
	}

	// Constructs a Y-axis rotation matrix
	template<typename EnabledFor3x3OrGreater = std::enable_if_t<(S >= 3)>>
	Matrix(const YRotationTag&, const Radian<T>& theta) noexcept
	{
		MakeYRotation(theta);
	}

	// Constructs a Z-axis rotation matrix
	template<typename EnabledFor2x2OrGreater = std::enable_if_t<(S >= 2)>>
	Matrix(const ZRotationTag&, const Radian<T>& psi) noexcept
		: Type(Epic::Identity)
	{
		const T sinx = psi.Sin();
		const T cosx = psi.Cos();

		Values[0 * ColumnType::Size + 0] = cosx;
		Values[0 * ColumnType::Size + 1] = sinx;
		Values[1 * ColumnType::Size + 0] = -sinx;
		Values[1 * ColumnType::Size + 1] = cosx;
	}

	// Constructs a 2D rotation matrix
	template<typename EnabledFor2x2OrGreater = std::enable_if_t<(S >= 2)>>
	Matrix(const RotationTag&, const Radian<T>& psi) noexcept
	{
		MakeRotation(psi);
	}

	// Constructs a 3D rotation matrix
	template<typename EnabledFor3x3OrGreater = std::enable_if_t<(S >= 3)>>
	Matrix(const Radian<T>& pitch, const Radian<T>& heading, const Radian<T>& roll) noexcept
	{
		MakeRotation(pitch, heading, roll);
	}

	// Constructs a 3D rotation matrix
	template<typename EnabledFor3x3OrGreater = std::enable_if_t<(S >= 3)>>
	Matrix(const RotationTag&, const Vector3<T>& axis, const Radian<T>& angle) noexcept
	{
		MakeRotation(axis, angle);
	}

	// Constructs a 3D rotation matrix
	template<class U, typename EnabledFor3x3OrGreater = std::enable_if_t<(S >= 3)>>
	Matrix(const RotationTag&, const Quaternion<U>& q) noexcept
		: Matrix(q)
	{ }

	// Constructs a shear matrix from a shear amount and the target column/row coordinates
	template<typename EnabledFor2x2OrGreater = std::enable_if_t<(S >= 2)>>
	Matrix(const ShearTag&, const T shear, const size_t column, const size_t row) noexcept
		: Type(Epic::Identity)
	{
		assert(column >= 0 && column < ColumnCount);
		assert(row >= 0 && row < ColumnType::Size);

		Values[(ColumnType::Size * column) + row] = shear;
	}

	// Constructs a homogeneous "look at" matrix from a target position, an eye location, and an up direction
	template<typename EnabledFor4x4 = std::enable_if_t<(S == 4)>>
	Matrix(const LookAtTag&, const Point3<T>& target,
		   const Point3<T>& eye = { T(0), T(0), T(0) },
		   const Normal3<T>& up = { T(0), T(1), T(0) }) noexcept
	{ 
		LookAt(target, eye, up);
	}

	// Constructs a homogeneous frustum matrix from boundary values
	template<typename EnabledFor4x4 = std::enable_if_t<(S == 4)>>
	Matrix(const FrustumTag&, const T left, const T right, const T top, const T bottom, const T znear, const T zfar) noexcept
	{
		const auto h = top - bottom;
		const auto w = right - left;
		const auto d = zfar - znear;
		const auto n2 = T(2) * znear;
		const auto z = T(0);

		assert(h != T(0));
		assert(w != T(0));
		assert(d != T(0));

		Columns[0].Reset(n2 / w, z, z, z);
		Columns[1].Reset(z, n2 / h, z, z);
		Columns[2].Reset((right + left) / w, (top + bottom) / h, -(zfar + znear) / d, T(-1));
		Columns[3].Reset(z, z, (-n2*zfar) / d, z);
	}

	// Constructs a homogeneous perspective matrix from a field-of-view, aspect ratio, and near/far distances
	template<typename EnabledFor4x4 = std::enable_if_t<(S == 4)>>
	Matrix(const PerspectiveTag&, const Radian<T>& fovy, const T aspectRatio, const T znear, const T zfar) noexcept
	{
		const auto z = T(0);
		const auto f = T(1) / (fovy / T(2)).Tan();
		const auto d = znear - zfar;

		assert(d != T(0));
		assert(aspectRatio != T(0));

		Columns[0].Reset(f / aspectRatio, z, z, z);
		Columns[1].Reset(z, f, z, z);
		Columns[2].Reset(z, z, (zfar + znear) / d, T(-1));
		Columns[3].Reset(z, z, (T(2) * zfar * znear) / d, z);
	}

	// Constructs a homogeneous orthographic matrix from boundary values
	template<typename EnabledFor4x4 = std::enable_if_t<(S == 4)>>
	Matrix(const OrthoTag&, const T left, const T right, const T top, const T bottom, const T znear, const T zfar) noexcept
	{
		const auto h = top - bottom;
		const auto w = right - left;
		const auto d = zfar - znear;
		const auto z = T(0);

		assert(h != T(0));
		assert(w != T(0));
		assert(d != T(0));

		Columns[0].Reset(T(2) / w, z, z, z);
		Columns[1].Reset(z, T(2) / h, z, z);
		Columns[2].Reset(z, z, T(-2) / d, z);
		Columns[3].Reset(-(right + left) / w, -(top + bottom) / h, -(zfar + znear) / d, T(1));
	}

	// Constructs a homogeneous orthographic matrix from boundary values ([near, far] preset to [-1, 1])
	template<typename EnabledFor4x4 = std::enable_if_t<(S == 4)>>
	Matrix(const Ortho2DTag&, const T left, const T right, const T top, const T bottom) noexcept
		: Type(Epic::Ortho, left, right, top, bottom, T(-1), T(1))
	{ }

	// Constructs a homogeneous projective picking matrix from a window coordinate, picking region, and viewport boundaries
	template<typename EnabledFor4x4 = std::enable_if_t<(S == 4)>>
	Matrix(const PickingTag&, 
		   const T pickx, const T picky, const T pickw, const T pickh, 
		   const T vpX, const T vpY, const T vpW, const T vpH) noexcept
		: Type(Epic::Identity)
	{
		assert(pickw > T(0));
		assert(pickh > T(0));

		Values[0] = vpW / pickw;
		Values[5] = vpH / pickh;
		Values[12] = (vpW + T(2) * (vpX - pickx)) / pickw;
		Values[13] = (vpH + T(2) * (vpY - picky)) / pickh;
	}

	// Constructs a homogeneous projective shadow matrix from a ground plane and a light source
	template<typename EnabledFor4x4 = std::enable_if_t<(S == 4)>>
	Matrix(const ShadowTag&, const Vector4<T>& ground, const Point4<T>& light) noexcept
	{
		const auto dot = ground.Dot(light);

		Columns[0].Reset(dot - light[0] * ground[0], -light[0] * ground[1], -light[0] * ground[2], -light[0] * ground[3]);
		Columns[1].Reset(-light[1] * ground[0], dot - light[1] * ground[1], -light[1] * ground[2], -light[1] * ground[3]);
		Columns[2].Reset(-light[2] * ground[0], -light[2] * ground[1], dot - light[2] * ground[2], -light[2] * ground[3]);
		Columns[3].Reset(-light[3] * ground[0], -light[3] * ground[1], -light[3] * ground[2], dot - light[3] * ground[3]);
	}

	#pragma endregion

public:
	#pragma region Range Accessors

	// Retrieves the number of columns
	constexpr size_t size() const noexcept
	{
		return ColumnCount;
	}

	// Retrieves a pointer to the underlying column data
	constexpr const ColumnType* data() const noexcept
	{
		return Columns.data();
	}

	// Retrieves a pointer to the underlying column data
	ColumnType* data() noexcept
	{
		return Columns.data();
	}

	// Accesses the column at 'index'
	ColumnType& at(size_t index) noexcept
	{
		assert(index >= 0 && index < ColumnCount);
		return Columns[index];
	}

	// Accesses the column at 'index'
	const ColumnType& at(size_t index) const noexcept
	{
		assert(index >= 0 && index < ColumnCount);
		return Columns[index];
	}

	// Accesses the column at 'index'
	ColumnType& operator[] (size_t index) noexcept
	{
		assert(index >= 0 && index < ColumnCount);
		return Columns[index];
	}

	// Accesses the column at 'index'
	const ColumnType& operator[] (size_t index) const noexcept
	{
		assert(index >= 0 && index < ColumnCount);
		return Columns[index];
	}

	// Retrieves an iterator to the first column
	auto begin() noexcept -> decltype(Columns.begin())
	{
		return Columns.begin();
	}

	// Retrieves an iterator to the first column
	auto begin() const noexcept -> decltype(Columns.begin())
	{
		return Columns.begin();
	}

	// Retrieves an iterator to one past the last column
	auto end() const noexcept -> decltype(Columns.end())
	{
		return Columns.end();
	}

	#pragma endregion

public:
	// Multiplies this matrix and 'vec' together. (vec' = M * vec)
	void Transform(Vector<T, S>& vec) const noexcept
	{
		const auto src = vec;

		TMP::ForEachN<ColumnCount>::Apply([&] (size_t i)
		{
			vec[i] = src[0] * Values[i];

			for (size_t j = 1; j < ColumnCount; ++j)
				vec[i] += src[j] * Values[(ColumnType::Size * j) + i];
		});
	}

	// Multiplies this matrix and a homogenized (point) 'vec' together. (vec' = M * vec)
	template<typename EnabledFor2x2OrGreater = std::enable_if_t<(S >= 2)>>
	void Transform(Vector<T, S - 1>& vec) const noexcept
	{
		const auto src = vec;

		TMP::ForEachN<ColumnCount - 1>::Apply([&] (size_t i)
		{
			vec[i] = src[0] * Values[i];

			for (size_t j = 1; j < (ColumnCount - 1); ++j)
				vec[i] += src[j] * Values[(ColumnType::Size * j) + i];

			vec[i] += Values[(ColumnType::Size * (ColumnCount - 1)) + i];
		});
	}

	// Multiplies this matrix and 'vec' together. (Row Major) (vec' = vec * M)
	void TransformRM(Vector<T, S>& vec) const noexcept
	{
		auto src = vec;

		TMP::ForEachN<ColumnType::Size>::Apply([&] (size_t i)
		{
			vec[i] = src[0] * Values[ColumnCount * i];

			for (size_t j = 1; j < ColumnType::Size; ++j)
				vec[i] += src[j] * Values[(ColumnCount * i) + j];
		});
	}

	// Multiplies this matrix and a homogenized (point) 'vec' together. (Row Major) (vec' = vec * M)
	template<typename EnabledFor2x2OrGreater = std::enable_if_t<(S >= 2)>>
	void TransformRM(Vector<T, S - 1>& vec) const noexcept
	{
		const auto src = vec;

		TMP::ForEachN<ColumnType::Size>::Apply([&] (size_t i)
		{
			vec[i] = src[0] * Values[ColumnCount * i];

			for (size_t j = 1; j < (ColumnType::Size - 1); ++j)
				vec[i] += src[j] * Values[(ColumnCount * i) + j];

			vec[i] += Values[(ColumnCount * i) + ColumnType::Size - 1];
		});
	}

public:
	// Sets a span of values explicitly
	template<class Arg, class... Args, typename = std::enable_if_t<(detail::Span<Arg, Args...>::value == Size)>>
	Type& Reset(const Arg& arg, const Args&... args) noexcept
	{
		Construct(arg, args...);
		return *this;
	}

	// Fills this matrix with 'value'
	constexpr Type& Fill(const T& value) noexcept
	{
		TMP::ForEachN<Size>::Apply([&] (size_t n) { Values[n] = value; });
		return *this;
	}

	// Sets this matrix to the identity matrix
	constexpr Type& MakeIdentity() noexcept
	{
		TMP::ForEachN<Size>::Apply([&] (size_t n) { Values[n] = T(0); });
		TMP::ForEachN<ColumnCount>::Apply([&] (size_t n) { Values[ColumnCount * n + n] = T(1); });

		return *this;
	}

	// Sets this matrix to a matrix capable of placing things at position vT, oriented in rotation psi, and scaled by vS
	template<typename EnabledFor3x3 = std::enable_if_t<(S == 3)>>
	Type& MakeTRS(const Vector2<T>& vT, const Radian<T>& psi, const Vector2<T>& vS) noexcept
	{
		MakeRotation(psi);

		cx *= vS.x;
		cy *= vS.y;

		cz = vT;

		return *this;
	}

	// Sets this matrix to a matrix capable of placing things at position vT, oriented in rotation qR, and scaled by vS
	template<typename EnabledFor4x4 = std::enable_if_t<(S == 4)>>
	Type& MakeTRS(const Vector3<T>& vT, const Quaternion<T>& qR, const Vector3<T>& vS) noexcept
	{
		MakeRotation(qR);

		cx *= vS.x;
		cy *= vS.y;
		cz *= vS.z;

		cw = vT;

		return *this;
	}

	// Sets this matrix to a translation matrix
	template<class Arg, class... Args,
		typename = std::enable_if_t<(detail::Span<Arg, Args...>::value <= ColumnType::Size)>>
	Type& MakeTranslation(const Arg& arg, const Args&... args) noexcept
	{
		static constexpr size_t SpanV = detail::Span<Arg, Args...>::value;
		static constexpr size_t DestN = Size - ColumnType::Size;

		MakeIdentity();

		Vector<T, SpanV> values{ arg, args... };
		TMP::ForEachN<SpanV>::Apply([&](size_t i) { Values[DestN + i] = values[i]; });

		return *this;
	}

	// Sets this matrix to a scale matrix
	template<class Arg, class... Args,
		typename = std::enable_if_t<(detail::Span<Arg, Args...>::value <= ColumnType::Size)>>
	Type& MakeScale(const Arg& arg, const Args&... args) noexcept
	{
		static constexpr size_t SpanV = detail::Span<Arg, Args...>::value;

		MakeIdentity();

		Vector<T, SpanV> values{ arg, args... };
		TMP::ForEachN<SpanV>::Apply([&](size_t i) { Values[ColumnCount * i + i] = values[i]; });

		return *this;
	}

	// Sets this matrix to an X-axis rotation matrix
	template<typename EnabledFor3x3OrGreater = std::enable_if_t<(S >= 3)>>
	Type& MakeXRotation(const Radian<T>& phi) noexcept
	{
		MakeIdentity();

		const T sinx = phi.Sin();
		const T cosx = phi.Cos();

		Values[1 * ColumnType::Size + 1] = cosx;
		Values[1 * ColumnType::Size + 2] = sinx;
		Values[2 * ColumnType::Size + 1] = -sinx;
		Values[2 * ColumnType::Size + 2] = cosx;

		return *this;
	}

	// Sets this matrix to a Y-axis rotation matrix
	template<typename EnabledFor3x3OrGreater = std::enable_if_t<(S >= 3)>>
	Type& MakeYRotation(const Radian<T>& theta) noexcept
	{
		MakeIdentity();

		const T sinx = theta.Sin();
		const T cosx = theta.Cos();

		Values[0 * ColumnType::Size + 0] = cosx;
		Values[0 * ColumnType::Size + 2] = -sinx;
		Values[2 * ColumnType::Size + 0] = sinx;
		Values[2 * ColumnType::Size + 2] = cosx;

		return *this;
	}

	// Sets this matrix to a Z-axis rotation matrix
	template<typename EnabledFor2x2OrGreater = std::enable_if_t<(S >= 2)>>
	Type& MakeZRotation(const Radian<T>& psi) noexcept
	{
		MakeIdentity();

		const T sinx = psi.Sin();
		const T cosx = psi.Cos();

		Values[0 * ColumnType::Size + 0] = cosx;
		Values[0 * ColumnType::Size + 1] = sinx;
		Values[1 * ColumnType::Size + 0] = -sinx;
		Values[1 * ColumnType::Size + 1] = cosx;

		return *this;
	}

	// Sets this matrix to a 2D rotation matrix
	template<typename EnabledFor2x2OrGreater = std::enable_if_t<(S >= 2)>>
	Type& MakeRotation(const Radian<T>& psi) noexcept
	{
		MakeZRotation(psi);

		return *this;
	}

	// Sets this matrix to a 3D rotation matrix
	template<typename EnabledFor3x3OrGreater = std::enable_if_t<(S >= 3)>>
	Type& MakeRotation(const Radian<T>& pitch, const Radian<T>& heading, const Radian<T>& roll) noexcept
	{
		return MakeRotation(Epic::Quaternion<T>{ pitch, heading, roll });
	}

	// Sets this matrix to a 3D rotation matrix
	template<typename EnabledFor3x3OrGreater = std::enable_if_t<(S >= 3)>>
	Type& MakeRotation(const Vector3<T>& axis, const Radian<T>& angle) noexcept
	{
		MakeIdentity();

		const T sinx = angle.Sin();
		const T cosx = angle.Cos();
		const T cos1x = T(1) - cosx;

		const T cxx = cos1x * axis[0] * axis[0];
		const T cyy = cos1x * axis[1] * axis[1];
		const T czz = cos1x * axis[2] * axis[2];
		const T cxy = cos1x * axis[0] * axis[1];
		const T cxz = cos1x * axis[0] * axis[2];
		const T cyz = cos1x * axis[1] * axis[2];

		const T sx = sinx * axis[0];
		const T sy = sinx * axis[1];
		const T sz = sinx * axis[2];

		Values[0 * ColumnType::Size + 0] = cxx + cosx;
		Values[0 * ColumnType::Size + 1] = cxy + sz;
		Values[0 * ColumnType::Size + 2] = cxz - sy;

		Values[1 * ColumnType::Size + 0] = cxy - sz;
		Values[1 * ColumnType::Size + 1] = cyy + cosx;
		Values[1 * ColumnType::Size + 2] = cyz + sx;

		Values[2 * ColumnType::Size + 0] = cxz + sy;
		Values[2 * ColumnType::Size + 1] = cyz - sx;
		Values[2 * ColumnType::Size + 2] = czz + cosx;

		return *this;
	}

	// Sets this matrix to a 3D rotation matrix
	template<class U, typename EnabledFor3x3OrGreater = std::enable_if_t<(S >= 3)>>
	Type& MakeRotation(const Quaternion<U>& q) noexcept
	{
		MakeIdentity();

		const T qx = static_cast<T>(q.x);
		const T qy = static_cast<T>(q.y);
		const T qz = static_cast<T>(q.z);
		const T qw = static_cast<T>(q.w);

		const T qxx = qx * qx;
		const T qyy = qy * qy;
		const T qzz = qz * qz;
		const T qxz = qx * qz;
		const T qxy = qx * qy;
		const T qyz = qy * qz;
		const T qwx = qw * qx;
		const T qwy = qw * qy;
		const T qwz = qw * qz;

		Values[0 * ColumnType::Size + 0] = T(1) - T(2) * (qyy + qzz);
		Values[0 * ColumnType::Size + 1] =		  T(2) * (qxy + qwz);
		Values[0 * ColumnType::Size + 2] =		  T(2) * (qxz - qwy);

		Values[1 * ColumnType::Size + 0] =		  T(2) * (qxy - qwz);
		Values[1 * ColumnType::Size + 1] = T(1) - T(2) * (qxx + qzz);
		Values[1 * ColumnType::Size + 2] =		  T(2) * (qyz + qwx);

		Values[2 * ColumnType::Size + 0] =		  T(2) * (qxz + qwy);
		Values[2 * ColumnType::Size + 1] =		  T(2) * (qyz - qwx);
		Values[2 * ColumnType::Size + 2] = T(1) - T(2) * (qxx + qyy);

		return *this;
	}

	// Sets this matrix to a homogeneous lookat matrix using a target position, an eye location, and an up direction
	template<typename EnabledFor4x4 = std::enable_if_t<(S == 4)>>
	Type& LookAt(const Point3<T>& target,
				 const Point3<T>& eye = { T(0), T(0), T(0) },
				 const Normal3<T>& up = { T(0), T(1), T(0) }) noexcept
	{
		const auto zaxis = Vector3<T>::SafeNormalOf(target - eye);
		const auto xaxis = Vector3<T>::SafeNormalOf(zaxis.Cross(up));
		const auto yaxis = xaxis.Cross(zaxis);
		const auto z = T(0);

		Columns[0].Reset(xaxis.x, yaxis.x, -zaxis.x, z);
		Columns[1].Reset(xaxis.y, yaxis.y, -zaxis.y, z);
		Columns[2].Reset(xaxis.z, yaxis.z, -zaxis.z, z);
		Columns[3].Reset(-xaxis.Dot(eye), -yaxis.Dot(eye), zaxis.Dot(eye), T(1));

		return *this;
	}

	// Create a quaternion from this rotation matrix
	template<typename EnabledFor3x3OrGreater = std::enable_if_t<(S >= 3)>>
	Quaternion<T> ToQuaternion() const noexcept
	{
		const auto trace = Trace();
		
		if(trace > T(0.000001))
		{
			const auto sqt = std::sqrt(trace) * T(2);
			
			return Quaternion<T>
			{
				(Columns[1][2] - Columns[2][1]) / sqt,
				(Columns[2][0] - Columns[0][2]) / sqt,
				(Columns[0][1] - Columns[1][0]) / sqt,
				sqt / T(4)
			};
		}
		else if(Columns[0][0] > Columns[1][1] && Columns[0][0] > Columns[2][2])
		{
			const auto sqt = std::sqrt(T(1) + Columns[0][0] - Columns[1][1] - Columns[2][2]) * T(2);
			
			return Quaternion<T>
			{
				sqt / T(4),
				(Columns[0][1] + Columns[1][0]) / sqt,
				(Columns[2][0] + Columns[0][2]) / sqt,
				(Columns[1][2] + Columns[2][1]) / sqt
			};
		}
		else if(Columns[1][1] > Columns[2][2])
		{
			const auto sqt = std::sqrt(T(1) + Columns[1][1] - Columns[0][0] - Columns[2][2]) * T(2);
			
			return Quaternion<T>
			{
				(Columns[0][1] + Columns[1][0]) / sqt,
				sqt / T(4),
				(Columns[1][2] + Columns[2][1]) / sqt,
				(Columns[2][0] + Columns[0][2]) / sqt
			};
		}
		else
		{
			const auto sqt = std::sqrt(T(1) + Columns[2][2] - Columns[0][0] - Columns[1][1]) * T(2);
			
			return Quaternion<T>
			{
				(Columns[2][0] + Columns[0][2]) / sqt,
				(Columns[1][2] + Columns[2][1]) / sqt,
				sqt / T(4),
				(Columns[0][1] + Columns[1][0]) / sqt
			};
		}
	}

public:
	// Calculates the sum of the main diagonal values
	constexpr T Trace() const noexcept
	{
		T result = T(0);

		TMP::ForEachN<ColumnCount>::Apply([&](size_t i)
		{
			result += Values[(i * ColumnType::Size) + i];
		});

		return result;
	}

	// Calculates the determinant of this matrix.
	T Determinant() const noexcept
	{
		return DeterminantHelper<ColumnCount>();
	}

	// Multiplies this matrix and 'mat' together. (M' = M * m)
	Type& Compose(const Type& mat) noexcept
	{
		Type result = Epic::Zero;

		TMP::ForEachN<ColumnCount>::Apply([&](size_t i)
		{
			for (size_t j = 0; j < ColumnType::Size; ++j)
				result[i] += Columns[j] * mat.Columns[i][j];
		});

		return (*this = result);
	}

	// Rearranges this matrix so that its columns become its rows
	Type& Transpose() noexcept
	{
		TMP::ForEachN<ColumnCount>([&](size_t i) 
		{
			for (size_t j = i + 1; j < ColumnCount; ++j)
				std::swap
				(
					Values[(i * ColumnType::Size) + j], 
					Values[(j * ColumnType::Size) + i]
				);
		});

		return *this;
	}

	// Inverts this matrix under the assumption that it describes a rigid-body transformation.
	Type& InvertRigid() noexcept
	{
		return TransposeInvertRigid().Transpose();
	}

	// Inverts this matrix. (s.t. M * inverse(M) = identity(M))
	Type& Invert() noexcept
	{
		const T det = Determinant();
		if (det == T(0))
			return *this;

		if constexpr (ColumnCount == 1)
		{
			if (Values[0] != T(0))
				Values[0] = T(1) / det;
		}

		else if constexpr (ColumnCount == 2)
		{
			const auto t = Values[0];

			Values[0] = Values[3];
			Values[1] = -Values[1];
			Values[2] = -Values[2];
			Values[3] = t;

			*this *= T(1) / det;
		}

		else if constexpr (ColumnCount == 3)
		{
			Type adj;
			adj.Values[0] =  (Values[4] * Values[8]) - (Values[5] * Values[7]);
			adj.Values[1] = -(Values[1] * Values[8]) + (Values[2] * Values[7]);
			adj.Values[2] =  (Values[1] * Values[5]) - (Values[2] * Values[4]);
			adj.Values[3] = -(Values[3] * Values[8]) + (Values[5] * Values[6]);
			adj.Values[4] =  (Values[0] * Values[8]) - (Values[2] * Values[6]);
			adj.Values[5] = -(Values[0] * Values[5]) + (Values[2] * Values[3]);
			adj.Values[6] =  (Values[3] * Values[7]) - (Values[4] * Values[6]);
			adj.Values[7] = -(Values[0] * Values[7]) + (Values[1] * Values[6]);
			adj.Values[8] =  (Values[0] * Values[4]) - (Values[1] * Values[3]);

			*this = adj;
			*this *= T(1) / det;
		}

		else
		{
			Type lower = Epic::Identity;
			Type& upper = *this;

			TMP::ForEachN<ColumnCount>([&](size_t i) 
			{
				// Partial pivoting (Column Switching ERO)
				T v = std::abs(upper.Values[(i * ColumnType::Size) + i]);
				size_t column = i;

				for (size_t j = i + 1; j < ColumnCount; ++j)
				{
					const T iv = std::abs(upper.Values[(j * ColumnType::Size) + i]);
					if (iv > v)
					{
						column = j;
						v = iv;
					}
				}

				if (column != i)
				{
					auto cv = upper[column];
					upper[column] = upper[i];
					upper[i] = cv;

					cv = lower[column];
					lower[column] = lower[i];
					lower[i] = cv;
				}

				// Reduce the diagonal (Column multiplication ERO)
				const T tc = T(1) / upper.Values[(i * ColumnType::Size) + i];
				lower[i] *= tc;
				upper[i] *= tc;

				// Zero the column i at column > i (Column addition ERO)
				for (size_t j = i + 1; j < ColumnCount; ++j)
				{
					const T t = upper.Values[(j * ColumnType::Size) + i];
					upper[j] -= upper[i] * t;
					lower[j] -= lower[i] * t;

					upper.Values[(j * ColumnType::Size) + i] = T(0);
				}
			});

			for (size_t i = ColumnCount - 1; i > 0; --i)
			{
				for (long long j = i - 1; j >= 0; --j)
				{
					size_t js = static_cast<size_t>(j);

					const T t = upper.Values[(js * ColumnType::Size) + i];
					lower[js] -= lower[i] * t;
					upper[js] -= upper[i] * t;
				}
			}

			*this = lower;
		}

		return *this;
	}

	// Inverts this matrix under the assumption that it describes a rigid-body transformation; then transposes it.
	Type& TransposeInvertRigid() noexcept
	{
		TMP::ForEachN<ColumnCount - 1>([&](size_t i)
		{
			Columns[i][ColumnType::Size - 1] = -Columns[i].Dot(Columns[ColumnCount - 1]);
		});

		Columns[ColumnCount - 1] = T(0);
		Columns[ColumnCount - 1][ColumnType::Size - 1] = T(1);

		// NOTE: *this is now equal to the transposed inverse

		return *this;
	}

	// Inverts this matrix; then transposes it.
	Type& TransposeInvert() noexcept
	{
		Invert();
		return Transpose();
	}

	// Constructs a matrix from the square region between [I, I + N]
	template<size_t I = 0, size_t N = ColumnCount>
	Matrix<T, N> Slice() const noexcept
	{
		if constexpr ((I + N) > ColumnCount)
			static_assert(false, "Slice parameters are out of bounds.");

		Matrix<T, N> result;
		size_t src = (ColumnCount * I) + I;
		size_t dest = 0;

		TMP::ForEachN<N>::Apply([&](size_t)
		{
			for (size_t j = 0; j < N; ++j)
				result.Values[dest++] = Values[src++];

			src += ColumnCount - N;
		});

		return result;
	}

	// Constructs a matrix from this matrix, less 'Amount' rows and columns
	// NOTE: Equivalent to Slice<0, S - Amount>()
	template<size_t Amount = 1>
	auto Contract() const noexcept
	{
		if constexpr (Amount >= ColumnCount)
			static_assert(false, "Contracted result Matrix must exceed size 0.");

		return Slice<0, ColumnCount - Amount>();
	}

	// Constructs a matrix from this matrix with 'Amount' additional rows and columns.
	// Added rows/columns are filled with values from the identity matrix.
	template<size_t Amount = 1>
	auto Expand() const noexcept
	{
		return Expand<Amount>(Epic::Identity);
	}

	// Constructs a matrix from this matrix with 'Amount' additional rows and columns.
	// Added rows/columns are filled with values from the zeroes matrix.
	template<size_t Amount = 1>
	auto Expand(const ZeroesTag&) const noexcept
	{
		constexpr static size_t ExpandedSz = ColumnCount + Amount;

		Matrix<T, ExpandedSz> result(*this);

		TMP::ForEachN<ColumnCount>::Apply([&](size_t c)
		{
			for (size_t r = ColumnType::Size; r < ExpandedSz; ++r)
				result.Values[(c * ExpandedSz) + r] = T(0);
		});
		
		TMP::ForEachN<ExpandedSz * Amount>::Apply([&](size_t r)
		{
			result.Values[(ExpandedSz * ColumnCount) + r] = T(0);
		});

		return result;
	}

	// Constructs a matrix from this matrix with 'Amount' additional rows and columns.
	// Added rows/columns are filled with values from the ones matrix.
	template<size_t Amount = 1>
	auto Expand(const OnesTag&) const noexcept
	{
		constexpr static size_t ExpandedSz = ColumnCount + Amount;

		Matrix<T, ExpandedSz> result(*this);

		TMP::ForEachN<ColumnCount>::Apply([&](size_t c)
		{
			for (size_t r = ColumnType::Size; r < ExpandedSz; ++r)
				result.Values[(c * ExpandedSz) + r] = T(1);
		});

		TMP::ForEachN<ExpandedSz * Amount>::Apply([&](size_t r)
		{
			result.Values[(ExpandedSz * ColumnCount) + r] = T(1);
		});

		return result;
	}

	// Constructs a matrix from this matrix with 'Amount' additional rows and columns.
	// Added rows/columns are filled with values from the identity matrix.
	template<size_t Amount = 1>
	auto Expand(const IdentityTag&) const noexcept
	{
		constexpr static size_t ExpandedSz = ColumnCount + Amount;

		Matrix<T, ExpandedSz> result(*this);

		TMP::ForEachN<ColumnCount>::Apply([&](size_t c)
		{
			for (size_t r = ColumnType::Size; r < ExpandedSz; ++r)
				result.Values[(c * ExpandedSz) + r] = T(0);
		});

		for (size_t c = ColumnCount; c < ExpandedSz; ++c)
		{
			TMP::ForEachN<ExpandedSz>::Apply([&](size_t r)
			{
				result.Values[(ExpandedSz * c) + r] = (c == r) ? T(1) : T(0);
			});
		}

		return result;
	}

public:
	// Returns 'matA' * 'matB'
	static Type CompositeOf(const Type& matA, const Type& matB) noexcept
	{
		Type result{ matA };
		return result.Compose(matB);
	}

	// Copies 'mat' and transposes it
	static Type TransposeOf(const Type& mat) noexcept
	{
		Type result{ mat };
		return result.Transpose();
	}

	// Copies 'mat' and inverts it under the assumption that it describes a rigid-body transformation.
	static Type RigidInverseOf(const Type& mat) noexcept
	{
		Type result{ mat };
		return result.InvertRigid();
	}

	// Copies 'mat' and inverts it
	static Type InverseOf(const Type& mat) noexcept
	{
		Type result{ mat };
		return result.Invert();
	}

	// Copies 'mat'. The copy is then inverted under the assumption that it describes a rigid-body transformation and then transposed.
	static Type TransposedRigidInverseOf(const Type& mat) noexcept
	{
		Type result{ mat };
		return result.TransposeInvertRigid();
	}

	// Copies 'mat'. The copy is then inverted and then transposed.
	static Type TransposedInverseOf(const Type& mat) noexcept
	{
		Type result{ mat };
		return result.TransposeInvert();
	}

public:
	// Copy this matrix with negated values
	Type operator - () const noexcept
	{
		Type result;
		TMP::ForEachN<Size>::Apply([&](size_t n) { result[n] = -Values[n]; });
		return result;
	}

	// Copy this matrix inverted under the assumption that it describes a rigid-body transformation
	Type operator ~ () const noexcept
	{
		return Type::RigidInverseOf(*this);
	}

	// Implicitly converts to T (only available if this matrix has just 1 element)
	template<class U = T, typename = std::enable_if_t<(Size == 1) && std::is_same_v<T, U>>>
	operator T() const noexcept
	{
		return Values[0];
	}

public:
	#pragma region Assignment Operators

	// Set all values to zero
	Type& operator = (const ZeroesTag&) noexcept
	{
		return Fill(T(0));
	}

	// Set all values to one
	Type& operator = (const OnesTag&) noexcept
	{
		return Fill(T(1));
	}

	// Set this matrix to an identity matrix (s.t. M * identity(M) = M)
	Type& operator = (const IdentityTag&) noexcept
	{
		return MakeIdentity();
	}

	// Compose this matrix with m
	Type& operator *= (const Type& m) noexcept
	{
		return Compose(m);
	}

	//////

	#define CREATE_SCALAR_ASSIGNMENT_OPERATOR(Op)			\
															\
	Type& operator Op (const T& value) noexcept				\
	{														\
		TMP::ForEachN<Size>::Apply([&](size_t index)		\
		{ Values[index] Op value; });						\
		return *this;										\
	}														\
															\
	template<class U>										\
	Type& operator Op (const U(&values)[Size]) noexcept		\
	{														\
		TMP::ForEachN<Size>::Apply([&](size_t index)		\
		{ Values[index] Op values[index]; });				\
		return *this;										\
	}

	CREATE_SCALAR_ASSIGNMENT_OPERATOR(= );
	CREATE_SCALAR_ASSIGNMENT_OPERATOR(+= );
	CREATE_SCALAR_ASSIGNMENT_OPERATOR(-= );
	CREATE_SCALAR_ASSIGNMENT_OPERATOR(*= );
	CREATE_SCALAR_ASSIGNMENT_OPERATOR(/= );

	#undef CREATE_SCALAR_ASSIGNMENT_OPERATOR

	//////

	#define CREATE_MATRIX_ASSIGNMENT_OPERATOR(Op)					\
																	\
	Type& operator Op (const Type& mat) noexcept					\
	{																\
		TMP::ForEachN<Size>::Apply([&](size_t index) {				\
			Values[index] Op mat.Values[index];						\
		});															\
		return *this;												\
	}																\
																	\
	template<class U>												\
	Type& operator Op (const Matrix<U, S>& mat) noexcept			\
	{																\
		TMP::ForEachN<Size>::Apply([&](size_t index) {				\
			Values[index] Op T(mat.Values[index]);					\
		});															\
		return *this;												\
	}

	CREATE_MATRIX_ASSIGNMENT_OPERATOR(= );
	CREATE_MATRIX_ASSIGNMENT_OPERATOR(+= );
	CREATE_MATRIX_ASSIGNMENT_OPERATOR(-= );

	#undef CREATE_MATRIX_ASSIGNMENT_OPERATOR

	//////

	#define CREATE_LOGIC_ASSIGNMENT_OPERATOR(Op)						\
																		\
	Type& operator Op (const T& value) noexcept							\
	{																	\
		if constexpr (std::is_integral_v<T>)							\
		{																\
			TMP::ForEachN<Size>::Apply([&](size_t index) {				\
				Values[index] Op value;									\
			});															\
		}																\
		else static_assert(false,										\
			"Logical operators are only valid for integral types.");	\
		return *this;													\
	}																	\
																		\
	template<class U>													\
	Type& operator Op (const U(&values)[Size]) noexcept					\
	{																	\
		if constexpr (std::is_integral_v<T> && std::is_integral_v<U>)	\
		{																\
			TMP::ForEachN<Size>::Apply([&](size_t index) {				\
				Values[index] Op values[index];							\
			});															\
		}																\
		else static_assert(false,										\
			"Logical operators are only valid for integral types.");	\
		return *this;													\
	}

	// The following assignment operators will fail for non-integral types
	CREATE_LOGIC_ASSIGNMENT_OPERATOR(|= );
	CREATE_LOGIC_ASSIGNMENT_OPERATOR(&= );
	CREATE_LOGIC_ASSIGNMENT_OPERATOR(^= );
	CREATE_LOGIC_ASSIGNMENT_OPERATOR(%= );
	CREATE_LOGIC_ASSIGNMENT_OPERATOR(<<= );
	CREATE_LOGIC_ASSIGNMENT_OPERATOR(>>= );

	#undef CREATE_LOGIC_ASSIGNMENT_OPERATOR

	//////

	#define CREATE_MATRIX_LOGIC_ASSIGNMENT_OPERATOR(Op)					\
																		\
	Type& operator Op (const Type& mat) noexcept						\
	{																	\
		if constexpr (std::is_integral_v<T>)							\
		{																\
			TMP::ForEachN<Size>::Apply([&](size_t index) {				\
				Values[index] Op mat.Values[index];						\
			});															\
		}																\
		else static_assert(false,										\
			"Logical operators are only valid for integral types.");	\
		return *this;													\
	}																	\
																		\
	template<class U>													\
	Type& operator Op (const Matrix<U, S>& mat) noexcept				\
	{																	\
		if constexpr (std::is_integral_v<T> && std::is_integral_v<U>)	\
		{																\
			TMP::ForEachN<Size>::Apply([&](size_t index) {				\
				Values[index] Op T(mat.Values[index]);					\
			});															\
		}																\
		else static_assert(false,										\
			"Logical operators are only valid for integral types.");	\
		return *this;													\
	}
	
	// The following assignment operators will fail for non-integral types
	CREATE_MATRIX_LOGIC_ASSIGNMENT_OPERATOR(|= );
	CREATE_MATRIX_LOGIC_ASSIGNMENT_OPERATOR(&= );
	CREATE_MATRIX_LOGIC_ASSIGNMENT_OPERATOR(^= );
	CREATE_MATRIX_LOGIC_ASSIGNMENT_OPERATOR(%= );
	CREATE_MATRIX_LOGIC_ASSIGNMENT_OPERATOR(<<= );
	CREATE_MATRIX_LOGIC_ASSIGNMENT_OPERATOR(>>= );

	#undef CREATE_MATRIX_LOGIC_ASSIGNMENT_OPERATOR

	#pragma endregion

public:
	#pragma region Arithmetic Operators

	// Return the composite of this matrix and 'mat'
	Type operator * (const Type& mat) const noexcept
	{
		return Type::CompositeOf(*this, mat);
	}

	//////

	#define CREATE_SCALAR_ARITHMETIC_OPERATOR(Op) 						\
																		\
	Type operator Op (const T value) const noexcept						\
	{																	\
		return Type(*this) Op= value;									\
	}																	\
																		\
	friend Type operator Op (const T value, const Type& mat) noexcept	\
	{																	\
		return Type(mat) Op= value;										\
	}																	\
																		\
	template<class U>													\
	Type operator Op (const U(&values)[Size]) const	noexcept			\
	{																	\
		return Type(*this) Op= values;									\
	}

	CREATE_SCALAR_ARITHMETIC_OPERATOR(+);
	CREATE_SCALAR_ARITHMETIC_OPERATOR(-);
	CREATE_SCALAR_ARITHMETIC_OPERATOR(*);
	CREATE_SCALAR_ARITHMETIC_OPERATOR(/);

	// The following arithmetic operators are only defined for integral types
	CREATE_SCALAR_ARITHMETIC_OPERATOR(|);
	CREATE_SCALAR_ARITHMETIC_OPERATOR(&);
	CREATE_SCALAR_ARITHMETIC_OPERATOR(^);
	CREATE_SCALAR_ARITHMETIC_OPERATOR(%);
	CREATE_SCALAR_ARITHMETIC_OPERATOR(<<);
	CREATE_SCALAR_ARITHMETIC_OPERATOR(>>);

	#undef CREATE_SCALAR_ARITHMETIC_OPERATOR

	//////

	#define CREATE_MATRIX_ARITHMETIC_OPERATOR(Op)					\
																	\
	Type operator Op (const Type& mat) const noexcept				\
	{																\
		return Type(*this) Op= mat;									\
	}																\
																	\
	template<class U>												\
	Type operator Op (const Matrix<U, S>& mat) const noexcept		\
	{																\
		return Type(*this) Op= mat;									\
	}																\

	CREATE_MATRIX_ARITHMETIC_OPERATOR(+);
	CREATE_MATRIX_ARITHMETIC_OPERATOR(-);

	// The following arithmetic operators are only defined for integral types
	CREATE_MATRIX_ARITHMETIC_OPERATOR(| );
	CREATE_MATRIX_ARITHMETIC_OPERATOR(&);
	CREATE_MATRIX_ARITHMETIC_OPERATOR(^);
	CREATE_MATRIX_ARITHMETIC_OPERATOR(%);
	CREATE_MATRIX_ARITHMETIC_OPERATOR(<< );
	CREATE_MATRIX_ARITHMETIC_OPERATOR(>> );

	#undef CREATE_MATRIX_ARITHMETIC_OPERATOR

	#pragma endregion

private:
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

	template<class U, size_t Sz>
	void PlaceAt(size_t offset, const Vector<U, Sz>& value) noexcept
	{
		TMP::ForEachN<Sz>::Apply([&](size_t n) { Values[offset++] = value[n]; });
	}

	template<class VectorT, size_t TS, size_t... Is>
	void PlaceAt(size_t offset, const Swizzler<VectorT, TS, Is...>& value) noexcept
	{
		PlaceAt(offset, value.ToVector());
	}

	template<class U, size_t N>
	void PlaceAt(size_t offset, const U(&value)[N]) noexcept
	{
		TMP::ForEachN<N>::Apply([&](size_t n) { Values[offset++] = value[n]; });
	}

	template<class U, size_t Sz>
	void PlaceAt(size_t offset, const std::array<U, Sz>& value) noexcept
	{
		TMP::ForEachN<Sz>::Apply([&](size_t n) { Values[offset++] = value[n]; });
	}

	template<class Val>
	void PlaceAt(size_t offset, const Val& value) noexcept
	{
		Values[offset] = value;
	}

	#pragma endregion

	#pragma region Determinant Helpers

	template<size_t n>
	T DeterminantHelper() const noexcept
	{
		if constexpr (n == 0)
			return 0;

		else if constexpr (n == 1)
			return Values[0];

		else if constexpr (n == 2)
			return (Values[0] * Values[3]) - (Values[1] * Values[2]);

		else if constexpr (n == 3)
		{
			// Determinant is the additive sum of the 3 top-to-bottom crosses 
			//	minus the additive sum of the 3 bottom-to-top crosses
			return (Values[0] * Values[4] * Values[8])
				 + (Values[1] * Values[5] * Values[6])
				 + (Values[2] * Values[3] * Values[7])
				 - (Values[6] * Values[4] * Values[2])
				 - (Values[7] * Values[5] * Values[0])
				 - (Values[8] * Values[3] * Values[1]);
		}

		else
		{
			// Get the 1st column of the matrix of minors
			auto minors = MinorsHelper<n>();

			// Transform it into a cofactor vector
			for (size_t i = 1; i < n; i += 2)
				minors[i] = -minors[i];

			// Calculate the determinant
			return minors.Dot(Columns[0]);
		}
	}

	template<size_t n>
	auto MinorsHelper() const noexcept
	{
		Vector<T, n> minors;
		Matrix<T, n - 1> minor;

		TMP::ForEachN<n>::Apply([&](size_t c) 
		{
			// Construct the minor matrix of [0, c]
			size_t d = 0;

			for (size_t i = 1; i < n; ++i)
			{
				for (size_t r = 0; r < n; ++r)
				{
					if (r != c)
						minor.Values[d++] = Values[(i * n) + r];
				}
			}

			// Set this minors value to the minor's determinant
			minors[c] = minor.DeterminantHelper<n - 1>();
		});

		return minors;
	}

	#pragma endregion
};

//////////////////////////////////////////////////////////////////////////////

// Friend Operators
namespace Epic
{
	template<class U, size_t Sz>
	inline bool operator == (const Matrix<U, Sz>& matA, const Matrix<U, Sz>& matB) noexcept
	{
		for (size_t i = 0; i < Sz; ++i)
			if (matA[i] != matB[i]) return false;

		return true;
	}

	template<class U, size_t Sz>
	inline bool operator != (const Matrix<U, Sz>& matA, const Matrix<U, Sz>& matB) noexcept
	{
		return !(matA == matB);
	}

	template<class U, size_t Sz>
	inline std::ostream& operator << (std::ostream& stream, const Matrix<U, Sz>& mat)
	{
		stream << "[\n";
		if (Sz > 0)
		{
			stream << std::fixed;
			TMP::ForEachN<Sz>::Apply([&] (size_t n)
			{
				stream << ' ' << mat[n];
				if (n < Sz - 1) stream << ',';
				stream << '\n';
			});
			stream << std::defaultfloat;
		}
		stream << ']';

		return stream;
	}

	template<class U, size_t Sz>
	inline std::wostream& operator << (std::wostream& stream, const Matrix<U, Sz>& mat)
	{
		stream << L"[\n";
		if (Sz > 0)
		{
			stream << std::fixed;
			TMP::ForEachN<Sz>::Apply([&] (size_t n)
			{
				stream << L' ' << mat[n];
				if (n < Sz - 1) stream << L',';
				stream << L'\n';
			});
			stream << std::defaultfloat;
		}
		stream << L']';

		return stream;
	}

	template<class U, size_t Sz>
	inline std::istream& operator >> (std::istream& stream, Matrix<U, Sz>& mat)
	{
		if (stream.peek() == '[')
			stream.ignore(1);

		TMP::ForEachN<Sz>::Apply([&] (size_t n)
		{
			if (n > 0 && stream.peek() == ',')
				stream.ignore(1);
			stream >> mat[n];
		});

		if (stream.peek() == ']')
			stream.ignore(1);

		return stream;
	}

	template<class U, size_t Sz>
	inline std::wistream& operator >> (std::wistream& stream, Matrix<U, Sz>& mat)
	{
		if (stream.peek() == L'[')
			stream.ignore(1);

		TMP::ForEachN<Sz>::Apply([&] (size_t n)
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

// Vector/Matrix operators
namespace Epic
{
	template<class T, size_t S>
	inline auto operator * (const Matrix<T, S>& m, const Vector<T, S>& v) noexcept
	{
		auto result = v;
		m.Transform(result);
		return result;
	}

	template<class T, size_t S>
	inline auto operator * (const Vector<T, S>& v, const Matrix<T, S>& m) noexcept
	{
		auto result = v;
		m.TransformRM(result);
		return result;
	}

	template<class T, size_t S>
	inline auto operator * (const Matrix<T, S + 1>& m, const Vector<T, S>& v) noexcept
	{
		auto result = v;
		m.Transform(result);
		return result;
	}

	template<class T, size_t S>
	inline auto operator * (const Vector<T, S>& v, const Matrix<T, S + 1>& m) noexcept
	{
		auto result = v;
		m.TransformRM(result);
		return result;
	}

	template<class T, size_t S>
	inline auto operator / (const Matrix<T, S>& m, const Vector<T, S>& v) noexcept
	{
		auto result = v;
		Matrix<T, S>::InverseOf(m).Transform(result);
		return result;
	}

	template<class T, size_t S>
	inline auto operator / (const Vector<T, S>& v, const Matrix<T, S>& m) noexcept
	{
		auto result = v;
		Matrix<T, S>::InverseOf(m).TransformRM(result);
		return result;
	}

	template<class T, size_t S>
	inline auto operator / (const Matrix<T, S + 1>& m, const Vector<T, S>& v) noexcept
	{
		auto result = v;
		Matrix<T, S + 1>::InverseOf(m).Transform(result);
		return result;
	}

	template<class T, size_t S>
	inline auto operator / (const Vector<T, S>& v, const Matrix<T, S + 1>& m) noexcept
	{
		auto result = v;
		Matrix<T, S + 1>::InverseOf(m).TransformRM(result);
		return result;
	}
}

//////////////////////////////////////////////////////////////////////////////

// Aliases
namespace Epic
{
	template<class T = float> using Matrix2 = Matrix<T, 2>;
	template<class T = float> using Matrix3 = Matrix<T, 3>;
	template<class T = float> using Matrix4 = Matrix<T, 4>;
	template<class T = float> using Transform2 = Matrix<T, 2>;
	template<class T = float> using Transform3 = Matrix<T, 3>;
	template<class T = float> using Transform4 = Matrix<T, 4>;

	using Matrix2f = Matrix2<float>;
	using Matrix2d = Matrix2<double>;
	using Matrix2i = Matrix2<int>;
	using Matrix2l = Matrix2<long>;
	using Matrix3f = Matrix3<float>;
	using Matrix3d = Matrix3<double>;
	using Matrix3i = Matrix3<int>;
	using Matrix3l = Matrix3<long>;
	using Matrix4f = Matrix4<float>;
	using Matrix4d = Matrix4<double>;
	using Matrix4i = Matrix4<int>;
	using Matrix4l = Matrix4<long>;

	using Transform2f = Transform2<float>;
	using Transform2d = Transform2<double>;
	using Transform2i = Transform2<int>;
	using Transform2l = Transform2<long>;
	using Transform3f = Transform3<float>;
	using Transform3d = Transform3<double>;
	using Transform3i = Transform3<int>;
	using Transform3l = Transform3<long>;
	using Transform4f = Transform4<float>;
	using Transform4d = Transform4<double>;
	using Transform4i = Transform4<int>;
	using Transform4l = Transform4<long>;
}

