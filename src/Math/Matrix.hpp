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
	Matrix() = default;
	Matrix(const Type&) = default;
	Matrix(Type&&) = default;

	Matrix(std::initializer_list<RowType> rows)
	{
		std::copy
		(
			std::begin(rows),
			std::next(std::begin(rows), std::min(rows.size(), RowCount)),
			std::begin(Rows)
		);

		if (rows.size() < RowCount)
			std::fill(std::next(std::begin(Rows), rows.size()), std::end(Rows), RowType{ 0 });
	}

	Matrix(std::initializer_list<ValueType> values)
	{
		std::copy
		(
			std::begin(values),
			std::next(std::begin(values), std::min(values.size(), Size)),
			std::begin(Values)
		);

		if (values.size() < Size)
			std::fill(std::next(std::begin(Values), values.size()), std::end(Values), ValueType(0));
	}

};
