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
	Matrix() noexcept = default;
	Matrix(const Type&) noexcept = default;
	Matrix(Type&&) noexcept = default;

	template<class U, size_t Sz>
	inline Matrix(const Matrix<U, Sz>& mat) noexcept
	{
		// These branches should be optimized away (TODO: constexpr if when available)
		if (Sz < RowCount)
		{
			// 'mat' is smaller than this matrix
			ForEach<Sz>([&](size_t r)
			{
				ForEach<Sz>([&](size_t c)
				{
					Values[(r * RowSize) + c] = T(mat.Values[(r * Sz) + c]);
				});
			});
		}
		else if (Sz > RowCount)
		{
			// 'mat' is larger than this matrix
			ForEach<RowCount>([&](size_t r) 
			{
				ForEach<RowType::Size>([&](size_t c) 
				{
					Values[(r * RowSize) + c] = T(mat.Values[(r * Sz) + c]);
				});
			});
		}
		else
		{
			// 'mat' is the same size as this matrix
			ForEach<Size>([&](size_t n) { Values[n] = mat[n]; });
		}
	}

	inline Matrix(std::initializer_list<RowType> rows) noexcept
	{
		std::copy
		(
			std::begin(rows),
			std::next(std::begin(rows), std::min(rows.size(), RowCount)),
			std::begin(Rows)
		);

		if (rows.size() < RowCount)
			std::fill(std::next(std::begin(Rows), rows.size()), std::end(Rows), T(0));
	}

	inline Matrix(std::initializer_list<ValueType> values) noexcept
	{
		std::copy
		(
			std::begin(values),
			std::next(std::begin(values), std::min(values.size(), Size)),
			std::begin(Values)
		);

		if (values.size() < Size)
			std::fill(std::next(std::begin(Values), values.size()), std::end(Values), T(0));
	}

	template<class Arg, class... Args, 
		typename = std::enable_if_t<(detail::Span<Arg, Args...>::Value == Size)>>
	inline Matrix(Arg&& arg, Args&&... args) noexcept
	{
		Construct(std::forward<Arg>(arg), std::forward<Args>(args)...);
	}

	inline Matrix(const OnesMatrixTag&) noexcept
	{
		ForEach<Size>([&](size_t n) { Values[n] = T(1); });
	}

	inline Matrix(const ZeroesMatrixTag&) noexcept
	{
		ForEach<Size>([&](size_t n) { Values[n] = T(0); });
	}

	inline Matrix(const IdentityMatrixTag&) noexcept
	{
		size_t i = 0;

		ForEach<Size>([&](size_t n)
		{
			if (n == i)
			{
				Values[n] = T(1);
				i += RowType::Size + 1;
			}
			else
				Values[n] = T(0);
		});
	}

	// Translate/Scale/Rotate
	// LookAt
	// Frustum/Perspective/Ortho/Ortho2D/Picking Constructors
	// Shear/Shadow Constructors

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
	void Transform(Vector<T, Size>& vec) const noexcept
	{
		const decltype(vec) cache{ vec };
		
		// These branches should be optimized away (TODO: constexpr if when available)
		if (RowType::Size == 4)
		{
			vec[0] = (cache[0] * Values[0]) + (cache[1] * Values[4]) + (cache[2] * Values[8]);
			vec[1] = (cache[0] * Values[1]) + (cache[1] * Values[5]) + (cache[2] * Values[9]);
			vec[2] = (cache[0] * Values[2]) + (cache[1] * Values[6]) + (cache[2] * Values[10]);
			vec[3] = (cache[0] * Values[3]) + (cache[1] * Values[7]) + (cache[2] * Values[11]);
		}
		else if (RowType::Size == 3)
		{
			vec[0] = (cache[0] * Values[0]) + (cache[1] * Values[3]) + (cache[2] * Values[6]);
			vec[1] = (cache[0] * Values[1]) + (cache[1] * Values[4]) + (cache[2] * Values[7]);
			vec[2] = (cache[0] * Values[2]) + (cache[1] * Values[5]) + (cache[2] * Values[8]);
		}
		else if (RowType::Size == 2)
		{
			vec[0] = (cache[0] * Values[0]) + (cache[1] * Values[2]);
			vec[1] = (cache[0] * Values[1]) + (cache[1] * Values[3]);
		}
		else if (RowType::Size == 1)
		{
			vec[0] *= Values[0];
		}
		else
		{
			ForEach<RowCount>([&](size_t c) 
			{
				vec[c] = cache[0] * Values[c];

				for (size_t j = 1; j < RowCount; ++j)
					vec[c] += cache[j] * Values[(j * RowType::Size) + c];
			});
		}
	}

public:
	// Calculates the sum of the main diagonal values
	inline T Trace() const noexcept
	{
		T result = T(0);
		
		for (size_t i = 0; i < Size; i += (RowType::Size + 1))
			result += Values[i];

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
			ForEach<RowType::Size>([&](size_t c) 
			{
				const size_t vi = (r * RowType::Size) + c;
				result[vi] = T(0);

				ForEach<RowType::Size>([&](size_t v) 
				{
					result[vi] += Values[(r * RowType::Size) + v] * mat.Values[(v * RowType::Size) + c];
				});
			});
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
	Type& InvertFast() noexcept
	{
		ForEach<RowCount - 1>([&](size_t i) 
		{
			Rows[i][RowType::Size - 1] = -Rows[i].Dot(Rows[RowCount - 1]);
		});

		Rows[RowCount - 1] = T(0);
		Rows[RowCount - 1][RowType::Size - 1] = T(1);

		return Transpose();
	}

	// Inverts this matrix. (M * inverse(M) = identity(M))
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
			Type adj;

			adj.Values[0] =  ((Values[4] * Values[8]) - (Values[5] * Values[7]));
			adj.Values[1] = -((Values[1] * Values[8]) - (Values[2] * Values[7]));
			adj.Values[2] =  ((Values[1] * Values[5]) - (Values[2] * Values[4]));
			adj.Values[3] = -((Values[3] * Values[8]) - (Values[5] * Values[6]));
			adj.Values[4] =  ((Values[0] * Values[8]) - (Values[2] * Values[6]));
			adj.Values[5] = -((Values[0] * Values[5]) - (Values[2] * Values[3]));
			adj.Values[6] =  ((Values[3] * Values[7]) - (Values[4] * Values[6]));
			adj.Values[7] = -((Values[0] * Values[7]) - (Values[1] * Values[6]));
			adj.Values[8] =  ((Values[0] * Values[4]) - (Values[1] * Values[3]));

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
	inline Type& TransposeFastInvert() noexcept
	{
		InvertFast();
		return Transpose();
	}

	// Inverts this matrix; then transposes it.
	inline Type& TransposeInvert() noexcept
	{
		Invert();
		return Transpose();
	}

	template<size_t I = 0, size_t N = RowCount,
			 typename InvalidSliceParams = std::enable_if_t<((I + N) <= RowCount)>>
	Matrix<T, N> Slice() const noexcept
	{
		Matrix<T, N> result;
		size_t src = (RowCount * I) + I;
		size_t dest = 0;

		for (size_t i = 0; i < N; ++i)
		{
			for (size_t j = 0; j < N; ++j)
				result.Values[dest++] = Values[src++];

			src += RowCount - N;
		}

		return result;
	}

	template<size_t Amount = 1, typename ResultSizeMustBeGreaterThan0 = std::enable_if_t<(RowCount > Amount)>>
	auto Contract() const noexcept
	{
		return Slice<0, RowCount - Amount>();
	}

	template<size_t Amount = 1>
	auto Expand() const noexcept
	{
		return Expand<Amount>(IdentityMatrix);
	}

	template<size_t Amount = 1>
	auto Expand(const ZeroesMatrixTag&) const
	{
		constexpr static size_t Expanded = RowCount + Amount;

		Matrix<T, Expanded> result(*this);

		for (size_t r = 0; r < RowCount; ++r)
			for (size_t c = RowCount; c < Expanded; ++c)
				result.Values[(r * Expanded) + c] = T(0);

		for (size_t c = 0; c < Expanded * Amount; ++c)
			result.Values[(Expanded * RowCount) + c] = T(0);

		return result;
	}

	template<size_t Amount = 1>
	auto Expand(const OnesMatrixTag&) const
	{
		constexpr static size_t Expanded = RowCount + Amount;

		Matrix<T, Expanded> result(*this);

		for (size_t r = 0; r < RowCount; ++r)
			for (size_t c = RowCount; c < Expanded; ++c)
				result.Values[(r * Expanded) + c] = T(1);

		for (size_t c = 0; c < Expanded * Amount; ++c)
			result.Values[(Expanded * RowCount) + c] = T(1);

		return result;
	}

	template<size_t Amount = 1>
	auto Expand(const IdentityMatrixTag&) const
	{
		constexpr static size_t Expanded = RowCount + Amount;

		Matrix<T, Expanded> result(*this);

		for (size_t r = 0; r < RowCount; ++r)
			for (size_t c = RowCount; c < Expanded; ++c)
				result.Values[(r * Expanded) + c] = T(0);

		for (size_t r = RowCount; r < Expanded; ++r)
			for (size_t c = 0; c < Expanded; ++c)
				result.Values[(Expanded * r) + c] = (r == c) ? T(1) : T(0);

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
	static inline Type FastInverseOf(const Type& mat) noexcept
	{
		Type result{ mat };
		return result.InvertFast();
	}

	// Copies 'mat' and inverts it
	static inline Type InverseOf(const Type& mat) noexcept
	{
		Type result{ mat };
		return result.Invert();
	}

	// Copies 'mat'. The copy is then inverted under the assumption that it describes a rigid-body transformation and then transposed.
	static inline Type FastTransposedInverseOf(const Type& mat) noexcept
	{
		Type result{ mat };
		return result.TransposeFastInvert();
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

	// Copy this matrix inverted (assumes it describes a rigid-body transformation)
	inline Type operator ~ () const noexcept
	{
		return Type::FastInverseOf(*this);
	}

	// Implicit conversion to T if this Matrix contains only one value
	inline operator std::conditional_t<(Size == 1), T, struct OperationUnavailable>() const noexcept
	{
		return Values[0];
	}

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
	friend bool operator == (const Matrix<U, Sz>& matA, const Matrix<U, Sz>& matB) noexcept;

	template<class U, size_t Sz>
	friend bool operator != (const Matrix<U, Sz>& matA, const Matrix<U, Sz>& matB) noexcept;

	template<class U, size_t Sz>
	friend std::ostream& operator << (std::ostream& stream, const Matrix<U, Sz>& mat);

	template<class U, size_t Sz>
	friend std::wostream& operator << (std::wostream& stream, const Matrix<U, Sz>& mat);

	template<class U, size_t Sz>
	friend std::istream& operator >> (std::istream& stream, Matrix<U, Sz>& mat);

	template<class U, size_t Sz>
	friend std::wistream& operator >> (std::wistream& stream, Matrix<U, Sz>& mat);
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
