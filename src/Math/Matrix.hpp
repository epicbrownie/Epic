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
#include <Epic/Math/MatrixHelpers.hpp>
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
	using ValueType = typename Base::ValueType;
	constexpr static size_t Size = Base::Size;

	using RowType = typename Base::RowType;
	constexpr static size_t RowCount = Base::RowCount;

private:
	using Base::Rows;
	using Base::Values;

public:
	#pragma region Constructors

	Matrix() noexcept = default;
	Matrix(const Type&) noexcept = default;
	Matrix(Type&&) noexcept = default;

	// Copy-constructs a matrix
	template<class U, size_t Sz>
	inline Matrix(const Matrix<U, Sz>& mat) noexcept
	{
		// These branches should be optimized away (TODO: constexpr if when available)
		if (Sz < RowCount)
		{
			// 'mat' is smaller than this matrix
			Identity();

			ForEach<Sz>([&](size_t r)
			{
				for (size_t c = 0; c < Sz; ++c)
					Values[(r * RowType::Size) + c] = T(mat.Values[(r * Matrix<U, Sz>::RowType::Size) + c]);
			});
		}
		else if (Sz > RowCount)
		{
			// 'mat' is larger than this matrix
			ForEach<RowCount>([&](size_t r) 
			{
				for (size_t c = 0; c < RowType::Size; ++c)
					Values[(r * RowType::Size) + c] = T(mat.Values[(r * Matrix<U, Sz>::RowType::Size) + c]);
			});
		}
		else
		{
			// 'mat' is the same size as this matrix
			ForEach<Size>([&](size_t n) { Values[n] = T(mat[n]); });
		}
	}

	// Constructs a matrix filled with 'value'
	inline Matrix(const T value) noexcept
	{
		ForEach<Size>([&](size_t n) { Values[n] = value; });
	}

	// Constructs a matrix from a list of rows
	inline Matrix(std::initializer_list<RowType> rows) noexcept
	{
		std::copy
		(
			std::begin(rows),
			std::next(std::begin(rows), std::min(rows.size(), RowCount)),
			std::begin(Rows)
		);
	}

	// Constructs a matrix from a list of values
	inline Matrix(std::initializer_list<ValueType> values) noexcept
	{
		std::copy
		(
			std::begin(values),
			std::next(std::begin(values), std::min(values.size(), Size)),
			std::begin(Values)
		);
	}

	// Constructs a matrix from a span of values
	template<class Arg, class... Args, 
		typename = std::enable_if_t<(detail::Span<Arg, Args...>::Value == Size)>>
	inline Matrix(Arg&& arg, Args&&... args) noexcept
	{
		Construct(std::forward<Arg>(arg), std::forward<Args>(args)...);
	}

	// Constructs a matrix of zeroes
	inline Matrix(const ZeroesMatrixTag&) noexcept
		: Matrix(T(0))
	{ }

	// Constructs a matrix of ones
	inline Matrix(const OnesMatrixTag&) noexcept
		: Matrix(T(1))
	{ }

	// Constructs an identity matrix
	inline Matrix(const IdentityMatrixTag&) noexcept
		: Matrix(T(0))
	{
		ForEach<RowCount>([&](size_t n) { Values[RowType::Size * n + n] = T(1); });
	}

	// Constructs a translation matrix from a span of values
	template<class Arg, class... Args, 
		typename = std::enable_if_t<(detail::Span<Arg, Args...>::Value <= RowType::Size)>>
	inline Matrix(const TranslationMatrixTag&, Arg&& arg, Args&&... args) noexcept
	{
		Translate(std::forward<Arg>(arg), std::forward<Args>(args)...);
	}

	// Constructs a scale matrix from a span of values
	template<class Arg, class... Args,
		typename = std::enable_if_t<(detail::Span<Arg, Args...>::Value <= RowType::Size)>>
	inline Matrix(const ScaleMatrixTag&, Arg&& arg, Args&&... args) noexcept
	{
		Scale(std::forward<Arg>(arg), std::forward<Args>(args)...);
	}
	
	// Constructs an X-axis rotation matrix
	template<typename EnabledFor3x3OrGreater = std::enable_if_t<(S >= 3)>>
	inline Matrix(const XRotationMatrixTag&, const Radian<T>& phi) noexcept
	{
		RotateX(phi);
	}

	// Constructs a Y-axis rotation matrix
	template<typename EnabledFor3x3OrGreater = std::enable_if_t<(S >= 3)>>
	inline Matrix(const YRotationMatrixTag&, const Radian<T>& theta) noexcept
	{
		RotateY(theta);
	}

	// Constructs a Z-axis rotation matrix
	template<typename EnabledFor2x2OrGreater = std::enable_if_t<(S >= 2)>>
	inline Matrix(const ZRotationMatrixTag&, const Radian<T>& psi) noexcept
		: Matrix(IdentityMatrix)
	{
		const T sinx = psi.Sin();
		const T cosx = psi.Cos();

		Values[0 * RowType::Size + 0] = cosx;
		Values[0 * RowType::Size + 1] = sinx;
		Values[1 * RowType::Size + 0] = -sinx;
		Values[1 * RowType::Size + 1] = cosx;
	}

	// Constructs a 2D rotation matrix
	template<typename EnabledFor2x2OrGreater = std::enable_if_t<(S >= 2)>>
	inline Matrix(const RotationMatrixTag&, const Radian<T>& psi) noexcept
	{
		Rotate(psi);
	}

	// Constructs a 3D rotation matrix
	template<typename EnabledFor3x3OrGreater = std::enable_if_t<(S >= 3)>>
	inline Matrix(const RotationMatrixTag&, const Vector3<T>& axis, const Radian<T>& angle) noexcept
	{
		Rotate(axis, angle);
	}

	// Constructs a shear matrix from a shear amount and the target row/column coordinates
	template<typename EnabledFor2x2OrGreater = std::enable_if_t<(S >= 2)>>
	inline Matrix(const ShearMatrixTag&, const T shear, const size_t row, const size_t column) noexcept
		: Matrix(IdentityMatrix)
	{
		assert(row >= 0 && row < RowCount);
		assert(column >= 0 && column < RowType::Size);

		Values[(RowType::Size * row) + column] = shear;
	}

	// Constructs a homogeneous "look at" matrix from a target position, an eye location, and an up direction
	template<typename EnabledFor4x4 = std::enable_if_t<(S == 4)>>
	inline Matrix(const LookAtMatrixTag&, const Point3<T>& target,
				  const Point3<T>& eye = { T(0), T(0), T(0) },
				  const Normal3<T>& up = { T(0), T(1), T(0) }) noexcept
	{ 
		LookAt(target, eye, up);
	}

	// Constructs a homogeneous frustum matrix from boundary values
	template<typename EnabledFor4x4 = std::enable_if_t<(S == 4)>>
	inline Matrix(const FrustumMatrixTag&, const T left, const T right, const T top, const T bottom, const T znear, const T zfar) noexcept
	{
		const auto h = top - bottom;
		const auto w = right - left;
		const auto d = zfar - znear;
		const auto n2 = T(2) * znear;
		const auto z = T(0);

		assert(h != T(0));
		assert(w != T(0));
		assert(d != T(0));

		Rows[0].Reset(n2 / w, z, z, z);
		Rows[1].Reset(z, n2 / h, z, z);
		Rows[2].Reset((right + left) / w, (top + bottom) / h, -(zfar + znear) / d, T(-1));
		Rows[3].Reset(z, z, (-n2*zfar) / d, z);
	}

	// Constructs a homogeneous perspective matrix from a field-of-view, aspect ratio, and near/far distances
	template<typename EnabledFor4x4 = std::enable_if_t<(S == 4)>>
	inline Matrix(const PerspectiveMatrixTag&, const Radian<T>& fovy, const T aspectRatio, const T znear, const T zfar) noexcept
	{
		const auto z = T(0);
		const auto f = T(1) / (fovy / T(2)).Tan();
		const auto d = znear - zfar;

		assert(d != T(0));
		assert(aspectRatio != T(0));

		Rows[0].Reset(f / aspectRatio, z, z, z);
		Rows[1].Reset(z, f, z, z);
		Rows[2].Reset(z, z, (zfar + znear) / d, T(-1));
		Rows[3].Reset(z, z, (T(2) * zfar * znear) / d, z);
	}

	// Constructs a homogeneous orthographic matrix from boundary values
	template<typename EnabledFor4x4 = std::enable_if_t<(S == 4)>>
	inline Matrix(const OrthoMatrixTag&, const T left, const T right, const T top, const T bottom, const T znear, const T zfar) noexcept
	{
		const auto h = top - bottom;
		const auto w = right - left;
		const auto d = zfar - znear;
		const auto z = T(0);

		assert(h != T(0));
		assert(w != T(0));
		assert(d != T(0));

		Rows[0].Reset(T(2) / w, z, z, z);
		Rows[1].Reset(z, T(2) / h, z, z);
		Rows[2].Reset(z, z, T(-2) / d, z);
		Rows[3].Reset(-(right + left) / w, -(top + bottom) / h, -(zfar + znear) / d, T(1));
	}

	// Constructs a homogeneous orthographic matrix from boundary values ([near, far] preset to [-1, 1])
	template<typename EnabledFor4x4 = std::enable_if_t<(S == 4)>>
	inline Matrix(const Ortho2DMatrixTag&, const T left, const T right, const T top, const T bottom) noexcept
		: Matrix(OrthoMatrix, left, right, top, bottom, T(-1), T(1))
	{ }

	// Constructs a homogeneous projective picking matrix from a window coordinate, picking region, and viewport boundaries
	template<typename EnabledFor4x4 = std::enable_if_t<(S == 4)>>
	inline Matrix(const PickingMatrixTag&, 
				  const T pickx, const T picky, const T pickw, const T pickh, 
				  const T vpX, const T vpY, const T vpW, const T vpH) noexcept
		: Matrix(IdentityMatrix)
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
	inline Matrix(const ShadowMatrixTag&, const Vector4<T>& ground, const Point4<T>& light) noexcept
	{
		const auto dot = ground.Dot(light);

		Rows[0].Reset(dot - light[0] * ground[0], -light[0] * ground[1], -light[0] * ground[2], -light[0] * ground[3]);
		Rows[1].Reset(-light[1] * ground[0], dot - light[1] * ground[1], -light[1] * ground[2], -light[1] * ground[3]);
		Rows[2].Reset(-light[2] * ground[0], -light[2] * ground[1], dot - light[2] * ground[2], -light[2] * ground[3]);
		Rows[3].Reset(-light[3] * ground[0], -light[3] * ground[1], -light[3] * ground[2], dot - light[3] * ground[3]);
	}

	#pragma endregion

public:
	inline RowType& at(const size_t index) noexcept
	{
		assert(index >= 0 && index < RowCount);

		return Rows[index];
	}

	inline const RowType& at(const size_t index) const noexcept
	{
		assert(index >= 0 && index < RowCount);

		return Rows[index];
	}

	inline RowType& operator[] (const size_t index) noexcept
	{
		assert(index >= 0 && index < RowCount);

		return Rows[index];
	}

	inline const RowType& operator[] (const size_t index) const noexcept
	{
		assert(index >= 0 && index < RowCount);

		return Rows[index];
	}

	inline decltype(std::begin(Rows)) begin() noexcept
	{
		return std::begin(Rows);
	}

	constexpr decltype(std::begin(Rows)) begin() const noexcept
	{
		return std::begin(Rows);
	}

	inline decltype(std::end(Rows)) end() noexcept
	{
		return std::end(Rows);
	}

	constexpr decltype(std::end(Rows)) end() const noexcept
	{
		return std::end(Rows);
	}

	constexpr size_t size() const noexcept
	{
		return RowCount;
	}

	inline decltype(std::data(Rows)) data() noexcept
	{
		return std::data(Rows);
	}

	constexpr decltype(std::data(Rows)) data() const noexcept
	{
		return std::data(Rows);
	}

public:
	// Multiplies this matrix and 'vec' together. (vec' = vec * M)
	inline void Transform(Vector<T, S>& vec) const noexcept
	{
		const auto src = vec;

		// NOTE: The optimizer should unroll most or all of this
		ForEach<RowCount>([&](const size_t c)
		{
			vec[c] = src[0] * Values[c];

			for (size_t j = 1; j < RowCount; ++j)
				vec[c] += src[j] * Values[(RowType::Size * j) + c];
		});
	}

	// Multiplies this matrix and a homogenized 'vec' together. (vec' = vec * M)
	template<typename EnabledFor2x2OrGreater = std::enable_if_t<(S >= 2)>>
	inline void Transform(Vector<T, S - 1>& vec) const noexcept
	{
		const auto src = vec;

		// NOTE: The optimizer should unroll most or all of this
		ForEach<RowCount - 1>([&](const size_t c)
		{
			vec[c] = src[0] * Values[c];

			for (size_t j = 1; j < (RowCount - 1); ++j)
				vec[c] += src[j] * Values[(RowType::Size * j) + c];

			vec[c] += Values[(RowType::Size * (RowCount - 1)) + c];
		});
	}

public:
	// Sets this matrix to the zero matrix
	inline void Zero() noexcept
	{
		ForEach<Size>([&](size_t n) { Values[n] = T(0); });
	}

	// Sets this matrix to the ones matrix
	inline void One() noexcept
	{
		ForEach<Size>([&](size_t n) { Values[n] = T(1); });
	}

	// Sets this matrix to the identity matrix
	inline void Identity() noexcept
	{
		Zero();
		ForEach<RowCount>([&](size_t n) { Values[RowCount * n + n] = T(1); });
	}

	// Sets this matrix to a translation matrix
	template<class Arg, class... Args,
		typename = std::enable_if_t<(detail::Span<Arg, Args...>::Value <= RowType::Size)>>
	inline void Translate(Arg&& arg, Args&&... args) noexcept
	{
		static constexpr size_t SpanV = detail::Span<Arg, Args...>::Value;
		static constexpr size_t DestN = Size - RowType::Size;
		
		Identity();

		Vector<T, SpanV> values{ std::forward<Arg>(arg), std::forward<Args>(args)... };
		ForEach<SpanV>([&](size_t i) { Values[DestN + i] = values[i]; });
	}

	// Sets this matrix to a scale matrix
	template<class Arg, class... Args,
		typename = std::enable_if_t<(detail::Span<Arg, Args...>::Value <= RowType::Size)>>
	inline void Scale(Arg&& arg, Args&&... args) noexcept
	{
		static constexpr size_t SpanV = detail::Span<Arg, Args...>::Value;
		
		Identity();

		Vector<T, SpanV> values{ std::forward<Arg>(arg), std::forward<Args>(args)... };
		ForEach<SpanV>([&](size_t i) { Values[RowCount * i + i] = values[i]; });
	}

	// Sets this matrix to an X-axis rotation matrix
	template<typename EnabledFor3x3OrGreater = std::enable_if_t<(S >= 3)>>
	inline void RotateX(const Radian<T>& phi) noexcept
	{
		Identity();

		const T sinx = phi.Sin();
		const T cosx = phi.Cos();

		Values[1 * RowType::Size + 1] = cosx;
		Values[1 * RowType::Size + 2] = sinx;
		Values[2 * RowType::Size + 1] = -sinx;
		Values[2 * RowType::Size + 2] = cosx;
	}

	// Sets this matrix to a Y-axis rotation matrix
	template<typename EnabledFor3x3OrGreater = std::enable_if_t<(S >= 3)>>
	inline void RotateY(const Radian<T>& theta) noexcept
	{
		Identity();

		const T sinx = theta.Sin();
		const T cosx = theta.Cos();

		Values[0 * RowType::Size + 0] = cosx;
		Values[0 * RowType::Size + 2] = -sinx;
		Values[2 * RowType::Size + 0] = sinx;
		Values[2 * RowType::Size + 2] = cosx;
	}

	// Sets this matrix to a Z-axis rotation matrix
	template<typename EnabledFor2x2OrGreater = std::enable_if_t<(S >= 2)>>
	inline void RotateZ(const Radian<T>& psi) noexcept
	{
		Identity();

		const T sinx = psi.Sin();
		const T cosx = psi.Cos();

		Values[0 * RowType::Size + 0] = cosx;
		Values[0 * RowType::Size + 1] = sinx;
		Values[1 * RowType::Size + 0] = -sinx;
		Values[1 * RowType::Size + 1] = cosx;
	}

	// Sets this matrix to a 2D rotation matrix
	template<typename EnabledFor2x2OrGreater = std::enable_if_t<(S >= 2)>>
	inline void Rotate(const Radian<T>& psi) noexcept
	{
		RotateZ(psi);
	}

	// Sets this matrix to a 3D rotation matrix
	template<typename EnabledFor3x3OrGreater = std::enable_if_t<(S >= 3)>>
	void Rotate(const Vector3<T>& axis, const Radian<T>& angle) noexcept
	{
		Identity();

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

		Values[0 * RowType::Size + 0] = cxx + cosx;
		Values[0 * RowType::Size + 1] = cxy + sz;
		Values[0 * RowType::Size + 2] = cxz - sy;

		Values[1 * RowType::Size + 0] = cxy - sz;
		Values[1 * RowType::Size + 1] = cyy + cosx;
		Values[1 * RowType::Size + 2] = cyz + sx;

		Values[2 * RowType::Size + 0] = cxz + sy;
		Values[2 * RowType::Size + 1] = cyz - sx;
		Values[2 * RowType::Size + 2] = czz + cosx;
	}

	// Sets this matrix to a homogeneous lookat matrix using a target position, an eye location, and an up direction
	template<typename EnabledFor4x4 = std::enable_if_t<(S == 4)>>
	void LookAt(const Point3<T>& target,
				const Point3<T>& eye = { T(0), T(0), T(0) },
				const Normal3<T>& up = { T(0), T(1), T(0) }) noexcept
	{
		const auto zaxis = Vector3<T>::SafeNormalOf(target - eye);
		const auto xaxis = Vector3<T>::SafeNormalOf(up.Cross(zaxis));
		const auto yaxis = zaxis.Cross(xaxis);
		const auto z = T(0);

		Rows[0].Reset(xaxis.x, yaxis.x, zaxis.x, z);
		Rows[1].Reset(xaxis.y, yaxis.y, zaxis.y, z);
		Rows[2].Reset(xaxis.z, yaxis.z, zaxis.z, z);
		Rows[3].Reset(-xaxis.Dot(eye), -yaxis.Dot(eye), -zaxis.Dot(eye), T(1));
	}

public:
	// Calculates the sum of the main diagonal values
	inline T Trace() const noexcept
	{
		T result = T(0);
		
		ForEach<RowCount>([&](size_t i) 
		{
			result += Values[(i * RowType::Size) + i];
		});

		return result;
	}

	// Calculates the determinant of this matrix.
	inline T Determinant() const noexcept
	{
		return DeterminantHelper<RowCount>();
	}

	// Multiplies this matrix and 'mat' together. (M' = M * m)
	Type& Compose(const Type& mat) noexcept
	{
		Type result;
		
		ForEach<RowCount>([&](size_t r) 
		{
			for(size_t c=0; c<RowType::Size; ++c)
			{
				const size_t vi = (r * RowType::Size) + c;
				result[vi] = T(0);

				for(size_t v=0; v<RowType::Size; ++v)
					result[vi] += Values[(r * RowType::Size) + v] * mat.Values[(v * RowType::Size) + c];
			}
		});

		return (*this = result);
	}

	// Rearranges this matrix so that its columns become its rows
	Type& Transpose() noexcept
	{
		ForEach<RowCount>([&](size_t i) 
		{
			for (size_t j = i + 1; j < RowCount; ++j)
				std::swap
				(
					Values[(i * RowType::Size) + j], 
					Values[(j * RowType::Size) + i]
				);
		});
		
		return *this;
	}

	// Inverts this matrix under the assumption that it describes a rigid-body transformation.
	inline Type& InvertRigid() noexcept
	{
		return TransposeInvertRigid().Transpose();
	}

	// Inverts this matrix. (s.t. M * inverse(M) = identity(M))
	Type& Invert() noexcept
	{
		const T det = Determinant();
		if (det == T(0))
			return *this;

		// These branches should be optimized away (TODO: constexpr if when available)
		if (RowCount == 1)
		{
			if (Values[0] != T(0))
				Values[0] = T(1) / det;
		}
		else if (RowCount == 2)
		{
			const auto t = Values[0];

			Values[0] = Values[3];
			Values[1] = -Values[1];
			Values[2] = -Values[2];
			Values[3] = t;

			*this *= T(1) / det;
		}
		else if (RowCount == 3)
		{
			Type adj
			{
				 (Values[4] * Values[8]) - (Values[5] * Values[7]),
				-(Values[1] * Values[8]) - (Values[2] * Values[7]),
				 (Values[1] * Values[5]) - (Values[2] * Values[4]),
				-(Values[3] * Values[8]) - (Values[5] * Values[6]),
				 (Values[0] * Values[8]) - (Values[2] * Values[6]),
				-(Values[0] * Values[5]) - (Values[2] * Values[3]),
				 (Values[3] * Values[7]) - (Values[4] * Values[6]),
				-(Values[0] * Values[7]) - (Values[1] * Values[6]),
				 (Values[0] * Values[4]) - (Values[1] * Values[3])
			};

			*this = adj;
			*this *= T(1) / det;
		}
		else
		{
			Type lower = IdentityMatrix;
			Type& upper = *this;

			ForEach<RowCount>([&](size_t r) 
			{
				// Partial pivoting (Row Switching ERO)
				T v = std::abs(upper.Values[(r * RowType::Size) + r]);
				size_t row = r;

				for (size_t j = r + 1; j < RowCount; ++j)
				{
					const T iv = std::abs(upper.Values[(j * RowType::Size) + r]);
					if (iv > v)
					{
						row = j;
						v = iv;
					}
				}

				if (row != r)
				{
					auto rv = upper[row];
					upper[row] = upper[r];
					upper[r] = rv;

					rv = lower[row];
					lower[row] = lower[r];
					lower[r] = rv;
				}

				// Reduce the diagonal (Row multiplication ERO)
				const T tr = T(1) / upper.Values[(r * RowType::Size) + r];
				lower[r] *= tr;
				upper[r] *= tr;

				// Zero the column r at row > r (Row addition ERO)
				for (size_t j = r + 1; j < RowCount; ++j)
				{
					const T t = upper.Values[(j * RowType::Size) + r];
					upper[j] -= upper[r] * t;
					lower[j] -= lower[r] * t;

					upper.Values[(j * RowType::Size) + r] = T(0);
				}
			});

			for (size_t i = RowCount - 1; i > 0; --i)
			{
				for (long long j = i - 1; j >= 0; --j)
				{
					const T t = upper.Values[(j * RowType::Size) + i];
					lower[j] -= lower[i] * t;
					upper[j] -= upper[i] * t;
				}
			}

			*this = lower;
		}

		return *this;
	}

	// Inverts this matrix under the assumption that it describes a rigid-body transformation; then transposes it.
	inline Type& TransposeInvertRigid() noexcept
	{
		ForEach<RowCount - 1>([&](size_t i)
		{
			Rows[i][RowType::Size - 1] = -Rows[i].Dot(Rows[RowCount - 1]);
		});

		Rows[RowCount - 1] = T(0);
		Rows[RowCount - 1][RowType::Size - 1] = T(1);

		// NOTE: *this is now equal to the transposed inverse

		return *this;
	}

	// Inverts this matrix; then transposes it.
	inline Type& TransposeInvert() noexcept
	{
		Invert();
		return Transpose();
	}

	// Constructs a matrix from the square region between [I, I + N]
	template<size_t I = 0, size_t N = RowCount,
			 typename InvalidSlice = std::enable_if_t<((I + N) <= RowCount)>>
	Matrix<T, N> Slice() const noexcept
	{
		Matrix<T, N> result;
		size_t src = (RowCount * I) + I;
		size_t dest = 0;

		ForEach<N>([&](size_t)
		{
			for (size_t j = 0; j < N; ++j)
				result.Values[dest++] = Values[src++];

			src += RowCount - N;
		});

		return result;
	}

	// Constructs a matrix from this matrix, less 'Amount' rows and columns
	// NOTE: Equivalent to Slice<0, S - Amount>()
	template<size_t Amount = 1, typename ResultSizeMustBeGreaterThan0 = std::enable_if_t<(RowCount > Amount)>>
	auto Contract() const noexcept
	{
		return Slice<0, RowCount - Amount>();
	}

	// Constructs a matrix from this matrix with 'Amount' additional rows and columns.
	// Added rows/columns are filled with values from the identity matrix.
	template<size_t Amount = 1>
	inline auto Expand() const noexcept
	{
		return Expand<Amount>(IdentityMatrix);
	}

	// Constructs a matrix from this matrix with 'Amount' additional rows and columns.
	// Added rows/columns are filled with values from the zeroes matrix.
	template<size_t Amount = 1>
	auto Expand(const ZeroesMatrixTag&) const noexcept
	{
		constexpr static size_t Expanded = RowCount + Amount;

		Matrix<T, Expanded> result(*this);

		ForEach<RowCount>([&](size_t r)
		{
			for (size_t c = RowType::Size; c < Expanded; ++c)
				result.Values[(r * Expanded) + c] = T(0);
		});

		ForEach<Expanded * Amount>([&](size_t c)
		{
			result.Values[(Expanded * RowCount) + c] = T(0);
		});

		return result;
	}

	// Constructs a matrix from this matrix with 'Amount' additional rows and columns.
	// Added rows/columns are filled with values from the ones matrix.
	template<size_t Amount = 1>
	auto Expand(const OnesMatrixTag&) const noexcept
	{
		constexpr static size_t Expanded = RowCount + Amount;

		Matrix<T, Expanded> result(*this);

		ForEach<RowCount>([&](size_t r)
		{
			for (size_t c = RowType::Size; c < Expanded; ++c)
				result.Values[(r * Expanded) + c] = T(1);
		});

		ForEach<Expanded * Amount>([&](size_t c)
		{
			result.Values[(Expanded * RowCount) + c] = T(1);
		});

		return result;
	}

	// Constructs a matrix from this matrix with 'Amount' additional rows and columns.
	// Added rows/columns are filled with values from the identity matrix.
	template<size_t Amount = 1>
	auto Expand(const IdentityMatrixTag&) const noexcept
	{
		constexpr static size_t Expanded = RowCount + Amount;

		Matrix<T, Expanded> result(*this);

		ForEach<RowCount>([&](size_t r)
		{
			for (size_t c = RowType::Size; c < Expanded; ++c)
				result.Values[(r * Expanded) + c] = T(0);
		});

		for (size_t r = RowCount; r < Expanded; ++r)
		{
			ForEach<Expanded>([&](size_t c)
			{
				result.Values[(Expanded * r) + c] = (r == c) ? T(1) : T(0);
			});
		}

		return result;
	}

public:
	// Returns 'matA' * 'matB'
	static inline Type CompositeOf(const Type& matA, const Type& matB) noexcept
	{
		Type result{ matA };
		return result.Compose(matB);
	}

	// Copies 'mat' and transposes it
	static inline Type TransposeOf(const Type& mat) noexcept
	{
		Type result{ mat };
		return result.Transpose();
	}
	
	// Copies 'mat' and inverts it under the assumption that it describes a rigid-body transformation.
	static inline Type RigidInverseOf(const Type& mat) noexcept
	{
		Type result{ mat };
		return result.InvertRigid();
	}

	// Copies 'mat' and inverts it
	static inline Type InverseOf(const Type& mat) noexcept
	{
		Type result{ mat };
		return result.Invert();
	}

	// Copies 'mat'. The copy is then inverted under the assumption that it describes a rigid-body transformation and then transposed.
	static inline Type TransposedRigidInverseOf(const Type& mat) noexcept
	{
		Type result{ mat };
		return result.TransposeInvertRigid();
	}

	// Copies 'mat'. The copy is then inverted and then transposed.
	static inline Type TransposedInverseOf(const Type& mat) noexcept
	{
		Type result{ mat };
		return result.TransposeInvert();
	}

public:
	// Copy this matrix with negated values
	inline Type operator - () const noexcept
	{
		Type result;
		ForEach<Size>([&](size_t n) { result[n] = -Values[n]; });
		return result;
	}

	// Copy this matrix inverted under the assumption that it describes a rigid-body transformation
	inline Type operator ~ () const noexcept
	{
		return Type::RigidInverseOf(*this);
	}

	// Implicit conversion to T if this Matrix contains only one value
	inline operator std::conditional_t<(Size == 1), T, struct OperationUnavailable>() const noexcept
	{
		return Values[0];
	}

public:
	#pragma region Assignment Operators
	
	// Set all values to zero
	inline Type& operator = (const ZeroesMatrixTag&) noexcept
	{
		ForEach<Size>([&](size_t n) { Values[n] = T(0); });
		return *this;
	}

	// Set all values to one
	inline Type& operator = (const OnesMatrixTag&) noexcept
	{
		ForEach<Size>([&](size_t n) { Values[n] = T(1); });
		return *this;
	}

	// Set this Matrix to an identity matrix (s.t. M * identity(M) = M)
	inline Type& operator = (const IdentityMatrixTag&) noexcept
	{
		ForEach<Size>([&](size_t n) { Values[n] = T(0); });
		ForEach<RowCount>([&](size_t n) { Values[RowCount*n + n] = T(1); });
		return *this;
	}

	// Compose this matrix with m
	inline Type& operator *= (const Type& m) noexcept
	{
		return Compose(m);
	}

	//////

	#define CREATE_SCALAR_ASSIGNMENT_OPERATOR(Op)											\
																							\
	inline Type& operator Op (const T value) noexcept										\
	{																						\
		ForEach<Size>([&](size_t index) { Values[index] Op value; });						\
		return *this;																		\
	}																						\
																							\
	inline Type& operator Op (std::initializer_list<T> values) noexcept						\
	{																						\
		auto it = std::begin(values);														\
																							\
		ForEach<Size>([&](size_t index)														\
		{																					\
			if (it != std::end(values))														\
				Values[index] Op *it++;														\
		});																					\
																							\
		return *this;																		\
	}
	
	CREATE_SCALAR_ASSIGNMENT_OPERATOR(= );
	CREATE_SCALAR_ASSIGNMENT_OPERATOR(+= );
	CREATE_SCALAR_ASSIGNMENT_OPERATOR(-= );
	CREATE_SCALAR_ASSIGNMENT_OPERATOR(*= );
	CREATE_SCALAR_ASSIGNMENT_OPERATOR(/= );

	// The following assignment operators will fail for non-integral types
	CREATE_SCALAR_ASSIGNMENT_OPERATOR(|= );
	CREATE_SCALAR_ASSIGNMENT_OPERATOR(&= );
	CREATE_SCALAR_ASSIGNMENT_OPERATOR(^= );
	CREATE_SCALAR_ASSIGNMENT_OPERATOR(%= );
	CREATE_SCALAR_ASSIGNMENT_OPERATOR(<<= );
	CREATE_SCALAR_ASSIGNMENT_OPERATOR(>>= );

	#undef CREATE_SCALAR_ASSIGNMENT_OPERATOR

	//////

	#define CREATE_MATRIX_ASSIGNMENT_OPERATOR(Op)											\
																							\
	template<class U>																		\
	inline Type& operator Op (const Matrix<U, S>& mat) noexcept								\
	{																						\
		ForEach<Size>([&](size_t index) { Values[index] Op T(mat.Values[index]); });		\
		return *this;																		\
	}

	CREATE_MATRIX_ASSIGNMENT_OPERATOR(= );
	CREATE_MATRIX_ASSIGNMENT_OPERATOR(+= );
	CREATE_MATRIX_ASSIGNMENT_OPERATOR(-= );

	// The following assignment operators will fail for non-integral types
	CREATE_MATRIX_ASSIGNMENT_OPERATOR(|= );
	CREATE_MATRIX_ASSIGNMENT_OPERATOR(&= );
	CREATE_MATRIX_ASSIGNMENT_OPERATOR(^= );
	CREATE_MATRIX_ASSIGNMENT_OPERATOR(%= );
	CREATE_MATRIX_ASSIGNMENT_OPERATOR(<<= );
	CREATE_MATRIX_ASSIGNMENT_OPERATOR(>>= );

	#undef CREATE_MATRIX_ASSIGNMENT_OPERATOR

	#pragma endregion

public:
	#pragma region Arithmetic Operators

	// Return the composite of this matrix and 'mat'
	inline Type operator * (const Type& mat) const noexcept
	{
		return Type::CompositeOf(*this, mat);
	}

	//////

	#define CREATE_SCALAR_ARITHMETIC_OPERATOR(Op) 													\
																									\
	inline Type operator Op (const T value) const noexcept											\
	{																								\
		Type result{ *this };																		\
		result Op= value;																			\
		return result;																				\
	}																								\
																									\
	friend inline Type operator Op (const T value, const Type& mat) noexcept						\
	{																								\
		Type result{ mat };																			\
		result Op= value;																			\
		return result;																				\
	}																								\
																									\
	inline Type operator Op (std::initializer_list<T> values) const	noexcept						\
	{																								\
		Type result{ *this };																		\
		result Op= values;																			\
		return result;																				\
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

	#define CREATE_MATRIX_ARITHMETIC_OPERATOR(Op)								\
																				\
	template<class U>															\
	inline Type operator Op (const Matrix<U, S>& mat) const	noexcept			\
	{																			\
		Type result{ *this };													\
		result Op= mat;															\
		return result;															\
	}																			\

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
	#pragma region Iteration Helpers

	template<size_t N, class Function, class... Args>
	inline void ForEach(Function fn, Args&&... args) noexcept
	{
		Epic::TMP::ForEach<Epic::TMP::MakeSequence<size_t, N>>::Apply(fn, std::forward<Args>(args)...);
	}

	template<size_t N, class Function, class... Args>
	inline void ForEach(Function fn, Args&&... args) const noexcept
	{
		Epic::TMP::ForEach<Epic::TMP::MakeSequence<size_t, N>>::Apply(fn, std::forward<Args>(args)...);
	}

	#pragma endregion

	#pragma region Construction Helpers

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

	#pragma endregion

	#pragma region Determinant Helpers

	template<size_t n>
	inline T DeterminantHelper() const noexcept
	{
		// Get the 1st row of the matrix of minors
		Vector<T, RowType::Size> minors = MinorsHelper();

		// Transform it into a cofactor vector
		for (size_t i = 1; i < RowType::Size; i += 2)
			minors[i] = -minors[i];

		// Calculate the determinant
		return minors.Dot(Rows[0]);
	}

	template<>
	inline T DeterminantHelper<0>() const noexcept
	{
		// Matrix<T, 0> Optimization
		return 0;
	}

	template<>
	inline T DeterminantHelper<1>() const noexcept
	{
		// Matrix<T, 1> Optimization
		return Values[0];
	}

	template<>
	inline T DeterminantHelper<2>() const noexcept
	{
		// Matrix<T, 2> Optimization - Determinant = M[0][0] * M[1][1] - M[1][0] * M[0][1]
		return (Values[0] * Values[3]) - (Values[1] * Values[2]);
	}

	template<>
	inline T DeterminantHelper<3>() const noexcept
	{
		// Matrix<T, 3> Optimization - Determinant is the additive sum of the 3 top-to-bottom crosses 
		//							   minus the additive sum of the 3 bottom-to-top crosses
		return (Values[0] * Values[4] * Values[8])
			 + (Values[1] * Values[5] * Values[6]) 
			 + (Values[2] * Values[3] * Values[7])
			 - (Values[0] * Values[5] * Values[7])
			 - (Values[1] * Values[3] * Values[8])
			 - (Values[2] * Values[4] * Values[6]);
	}

	inline auto MinorsHelper() const noexcept
	{
		Vector<T, RowType::Size> minors;
		Matrix<T, RowType::Size - 1> minor;

		ForEach<RowCount>([&](size_t c) 
		{
			// Construct the minor of [0, c]
			size_t d = 0;

			for (size_t i = 1; i < RowCount; ++i)
			{
				ForEach<RowCount>([&](size_t j) 
				{
					if (j != c)
						minor.Values[d++] = Values[(i * RowType::Size) + j];
				});
			}

			// Set this minors value to the minor's determinant
			minors[c] = minor.DeterminantHelper<RowCount - 1>();
		});

		return minors;
	}

	#pragma endregion

public:
	template<class U, size_t Sz>
	friend inline bool operator == (const Matrix<U, Sz>& matA, const Matrix<U, Sz>& matB) noexcept;

	template<class U, size_t Sz>
	friend inline bool operator != (const Matrix<U, Sz>& matA, const Matrix<U, Sz>& matB) noexcept;

	template<class U, size_t Sz>
	friend inline std::ostream& operator << (std::ostream& stream, const Matrix<U, Sz>& mat);

	template<class U, size_t Sz>
	friend inline std::wostream& operator << (std::wostream& stream, const Matrix<U, Sz>& mat);

	template<class U, size_t Sz>
	friend inline std::istream& operator >> (std::istream& stream, Matrix<U, Sz>& mat);

	template<class U, size_t Sz>
	friend inline std::wistream& operator >> (std::wistream& stream, Matrix<U, Sz>& mat);
};

//////////////////////////////////////////////////////////////////////////////

// Friend Operators
namespace Epic
{
	template<class U, size_t Sz>
	inline bool operator == (const Matrix<U, Sz>& matA, const Matrix<U, Sz>& matB) noexcept
	{
		bool result = true;

		for (size_t i = 0; i < Sz; ++i)
			result &= (matA[i] == matB[i]);

		return result;
	}

	template<class U, size_t Sz>
	inline bool operator != (const Matrix<U, Sz>& matA, const Matrix<U, Sz>& matB) noexcept
	{
		bool result = true;

		for (size_t i = 0; i < Sz; ++i)
			result &= (matA[i] != matB[i]);

		return result;
	}

	template<class U, size_t Sz>
	inline std::ostream& operator << (std::ostream& stream, const Matrix<U, Sz>& mat)
	{
		stream << '[' << std::endl;
		if (Sz > 0)
		{
			stream << std::fixed;
			mat.ForEach<Sz>([&](size_t n)
			{
				stream << ' ' << mat[n];
				if (n < Sz - 1) stream << ',';
				stream << std::endl;
			});
			stream << std::defaultfloat;
		}
		stream << ']';

		return stream;
	}

	template<class U, size_t Sz>
	inline std::wostream& operator << (std::wostream& stream, const Matrix<U, Sz>& mat)
	{
		stream << L'[' << std::endl;
		if (Sz > 0)
		{
			stream << std::fixed;
			mat.ForEach<Sz>([&](size_t n)
			{
				stream << L' ' << mat[n];
				if (n < Sz - 1) stream << L',';
				stream << std::endl;
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

		mat.ForEach<Sz>([&](size_t n)
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

		mat.ForEach<Sz>([&](size_t n)
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

// Vector * Matrix operators
namespace Epic
{
	template<class T, size_t S>
	inline Vector<T, S>& Epic::Vector<T, S>::operator *= (const Epic::Matrix<T, S>& mat) noexcept
	{
		mat.Transform(*this);
		return *this;
	}

	template<class T, size_t S>
	inline Vector<T, S>& Epic::Vector<T, S>::operator *= (const Epic::Matrix<T, S + 1>& mat) noexcept
	{
		mat.Transform(*this);
		return *this;
	}

	template<class T, size_t S>
	inline auto operator * (const Vector<T, S>& vec, const Matrix<T, S>& mat) noexcept
	{
		auto result = vec;
		mat.Transform(result);
		return result;
	}

	template<class T, size_t S>
	inline auto operator * (const Vector<T, S>& vec, const Matrix<T, S + 1>& mat) noexcept
	{
		auto result = vec;
		mat.Transform(result);
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
