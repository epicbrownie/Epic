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

			for (size_t c = 0; c < Sz; ++c)
			{
				const auto cd = c * ColumnType::Size;
				const auto cs = c * Matrix<U, Sz>::ColumnType::Size;

				for (size_t r = 0; r < Sz; ++r)
					Values[cd + r] = static_cast<T>(mat.Values[cs + r]);
			}
		}
		else if constexpr (Sz > ColumnCount)
		{
			// 'mat' is larger than this matrix
			for (size_t c = 0; c < ColumnCount; ++c)
			{
				const auto cd = c * ColumnType::Size;
				const auto cs = c * Matrix<U, Sz>::ColumnType::Size;

				for (size_t r = 0; r < ColumnType::Size; ++r)
					Values[cd + r] = static_cast<T>(mat.Values[cs + r]);
			}
		}
		else
		{
			// 'mat' is the same size as this matrix
			for (size_t n = 0; n < Size; ++n)
				Values[n] = static_cast<T>(mat.Values[n]);
		}
	}

	// Constructs a matrix from a list of values.
	template<class U>
	explicit Matrix(const U(&values)[Size]) noexcept
	{
		for (size_t n = 0; n < Size; ++n)
			Values[n] = static_cast<T>(values[n]);
	}

	// Constructs a matrix whose values are all set to a value
	template<class U, typename = std::enable_if_t<std::is_convertible_v<U, T>>>
	Matrix(U value) noexcept
	{
		const T cv = static_cast<T>(std::move(value));
		for (size_t n = 0; n < Size; ++n)
			Values[n] = cv;
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
	explicit Matrix(Quaternion<U> q) noexcept
	{
		MakeRotation(std::move(q));
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
		for (size_t n = 0; n < ColumnCount; ++n)
			Values[ColumnType::Size * n + n] = cv1;
	}

	// Constructs a 2D TRS matrix
	template<typename = std::enable_if_t<(S == 3)>>
	Matrix(Vector2<T> vT, Radian<T> psi, Vector2<T> vS) noexcept
	{
		MakeTRS(std::move(vT), std::move(psi), std::move(vS));
	}

	// Constructs a 3D TRS matrix
	template<typename = std::enable_if_t<(S == 4)>>
	Matrix(Vector3<T> vT, Quaternion<T> qR, Vector3<T> vS) noexcept
	{
		MakeTRS(std::move(vT), std::move(qR), std::move(vS));
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
	Matrix(const XRotationTag&, Radian<T> phi) noexcept
	{
		MakeXRotation(std::move(phi));
	}

	// Constructs a Y-axis rotation matrix
	template<typename EnabledFor3x3OrGreater = std::enable_if_t<(S >= 3)>>
	Matrix(const YRotationTag&, Radian<T> theta) noexcept
	{
		MakeYRotation(std::move(theta));
	}

	// Constructs a Z-axis rotation matrix
	template<typename EnabledFor2x2OrGreater = std::enable_if_t<(S >= 2)>>
	Matrix(const ZRotationTag&, Radian<T> psi) noexcept
	{
		MakeZRotation(std::move(psi));
	}

	// Constructs a 2D rotation matrix
	template<typename EnabledFor2x2OrGreater = std::enable_if_t<(S >= 2)>>
	Matrix(const RotationTag&, Radian<T> psi) noexcept
	{
		MakeRotation(std::move(psi));
	}

	// Constructs a 3D rotation matrix
	template<typename EnabledFor3x3OrGreater = std::enable_if_t<(S >= 3)>>
	Matrix(Radian<T> pitch, Radian<T> heading, Radian<T> roll) noexcept
	{
		MakeRotation(std::move(pitch), std::move(heading), std::move(roll));
	}

	// Constructs a 3D rotation matrix
	template<typename EnabledFor3x3OrGreater = std::enable_if_t<(S >= 3)>>
	Matrix(const RotationTag&, Vector3<T> axis, Radian<T> angle) noexcept
	{
		MakeRotation(std::move(axis), std::move(angle));
	}

	// Constructs a 3D rotation matrix
	template<class U, typename EnabledFor3x3OrGreater = std::enable_if_t<(S >= 3)>>
	Matrix(const RotationTag&, Quaternion<U> q) noexcept
		: Type(q)
	{ }

	// Constructs a shear matrix from a shear amount and the target column/row coordinates
	template<typename EnabledFor2x2OrGreater = std::enable_if_t<(S >= 2)>>
	Matrix(const ShearTag&, T shear, size_t column, size_t row) noexcept
		: Type(Epic::Identity)
	{
		assert(column >= 0 && column < ColumnCount);
		assert(row >= 0 && row < ColumnType::Size);

		Values[(ColumnType::Size * column) + row] = std::move(shear);
	}

	// Constructs a homogeneous "look at" matrix from a target position, an eye location, and an up direction
	template<typename EnabledFor4x4 = std::enable_if_t<(S == 4)>>
	Matrix(const LookAtTag&, Point3<T> target,
		   Point3<T> eye = { T(0), T(0), T(0) },
		   Normal3<T> up = { T(0), T(1), T(0) }) noexcept
	{ 
		LookAt(std::move(target), std::move(eye), std::move(up));
	}

	// Constructs a homogeneous frustum matrix from boundary values
	template<typename EnabledFor4x4 = std::enable_if_t<(S == 4)>>
	Matrix(const FrustumTag&, T left, T right, T top, T bottom, T znear, T zfar) noexcept
	{
		const auto h = top - bottom;
		const auto w = right - left;
		const auto d = zfar - znear;
		const auto n2 = T(2) * znear;
		const auto z = T(0);

		assert(h != T(0));
		assert(w != T(0));
		assert(d != T(0));

		Values = 
		{
			n2 / w, z, z, z,
			z, n2 / h, z, z,
			(right + left) / w, (top + bottom) / h, -(zfar + znear) / d, T(-1),
			z, z, (-n2*zfar) / d, z
		};
	}

	// Constructs a homogeneous perspective matrix from a field-of-view, aspect ratio, and near/far distances
	template<typename EnabledFor4x4 = std::enable_if_t<(S == 4)>>
	Matrix(const PerspectiveTag&, Radian<T> fovy, T aspectRatio, T znear, T zfar) noexcept
	{
		const auto z = T(0);
		const auto f = T(1) / (fovy / T(2)).Tan();
		const auto d = znear - zfar;

		assert(d != T(0));
		assert(aspectRatio != T(0));

		Values =
		{
			f / std::move(aspectRatio), z, z, z,
			z, f, z, z,
			z, z, (zfar + znear) / d, T(-1),
			z, z, (T(2) * zfar * znear) / d, z
		};
	}

	// Constructs a homogeneous orthographic matrix from boundary values
	template<typename EnabledFor4x4 = std::enable_if_t<(S == 4)>>
	Matrix(const OrthoTag&, T left, T right, T top, T bottom, T znear, T zfar) noexcept
	{
		const auto h = top - bottom;
		const auto w = right - left;
		const auto d = zfar - znear;
		const auto z = T(0);

		assert(h != T(0));
		assert(w != T(0));
		assert(d != T(0));

		Values =
		{
			T(2) / w, z, z, z,
			z, T(2) / h, z, z,
			z, z, T(-2) / d, z,
			- (right + left) / w, -(top + bottom) / h, -(zfar + znear) / d, T(1)
		};
	}

	// Constructs a homogeneous orthographic matrix from boundary values ([near, far] preset to [-1, 1])
	template<typename EnabledFor4x4 = std::enable_if_t<(S == 4)>>
	Matrix(const Ortho2DTag&, T left, T right, T top, T bottom) noexcept
		: Type(Epic::Ortho, std::move(left), std::move(right), std::move(top), std::move(bottom), T(-1), T(1))
	{ }

	// Constructs a homogeneous projective picking matrix from a window coordinate, picking region, and viewport boundaries
	template<typename EnabledFor4x4 = std::enable_if_t<(S == 4)>>
	Matrix(const PickingTag&, T pickx, T picky, T pickw, T pickh, T vpX, T vpY, T vpW, T vpH) noexcept
		: Type(Epic::Identity)
	{
		assert(pickw > T(0));
		assert(pickh > T(0));

		const auto cv0 = T(0);
		const auto cv1 = T(1);
		const auto cv2 = T(2);

		Values =
		{
			vpW / pickw, cv0, cv0, cv0,
			cv0, vpH / pickh, cv0, cv0,
			cv0, cv0, cv1, cv0,
			(vpW + (cv2 * (vpX - pickx))) / pickw, (vpH + (cv2 * (vpY - picky))) / pickh, cv0, cv1
		};
	}

	// Constructs a homogeneous projective shadow matrix from a ground plane and a light source
	template<typename EnabledFor4x4 = std::enable_if_t<(S == 4)>>
	Matrix(const ShadowTag&, Vector4<T> ground, Point4<T> light) noexcept
	{
		const auto dot = ground.Dot(light);

		Values =
		{
			dot - light.x * ground.x, -light.x * ground.y, -light.x * ground.z, -light.x * ground.w,
			-light.y * ground.x, dot - light.y * ground.y, -light.y * ground.z, -light.y * ground.w,
			-light.z * ground.x, -light.z * ground.y, dot - light.z * ground.z, -light.z * ground.w,
			-light.w * ground.x, -light.w * ground.y, -light.w * ground.z, dot - light.w * ground.w
		};
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

		for (size_t i = 0; i < ColumnCount; ++i)
		{
			vec.Values[i] = src.Values[0] * Values[i];

			for (size_t j = 1; j < ColumnCount; ++j)
				vec.Values[i] += src.Values[j] * Values[(ColumnType::Size * j) + i];
		}
	}

	// Multiplies this matrix and a homogenized (point) 'vec' together. (vec' = M * vec)
	template<typename EnabledFor2x2OrGreater = std::enable_if_t<(S >= 2)>>
	void Transform(Vector<T, S - 1>& vec) const noexcept
	{
		const auto src = vec;

		for (size_t i = 0; i < ColumnCount - 1; ++i)
		{
			vec.Values[i] = src.Values[0] * Values[i];

			for (size_t j = 1; j < (ColumnCount - 1); ++j)
				vec.Values[i] += src.Values[j] * Values[(ColumnType::Size * j) + i];

			vec.Values[i] += Values[(ColumnType::Size * (ColumnCount - 1)) + i];
		}
	}

	// Multiplies this matrix and 'vec' together. (Row Major) (vec' = vec * M)
	void TransformRM(Vector<T, S>& vec) const noexcept
	{
		auto src = vec;

		for (size_t i = 0; i < ColumnType::Size; ++i)
		{
			vec.Values[i] = src.Values[0] * Values[ColumnCount * i];

			for (size_t j = 1; j < ColumnType::Size; ++j)
				vec.Values[i] += src.Values[j] * Values[(ColumnCount * i) + j];
		}
	}

	// Multiplies this matrix and a homogenized (point) 'vec' together. (Row Major) (vec' = vec * M)
	template<typename EnabledFor2x2OrGreater = std::enable_if_t<(S >= 2)>>
	void TransformRM(Vector<T, S - 1>& vec) const noexcept
	{
		const auto src = vec;

		for (size_t i = 0; i < ColumnType::Size; ++i)
		{
			vec.Values[i] = src.Values[0] * Values[ColumnCount * i];

			for (size_t j = 1; j < (ColumnType::Size - 1); ++j)
				vec.Values[i] += src.Values[j] * Values[(ColumnCount * i) + j];

			vec.Values[i] += Values[(ColumnCount * i) + ColumnType::Size - 1];
		}
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
	constexpr Type& Fill(T value) noexcept
	{
		for (size_t n = 0; n < Size; ++n)
			Values[n] = value;

		return *this;
	}

	// Sets this matrix to the identity matrix
	constexpr Type& MakeIdentity() noexcept
	{
		const auto cv0 = T(0);
		const auto cv1 = T(1);

		for (size_t n = 0; n < Size; ++n)
			Values[n] = cv0;
		
		for (size_t n = 0; n < ColumnCount; ++n)
			Values[ColumnCount * n + n] = cv1;

		return *this;
	}

	// Sets this matrix to a matrix capable of placing things at position vT, oriented in rotation psi, and scaled by vS
	template<typename EnabledFor3x3 = std::enable_if_t<(S == 3)>>
	Type& MakeTRS(Vector2<T> vT, Radian<T> psi, Vector2<T> vS) noexcept
	{
		MakeRotation(std::move(psi));

		cx *= vS.x;
		cy *= vS.y;

		cz = std::move(vT);

		return *this;
	}

	// Sets this matrix to a matrix capable of placing things at position vT, oriented in rotation qR, and scaled by vS
	template<typename EnabledFor4x4 = std::enable_if_t<(S == 4)>>
	Type& MakeTRS(Vector3<T> vT, Quaternion<T> qR, Vector3<T> vS) noexcept
	{
		MakeRotation(std::move(qR));

		cx *= vS.x;
		cy *= vS.y;
		cz *= vS.z;

		cw = std::move(vT);

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
		for (size_t n = 0; n < SpanV; ++n)
			Values[DestN + n] = values.Values[n];

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
		for (size_t n = 0; n < SpanV; ++n)
			Values[ColumnCount * n + n] = values.Values[n];

		return *this;
	}

	// Sets this matrix to an X-axis rotation matrix
	template<typename EnabledFor3x3OrGreater = std::enable_if_t<(S >= 3)>>
	Type& MakeXRotation(Radian<T> phi) noexcept
	{
		MakeIdentity();

		const auto[sinx, cosx] = phi.SinCos();

		Values[1 * ColumnType::Size + 1] = cosx;
		Values[1 * ColumnType::Size + 2] = sinx;
		Values[2 * ColumnType::Size + 1] = -sinx;
		Values[2 * ColumnType::Size + 2] = cosx;

		return *this;
	}

	// Sets this matrix to a Y-axis rotation matrix
	template<typename EnabledFor3x3OrGreater = std::enable_if_t<(S >= 3)>>
	Type& MakeYRotation(Radian<T> theta) noexcept
	{
		MakeIdentity();

		const auto[sinx, cosx] = theta.SinCos();

		Values[0 * ColumnType::Size + 0] = cosx;
		Values[0 * ColumnType::Size + 2] = -sinx;
		Values[2 * ColumnType::Size + 0] = sinx;
		Values[2 * ColumnType::Size + 2] = cosx;

		return *this;
	}

	// Sets this matrix to a Z-axis rotation matrix
	template<typename EnabledFor2x2OrGreater = std::enable_if_t<(S >= 2)>>
	Type& MakeZRotation(Radian<T> psi) noexcept
	{
		MakeIdentity();

		const auto[sinx, cosx] = psi.SinCos();

		Values[0 * ColumnType::Size + 0] = cosx;
		Values[0 * ColumnType::Size + 1] = sinx;
		Values[1 * ColumnType::Size + 0] = -sinx;
		Values[1 * ColumnType::Size + 1] = cosx;

		return *this;
	}

	// Sets this matrix to a 2D rotation matrix
	template<typename EnabledFor2x2OrGreater = std::enable_if_t<(S >= 2)>>
	Type& MakeRotation(Radian<T> psi) noexcept
	{
		MakeZRotation(std::move(psi));
		return *this;
	}

	// Sets this matrix to a 3D rotation matrix
	template<typename EnabledFor3x3OrGreater = std::enable_if_t<(S >= 3)>>
	Type& MakeRotation(Radian<T> pitch, Radian<T> heading, Radian<T> roll) noexcept
	{
		return MakeRotation(Epic::Quaternion<T>
		{
			std::move(pitch),
			std::move(heading),
			std::move(roll)
		});
	}

	// Sets this matrix to a 3D rotation matrix
	template<typename EnabledFor3x3OrGreater = std::enable_if_t<(S >= 3)>>
	Type& MakeRotation(Vector3<T> axis, Radian<T> angle) noexcept
	{
		MakeIdentity();

		const auto[sinx, cosx] = angle.SinCos();
		const T cos1x = T(1) - cosx;

		const T cxx = cos1x * axis.Values[0] * axis.Values[0];
		const T cyy = cos1x * axis.Values[1] * axis.Values[1];
		const T czz = cos1x * axis.Values[2] * axis.Values[2];
		const T cxy = cos1x * axis.Values[0] * axis.Values[1];
		const T cxz = cos1x * axis.Values[0] * axis.Values[2];
		const T cyz = cos1x * axis.Values[1] * axis.Values[2];

		const T sx = sinx * axis.Values[0];
		const T sy = sinx * axis.Values[1];
		const T sz = sinx * axis.Values[2];

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
	Type& MakeRotation(Quaternion<U> q) noexcept
	{
		MakeIdentity();

		const auto qx = static_cast<T>(q.x);
		const auto qy = static_cast<T>(q.y);
		const auto qz = static_cast<T>(q.z);
		const auto qw = static_cast<T>(q.w);

		const auto qxx = qx * qx;
		const auto qyy = qy * qy;
		const auto qzz = qz * qz;
		const auto qxz = qx * qz;
		const auto qxy = qx * qy;
		const auto qyz = qy * qz;
		const auto qwx = qw * qx;
		const auto qwy = qw * qy;
		const auto qwz = qw * qz;

		const auto cv1 = T(1);
		const auto cv2 = T(2);

		Values[0 * ColumnType::Size + 0] = cv1 - cv2 * (qyy + qzz);
		Values[0 * ColumnType::Size + 1] =		 cv2 * (qxy + qwz);
		Values[0 * ColumnType::Size + 2] =		 cv2 * (qxz - qwy);
												 
		Values[1 * ColumnType::Size + 0] =		 cv2 * (qxy - qwz);
		Values[1 * ColumnType::Size + 1] = cv1 - cv2 * (qxx + qzz);
		Values[1 * ColumnType::Size + 2] =		 cv2 * (qyz + qwx);
												 
		Values[2 * ColumnType::Size + 0] =		 cv2 * (qxz + qwy);
		Values[2 * ColumnType::Size + 1] =		 cv2 * (qyz - qwx);
		Values[2 * ColumnType::Size + 2] = cv1 - cv2 * (qxx + qyy);

		return *this;
	}

	// Sets this matrix to a homogeneous lookat matrix using a target position, an eye location, and an up direction
	template<typename EnabledFor4x4 = std::enable_if_t<(S == 4)>>
	Type& LookAt(Point3<T> target,
				 Point3<T> eye = { T(0), T(0), T(0) },
				 Normal3<T> up = { T(0), T(1), T(0) }) noexcept
	{
		const auto zaxis = Vector3<T>::SafeNormalOf(target - eye);
		const auto xaxis = Vector3<T>::SafeNormalOf(zaxis.Cross(up));
		const auto yaxis = xaxis.Cross(zaxis);
		const auto z = T(0);

		Values =
		{
			xaxis.x, yaxis.x, -zaxis.x, z,
			xaxis.y, yaxis.y, -zaxis.y, z,
			xaxis.z, yaxis.z, -zaxis.z, z,
			-xaxis.Dot(eye), -yaxis.Dot(eye), zaxis.Dot(eye), T(1)
		};

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
			
			return
			{
				(Columns[1].Values[2] - Columns[2].Values[1]) / sqt,
				(Columns[2].Values[0] - Columns[0].Values[2]) / sqt,
				(Columns[0].Values[1] - Columns[1].Values[0]) / sqt,
				sqt / T(4)
			};
		}
		else if(Columns[0].Values[0] > Columns[1].Values[1] && Columns[0].Values[0] > Columns[2].Values[2])
		{
			const auto sqt = std::sqrt(T(1) + Columns[0].Values[0] - Columns[1].Values[1] - Columns[2].Values[2]) * T(2);
			
			return
			{
				sqt / T(4),
				(Columns[0].Values[1] + Columns[1].Values[0]) / sqt,
				(Columns[2].Values[0] + Columns[0].Values[2]) / sqt,
				(Columns[1].Values[2] + Columns[2].Values[1]) / sqt
			};
		}
		else if(Columns[1].Values[1] > Columns[2].Values[2])
		{
			const auto sqt = std::sqrt(T(1) + Columns[1].Values[1] - Columns[0].Values[0] - Columns[2].Values[2]) * T(2);
			
			return
			{
				(Columns[0].Values[1] + Columns[1].Values[0]) / sqt,
				sqt / T(4),
				(Columns[1].Values[2] + Columns[2].Values[1]) / sqt,
				(Columns[2].Values[0] + Columns[0].Values[2]) / sqt
			};
		}
		else
		{
			const auto sqt = std::sqrt(T(1) + Columns[2].Values[2] - Columns[0].Values[0] - Columns[1].Values[1]) * T(2);
			
			return
			{
				(Columns[2].Values[0] + Columns[0].Values[2]) / sqt,
				(Columns[1].Values[2] + Columns[2].Values[1]) / sqt,
				sqt / T(4),
				(Columns[0].Values[1] + Columns[1].Values[0]) / sqt
			};
		}
	}

public:
	// Calculates the sum of the main diagonal values
	constexpr T Trace() const noexcept
	{
		T result = T(0);

		for (size_t n = 0; n < ColumnCount; ++n)
			result += Values[(n * ColumnType::Size) + n];

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

		for (size_t i = 0; i < ColumnCount; ++i)
		{
			for (size_t j = 0; j < ColumnType::Size; ++j)
				result.Columns[i] += Columns[j] * mat.Columns[i].Values[j];
		}

		return (*this = result);
	}

	// Rearranges this matrix so that its columns become its rows
	Type& Transpose() noexcept
	{
		for (size_t i = 0; i < ColumnCount; ++i)
		{
			for (size_t j = i + 1; j < ColumnCount; ++j)
				std::swap
				(
					Values[(i * ColumnType::Size) + j],
					Values[(j * ColumnType::Size) + i]
				);
		}

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

			for (size_t i = 0; i < ColumnCount; ++i)
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
					auto cv = upper.Columns[column];
					upper.Columns[column] = upper.Columns[i];
					upper.Columns[i] = cv;

					cv = lower.Columns[column];
					lower.Columns[column] = lower.Columns[i];
					lower.Columns[i] = cv;
				}

				// Reduce the diagonal (Column multiplication ERO)
				const T tc = T(1) / upper.Values[(i * ColumnType::Size) + i];
				lower.Columns[i] *= tc;
				upper.Columns[i] *= tc;

				// Zero the column i at column > i (Column addition ERO)
				for (size_t j = i + 1; j < ColumnCount; ++j)
				{
					const T t = upper.Values[(j * ColumnType::Size) + i];
					upper.Columns[j] -= upper.Columns[i] * t;
					lower.Columns[j] -= lower.Columns[i] * t;

					upper.Values[(j * ColumnType::Size) + i] = T(0);
				}
			}

			for (size_t i = ColumnCount - 1; i > 0; --i)
			{
				for (long long j = i - 1; j >= 0; --j)
				{
					size_t js = static_cast<size_t>(j);

					const T t = upper.Values[(js * ColumnType::Size) + i];
					lower.Columns[js] -= lower.Columns[i] * t;
					upper.Columns[js] -= upper.Columns[i] * t;
				}
			}

			*this = lower;
		}

		return *this;
	}

	// Inverts this matrix under the assumption that it describes a rigid-body transformation; then transposes it.
	Type& TransposeInvertRigid() noexcept
	{
		for (size_t i = 0; i < ColumnCount - 1; ++i)
			Columns[i][ColumnType::Size - 1] = -Columns[i].Dot(Columns[ColumnCount - 1]);

		Columns[ColumnCount - 1] = T(0);
		Columns[ColumnCount - 1].Values[ColumnType::Size - 1] = T(1);

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

		for (size_t i = 0; i < N; ++i)
		{
			for (size_t j = 0; j < N; ++j)
				result.Values[dest++] = Values[src++];

			src += ColumnCount - N;
		}

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

		const auto cv0 = T(0);

		for (size_t c = 0; c < ColumnCount; ++c)
		{
			const auto o = c * ExpandedSz;
			for (size_t r = ColumnType::Size; r < ExpandedSz; ++r)
				result.Values[o + r] = cv0;
		}
		
		const auto o2 = ExpandedSz * ColumnCount;
		for (size_t r = 0; r < ExpandedSz * Amount; ++r)
			result.Values[o2 + r] = cv0;

		return result;
	}

	// Constructs a matrix from this matrix with 'Amount' additional rows and columns.
	// Added rows/columns are filled with values from the ones matrix.
	template<size_t Amount = 1>
	auto Expand(const OnesTag&) const noexcept
	{
		constexpr static size_t ExpandedSz = ColumnCount + Amount;

		Matrix<T, ExpandedSz> result(*this);

		const auto cv1 = T(1);

		for (size_t c = 0; c < ColumnCount; ++c)
		{
			const auto o = c * ExpandedSz;
			for (size_t r = ColumnType::Size; r < ExpandedSz; ++r)
				result.Values[o + r] = cv1;
		}

		const auto o2 = ExpandedSz * ColumnCount;
		for (size_t r = 0; r < ExpandedSz * Amount; ++r)
			result.Values[o2 + r] = cv1;

		return result;
	}

	// Constructs a matrix from this matrix with 'Amount' additional rows and columns.
	// Added rows/columns are filled with values from the identity matrix.
	template<size_t Amount = 1>
	auto Expand(const IdentityTag&) const noexcept
	{
		constexpr static size_t ExpandedSz = ColumnCount + Amount;

		Matrix<T, ExpandedSz> result(*this);

		const auto cv0 = T(0);
		const auto cv1 = T(1);

		for (size_t c = 0; c < ColumnCount; ++c)
		{
			const auto o = c * ExpandedSz;
			for (size_t r = ColumnType::Size; r < ExpandedSz; ++r)
				result.Values[o + r] = cv0;
		}

		for (size_t c = ColumnCount; c < ExpandedSz; ++c)
		{
			const auto o2 = ExpandedSz * c;
			for (size_t r = 0; r < ExpandedSz; ++r)
				result.Values[o2 + r] = (c == r) ? cv1 : cv0;
		}
		
		return result;
	}

public:
	// Returns 'matA' * 'matB'
	static Type CompositeOf(const Type& matA, const Type& matB) noexcept
	{
		return Type(matA).Compose(matB);
	}

	// Copies 'mat' and transposes it
	static Type TransposeOf(const Type& mat) noexcept
	{
		return Type(mat).Transpose();
	}

	// Copies 'mat' and inverts it under the assumption that it describes a rigid-body transformation.
	static Type RigidInverseOf(const Type& mat) noexcept
	{
		return Type(mat).InvertRigid();
	}

	// Copies 'mat' and inverts it
	static Type InverseOf(const Type& mat) noexcept
	{
		return Type(mat).Invert();
	}

	// Copies 'mat'. The copy is then inverted under the assumption that it describes a rigid-body transformation and then transposed.
	static Type TransposedRigidInverseOf(const Type& mat) noexcept
	{
		return Type(mat).TransposeInvertRigid();
	}

	// Copies 'mat'. The copy is then inverted and then transposed.
	static Type TransposedInverseOf(const Type& mat) noexcept
	{
		return Type(mat).TransposeInvert();
	}

public:
	// Copy this matrix with negated values
	Type operator - () const noexcept
	{
		Type result;

		for (size_t n = 0; n < Size; ++n)
			result.Values[n] = -Values[n];

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

	#define CREATE_SCALAR_ASSIGNMENT_OPERATOR(Op)					\
																	\
	Type& operator Op (T value) noexcept							\
	{																\
		for (size_t n = 0; n < Size; ++n)							\
			Values[n] Op value;										\
																	\
		return *this;												\
	}																\
																	\
	template<class U,												\
		typename = std::enable_if_t<std::is_convertible_v<U, T>>>	\
	Type& operator Op (const U(&values)[Size]) noexcept				\
	{																\
		for (size_t n = 0; n < Size; ++n)							\
			Values[n] Op static_cast<T>(values[n]);					\
																	\
		return *this;												\
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
		for (size_t n = 0; n < Size; ++n)							\
			Values[n] Op mat.Values[n];								\
																	\
		return *this;												\
	}																\
																	\
	template<class U,												\
		typename = std::enable_if_t<std::is_convertible_v<U, T>>>	\
	Type& operator Op (const Matrix<U, S>& mat) noexcept			\
	{																\
		for (size_t n = 0; n < Size; ++n)							\
			Values[n] Op static_cast<T>(mat.Values[n]);				\
																	\
		return *this;												\
	}

	CREATE_MATRIX_ASSIGNMENT_OPERATOR(= );
	CREATE_MATRIX_ASSIGNMENT_OPERATOR(+= );
	CREATE_MATRIX_ASSIGNMENT_OPERATOR(-= );

	#undef CREATE_MATRIX_ASSIGNMENT_OPERATOR

	//////

	#define CREATE_LOGIC_ASSIGNMENT_OPERATOR(Op)						\
																		\
	Type& operator Op (T value) noexcept								\
	{																	\
		if constexpr (std::is_integral_v<T>)							\
		{																\
			for (size_t n = 0; n < Size; ++n)							\
				Values[n] Op value;										\
		}																\
		else static_assert(false,										\
			"Logical operators are only valid for integral types.");	\
		return *this;													\
	}																	\
																		\
	template<class U,													\
		typename = std::enable_if_t<std::is_convertible_v<U, T>>>		\
	Type& operator Op (const U(&values)[Size]) noexcept					\
	{																	\
		if constexpr (std::is_integral_v<T> && std::is_integral_v<U>)	\
		{																\
			for (size_t n = 0; n < Size; ++n)							\
				Values[n] Op static_cast<T>(values[n]);					\
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
			for (size_t n = 0; n < Size; ++n)							\
				Values[n] Op mat.Values[n];								\
		}																\
		else static_assert(false,										\
			"Logical operators are only valid for integral types.");	\
		return *this;													\
	}																	\
																		\
	template<class U,													\
		typename = std::enable_if_t<std::is_convertible_v<U, T>>>		\
	Type& operator Op (const Matrix<U, S>& mat) noexcept				\
	{																	\
		if constexpr (std::is_integral_v<T> && std::is_integral_v<U>)	\
		{																\
			for (size_t n = 0; n < Size; ++n)							\
				Values[n] Op static_cast<T>(mat.Values[n]);				\
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
	Type operator Op (T value) const noexcept							\
	{																	\
		return Type(*this) Op= std::move(value);						\
	}																	\
																		\
	friend Type operator Op (T value, const Type& mat) noexcept			\
	{																	\
		return Type(mat) Op= std::move(value);							\
	}																	\
																		\
	template<class U,													\
		typename = std::enable_if_t<std::is_convertible_v<U, T>>>		\
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
	template<class U,												\
		typename = std::enable_if_t<std::is_convertible_v<U, T>>>	\
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
	void PlaceAt(size_t offset, const Vector<U, Sz>& vec) noexcept
	{
		for (size_t n = 0; n < Sz; ++n)
			Values[offset++] = static_cast<T>(vec.Values[n]);
	}

	template<class VectorT, size_t TS, size_t... Is>
	void PlaceAt(size_t offset, const Swizzler<VectorT, TS, Is...>& value) noexcept
	{
		PlaceAt(offset, value.ToVector());
	}

	template<class U, size_t N>
	void PlaceAt(size_t offset, const U(&values)[N]) noexcept
	{
		for (size_t n = 0; n < N; ++n)
			Values[offset++] = static_cast<T>(values[n]);
	}

	template<class U, size_t Sz>
	void PlaceAt(size_t offset, const std::array<U, Sz>& values) noexcept
	{
		for (size_t n = 0; n < Sz; ++n)
			Values[offset++] = static_cast<T>(values[n]);
	}

	template<class Val>
	void PlaceAt(size_t offset, Val value) noexcept
	{
		Values[offset] = std::move(static_cast<T>(value));
	}

	#pragma endregion

	#pragma region Determinant Helpers

	template<size_t N>
	T DeterminantHelper() const noexcept
	{
		if constexpr (N == 0)
			return 0;

		else if constexpr (N == 1)
			return Values[0];

		else if constexpr (N == 2)
			return (Values[0] * Values[3]) - (Values[1] * Values[2]);

		else if constexpr (N == 3)
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
			auto minors = MinorsHelper<N>();

			// Transform it into a cofactor vector
			for (size_t i = 1; i < N; i += 2)
				minors.Values[i] = -minors.Values[i];

			// Calculate the determinant
			return minors.Dot(Columns[0]);
		}
	}

	template<size_t N>
	auto MinorsHelper() const noexcept
	{
		Vector<T, N> minors;
		Matrix<T, N - 1> minor;

		for (size_t c = 0; c < N; ++c)
		{
			// Construct the minor matrix of [0, c]
			size_t d = 0;

			for (size_t i = 1; i < N; ++i)
			{
				for (size_t r = 0; r < N; ++r)
				{
					if (r != c)
						minor.Values[d++] = Values[(i * N) + r];
				}
			}

			// Set this minors value to the minor's determinant
			minors.Values[c] = minor.DeterminantHelper<N - 1>();
		}

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
			for (size_t n = 0; n < Sz; ++n)
			{
				stream << ' ' << mat[n];
				if (n < Sz - 1) stream << ',';
				stream << '\n';
			}
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
			for (size_t n = 0; n < Sz; ++n)
			{
				stream << L' ' << mat[n];
				if (n < Sz - 1) stream << L',';
				stream << L'\n';
			}
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

		for (size_t n = 0; n < Sz; ++n)
		{
			if (n > 0 && stream.peek() == ',')
				stream.ignore(1);
			stream >> mat[n];
		}

		if (stream.peek() == ']')
			stream.ignore(1);

		return stream;
	}

	template<class U, size_t Sz>
	inline std::wistream& operator >> (std::wistream& stream, Matrix<U, Sz>& mat)
	{
		if (stream.peek() == L'[')
			stream.ignore(1);

		for (size_t n = 0; n < Sz; ++n)
		{
			if (n > 0 && stream.peek() == L',')
				stream.ignore(1);
			stream >> mat[n];
		}

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
	inline auto operator * (const Matrix<T, S>& m, Vector<T, S> v) noexcept
	{
		auto result = std::move(v);
		m.Transform(result);
		return result;
	}

	template<class T, size_t S>
	inline auto operator * (Vector<T, S> v, const Matrix<T, S>& m) noexcept
	{
		auto result = std::move(v);
		m.TransformRM(result);
		return result;
	}

	template<class T, size_t S>
	inline auto operator * (const Matrix<T, S + 1>& m, Vector<T, S> v) noexcept
	{
		auto result = std::move(v);
		m.Transform(result);
		return result;
	}

	template<class T, size_t S>
	inline auto operator * (Vector<T, S> v, const Matrix<T, S + 1>& m) noexcept
	{
		auto result = std::move(v);
		m.TransformRM(result);
		return result;
	}

	template<class T, size_t S>
	inline auto operator / (const Matrix<T, S>& m, Vector<T, S> v) noexcept
	{
		auto result = std::move(v);
		Matrix<T, S>::InverseOf(m).Transform(result);
		return result;
	}

	template<class T, size_t S>
	inline auto operator / (Vector<T, S> v, const Matrix<T, S>& m) noexcept
	{
		auto result = std::move(v);
		Matrix<T, S>::InverseOf(m).TransformRM(result);
		return result;
	}

	template<class T, size_t S>
	inline auto operator / (const Matrix<T, S + 1>& m, Vector<T, S> v) noexcept
	{
		auto result = std::move(v);
		Matrix<T, S + 1>::InverseOf(m).Transform(result);
		return result;
	}

	template<class T, size_t S>
	inline auto operator / (Vector<T, S> v, const Matrix<T, S + 1>& m) noexcept
	{
		auto result = std::move(v);
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

