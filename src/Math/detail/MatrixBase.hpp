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

#include <Epic/Math/Vector.hpp>
#include <array>

//////////////////////////////////////////////////////////////////////////////

namespace Epic::detail
{
	template<class T, size_t Size>
	class MatrixBase;
}

//////////////////////////////////////////////////////////////////////////////

// MatrixBase
template<class T, size_t S>
class Epic::detail::MatrixBase
{
public:
	using Type = Epic::detail::MatrixBase<T, S>;
	
	using ValueType = T;
	static constexpr size_t Size = S * S;
	
	using RowType = Epic::Vector<T, S>;
	static constexpr size_t RowCount = S;

	union
	{
		std::array<RowType, RowCount> Rows;
		std::array<ValueType, Size> Values;
	};
};

//////////////////////////////////////////////////////////////////////////////

// MatrixBase<1>
template<class T>
class Epic::detail::MatrixBase<T, 1>
{
public:
	using Type = Epic::detail::MatrixBase<T, 1>;

	using ValueType = T;
	static constexpr size_t Size = 1;

	using RowType = Epic::Vector<T, 1>;
	static constexpr size_t RowCount = 1;

	union
	{
		std::array<RowType, RowCount> Rows;
		std::array<ValueType, Size> Values;

		struct
		{
			RowType rx;
		};
	};
};

//////////////////////////////////////////////////////////////////////////////

// MatrixBase<2>
template<class T>
class Epic::detail::MatrixBase<T, 2>
{
public:
	using Type = Epic::detail::MatrixBase<T, 2>;

	using ValueType = T;
	static constexpr size_t Size = 2;

	using RowType = Epic::Vector<T, 2>;
	static constexpr size_t RowCount = 2;

	union
	{
		std::array<RowType, RowCount> Rows;
		std::array<ValueType, Size> Values;

		struct
		{
			RowType rx;
			RowType ry;
		};
	};
};

//////////////////////////////////////////////////////////////////////////////

// MatrixBase<3>
template<class T>
class Epic::detail::MatrixBase<T, 3>
{
public:
	using Type = Epic::detail::MatrixBase<T, 3>;

	using ValueType = T;
	static constexpr size_t Size = 3;

	using RowType = Epic::Vector<T, 3>;
	static constexpr size_t RowCount = 3;

	union
	{
		std::array<RowType, RowCount> Rows;
		std::array<ValueType, Size> Values;

		struct
		{
			RowType rx;
			RowType ry;
			RowType rz;
		};
	};
};

//////////////////////////////////////////////////////////////////////////////

// MatrixBase<4>
template<class T>
class Epic::detail::MatrixBase<T, 4>
{
public:
	using Type = Epic::detail::MatrixBase<T, 4>;

	using ValueType = T;
	static constexpr size_t Size = 4;

	using RowType = Epic::Vector<T, 4>;
	static constexpr size_t RowCount = 4;

	union
	{
		std::array<RowType, RowCount> Rows;
		std::array<ValueType, Size> Values;

		struct
		{
			RowType rx;
			RowType ry;
			RowType rz;
			RowType rw;
		};
	};
};
