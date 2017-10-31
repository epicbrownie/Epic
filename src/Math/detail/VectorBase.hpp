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

//////////////////////////////////////////////////////////////////////////////

namespace Epic::detail
{
	template<std::size_t Size, template<std::size_t...> class SwizzlerGenerator, class TArray>
	class VectorBase;

	template<std::size_t Size, template<std::size_t...> class SwizzlerGenerator, class TArray>
	class SVectorBase;
}

//////////////////////////////////////////////////////////////////////////////

// VectorBase
template<std::size_t Size, template<std::size_t...> class SwizzlerGenerator, class TArray>
class Epic::detail::VectorBase
{
public:
	// Value Array
	TArray Values;
};

// VectorBase<1>
template<template<std::size_t...> class SwizzlerGenerator, class TArray>
class Epic::detail::VectorBase<1, SwizzlerGenerator, TArray>
{
public:
	union
	{
		// Value Array
		TArray Values;

		// 1-Component Swizzlers
		struct
		{
			typename SwizzlerGenerator<0>::Type x;
		};

		// 2-Component Swizzlers
		typename SwizzlerGenerator<0, 0>::Type xx;

		// 3-Component Swizzlers
		typename SwizzlerGenerator<0, 0, 0>::Type xxx;

		// 4-Component Swizzlers
		typename SwizzlerGenerator<0, 0, 0, 0>::Type xxxx;
	};
};

// VectorBase<2>
template<template<std::size_t...> class SwizzlerGenerator, class TArray>
class Epic::detail::VectorBase<2, SwizzlerGenerator, TArray>
{
public:
	union
	{
		// Value Array
		TArray Values;

		// 1-Component Swizzlers
		struct
		{
			typename SwizzlerGenerator<0>::Type x;
			typename SwizzlerGenerator<1>::Type y;
		};

		// 2-Component Swizzlers
		typename SwizzlerGenerator<0, 0>::Type xx;
		typename SwizzlerGenerator<1, 1>::Type yy;
		typename SwizzlerGenerator<0, 1>::Type xy;
		typename SwizzlerGenerator<1, 0>::Type yx;

		// 3-Component Swizzlers
		typename SwizzlerGenerator<0, 0, 0>::Type xxx;
		typename SwizzlerGenerator<1, 1, 1>::Type yyy;

		// 4-Component Swizzlers
		typename SwizzlerGenerator<0, 0, 0, 0>::Type xxxx;
		typename SwizzlerGenerator<1, 1, 1, 1>::Type yyyy;
		typename SwizzlerGenerator<0, 1, 0, 1>::Type xyxy;
		typename SwizzlerGenerator<1, 0, 1, 0>::Type yxyx;
	};
};

// VectorBase<3>
template<template<std::size_t...> class SwizzlerGenerator, class TArray>
class Epic::detail::VectorBase<3, SwizzlerGenerator, TArray>
{
public:
	union
	{
		// Value Array
		TArray Values;

		// 1-Component Swizzlers
		struct
		{
			typename SwizzlerGenerator<0>::Type x;
			typename SwizzlerGenerator<1>::Type y;
			typename SwizzlerGenerator<2>::Type z;
		};

		// 2-Component Swizzlers
		typename SwizzlerGenerator<0, 0>::Type xx;
		typename SwizzlerGenerator<1, 1>::Type yy;
		typename SwizzlerGenerator<2, 2>::Type zz;
		typename SwizzlerGenerator<0, 1>::Type xy;
		typename SwizzlerGenerator<1, 0>::Type yx;
		
		// 3-Component Swizzlers
		typename SwizzlerGenerator<0, 0, 0>::Type xxx;
		typename SwizzlerGenerator<1, 1, 1>::Type yyy;
		typename SwizzlerGenerator<2, 2, 2>::Type zzz;
		typename SwizzlerGenerator<0, 1, 2>::Type xyz;
		typename SwizzlerGenerator<0, 2, 1>::Type xzy;
		typename SwizzlerGenerator<1, 0, 2>::Type yxz;
		typename SwizzlerGenerator<1, 2, 0>::Type yzx;
		typename SwizzlerGenerator<2, 0, 1>::Type zxy;
		typename SwizzlerGenerator<2, 1, 0>::Type zyx;

		// 4-Component Swizzlers
		typename SwizzlerGenerator<0, 0, 0, 0>::Type xxxx;
		typename SwizzlerGenerator<0, 0, 0, 0>::Type yyyy;
		typename SwizzlerGenerator<0, 0, 0, 0>::Type zzzz;
	};
};

// VectorBase<4>
template<template<std::size_t...> class SwizzlerGenerator, class TArray>
class Epic::detail::VectorBase<4, SwizzlerGenerator, TArray>
{
public:
	union
	{
		// Value Array
		TArray Values;

		// 1-Component Swizzlers
		struct
		{
			typename SwizzlerGenerator<0>::Type x;
			typename SwizzlerGenerator<1>::Type y;
			typename SwizzlerGenerator<2>::Type z;
			typename SwizzlerGenerator<3>::Type w;
		};

		// 2-Component Swizzlers
		typename SwizzlerGenerator<0, 0>::Type xx;
		typename SwizzlerGenerator<1, 1>::Type yy;
		typename SwizzlerGenerator<2, 2>::Type zz;
		typename SwizzlerGenerator<3, 3>::Type ww;
		typename SwizzlerGenerator<0, 1>::Type xy;
		typename SwizzlerGenerator<1, 0>::Type yx;
		
		// 3-Component Swizzlers
		typename SwizzlerGenerator<0, 0, 0>::Type xxx;
		typename SwizzlerGenerator<1, 1, 1>::Type yyy;
		typename SwizzlerGenerator<2, 2, 2>::Type zzz;
		typename SwizzlerGenerator<3, 3, 3>::Type www;
		typename SwizzlerGenerator<0, 1, 2>::Type xyz;
		typename SwizzlerGenerator<0, 2, 1>::Type xzy;
		typename SwizzlerGenerator<1, 0, 2>::Type yxz;
		typename SwizzlerGenerator<1, 2, 0>::Type yzx;
		typename SwizzlerGenerator<2, 0, 1>::Type zxy;
		typename SwizzlerGenerator<2, 1, 0>::Type zyx;


		// 4-Component Swizzlers
		typename SwizzlerGenerator<0, 0, 0, 0>::Type xxxx;
		typename SwizzlerGenerator<1, 1, 1, 1>::Type yyyy;
		typename SwizzlerGenerator<2, 2, 2, 2>::Type zzzz;
		typename SwizzlerGenerator<3, 3, 3, 3>::Type wwww;
		typename SwizzlerGenerator<0, 1, 2, 3>::Type xyzw;
		typename SwizzlerGenerator<0, 2, 1, 3>::Type xzyw;
		typename SwizzlerGenerator<1, 0, 2, 3>::Type yxzw;
		typename SwizzlerGenerator<1, 2, 0, 3>::Type yzxw;
		typename SwizzlerGenerator<2, 0, 1, 3>::Type zxyw;
		typename SwizzlerGenerator<2, 1, 0, 3>::Type zyxw;
		typename SwizzlerGenerator<3, 0, 1, 2>::Type wxyz;
		typename SwizzlerGenerator<3, 0, 2, 1>::Type wxzy;
		typename SwizzlerGenerator<3, 1, 0, 2>::Type wyxz;
		typename SwizzlerGenerator<3, 1, 2, 0>::Type wyzx;
		typename SwizzlerGenerator<3, 2, 0, 1>::Type wzxy;
		typename SwizzlerGenerator<3, 2, 1, 0>::Type wzyx;
	};
};

//////////////////////////////////////////////////////////////////////////////

// SVectorBase
template<std::size_t Size, template<std::size_t...> class SwizzlerGenerator, class TArray>
class Epic::detail::SVectorBase
{
public:
	// Value Array
	TArray Values;
};

// SVectorBase<1>
template<template<std::size_t...> class SwizzlerGenerator, class TArray>
class Epic::detail::SVectorBase<1, SwizzlerGenerator, TArray>
{
public:
	union
	{
		// Value Array
		TArray Values;

		// 1-Component Swizzlers
		struct
		{
			typename SwizzlerGenerator<0>::Type x;
		};

		// 2-Component Swizzlers
		typename SwizzlerGenerator<0, 0>::Type xx;

		// 3-Component Swizzlers
		typename SwizzlerGenerator<0, 0, 0>::Type xxx;

		// 4-Component Swizzlers
		typename SwizzlerGenerator<0, 0, 0, 0>::Type xxxx;
	};
};

// SVectorBase<2>
template<template<std::size_t...> class SwizzlerGenerator, class TArray>
class Epic::detail::SVectorBase<2, SwizzlerGenerator, TArray>
{
public:
	union
	{
		// Value Array
		TArray Values;

		// 1-Component Swizzlers
		struct
		{
			typename SwizzlerGenerator<0>::Type x;
			typename SwizzlerGenerator<1>::Type y;
		};

		// 2-Component Swizzlers
		typename SwizzlerGenerator<0, 0>::Type xx;
		typename SwizzlerGenerator<0, 1>::Type xy;
		typename SwizzlerGenerator<1, 0>::Type yx;
		typename SwizzlerGenerator<1, 1>::Type yy;

		// 3-Component Swizzlers
		typename SwizzlerGenerator<0, 0, 0>::Type xxx;
		typename SwizzlerGenerator<0, 0, 1>::Type xxy;
		typename SwizzlerGenerator<0, 1, 0>::Type xyx;
		typename SwizzlerGenerator<0, 1, 1>::Type xyy;
		typename SwizzlerGenerator<1, 0, 0>::Type yxx;
		typename SwizzlerGenerator<1, 0, 1>::Type yxy;
		typename SwizzlerGenerator<1, 1, 0>::Type yyx;
		typename SwizzlerGenerator<1, 1, 1>::Type yyy;

		// 4-Component Swizzlers
		typename SwizzlerGenerator<0, 0, 0, 0>::Type xxxx;
		typename SwizzlerGenerator<0, 0, 0, 1>::Type xxxy;
		typename SwizzlerGenerator<0, 0, 1, 0>::Type xxyx;
		typename SwizzlerGenerator<0, 0, 1, 1>::Type xxyy;
		typename SwizzlerGenerator<0, 1, 0, 0>::Type xyxx;
		typename SwizzlerGenerator<0, 1, 0, 1>::Type xyxy;
		typename SwizzlerGenerator<0, 1, 1, 0>::Type xyyx;
		typename SwizzlerGenerator<0, 1, 1, 1>::Type xyyy;
		typename SwizzlerGenerator<1, 0, 0, 0>::Type yxxx;
		typename SwizzlerGenerator<1, 0, 0, 1>::Type yxxy;
		typename SwizzlerGenerator<1, 0, 1, 0>::Type yxyx;
		typename SwizzlerGenerator<1, 0, 1, 1>::Type yxyy;
		typename SwizzlerGenerator<1, 1, 0, 0>::Type yyxx;
		typename SwizzlerGenerator<1, 1, 0, 1>::Type yyxy;
		typename SwizzlerGenerator<1, 1, 1, 0>::Type yyyx;
		typename SwizzlerGenerator<1, 1, 1, 1>::Type yyyy;
	};
};

// SVectorBase<3>
template<template<std::size_t...> class SwizzlerGenerator, class TArray>
class Epic::detail::SVectorBase<3, SwizzlerGenerator, TArray>
{
public:
	union
	{
		// Value Array
		TArray Values;

		// 1-Component Swizzlers
		struct
		{
			typename SwizzlerGenerator<0>::Type x;
			typename SwizzlerGenerator<1>::Type y;
			typename SwizzlerGenerator<2>::Type z;
		};

		// 2-Component Swizzlers
		typename SwizzlerGenerator<0, 0>::Type xx;
		typename SwizzlerGenerator<0, 1>::Type xy;
		typename SwizzlerGenerator<0, 2>::Type xz;
		typename SwizzlerGenerator<1, 0>::Type yx;
		typename SwizzlerGenerator<1, 1>::Type yy;
		typename SwizzlerGenerator<1, 2>::Type yz;
		typename SwizzlerGenerator<2, 0>::Type zx;
		typename SwizzlerGenerator<2, 1>::Type zy;
		typename SwizzlerGenerator<2, 2>::Type zz;

		// 3-Component Swizzlers
		typename SwizzlerGenerator<0, 0, 0>::Type xxx;
		typename SwizzlerGenerator<0, 0, 1>::Type xxy;
		typename SwizzlerGenerator<0, 0, 2>::Type xxz;
		typename SwizzlerGenerator<0, 1, 0>::Type xyx;
		typename SwizzlerGenerator<0, 1, 1>::Type xyy;
		typename SwizzlerGenerator<0, 1, 2>::Type xyz;
		typename SwizzlerGenerator<0, 2, 0>::Type xzx;
		typename SwizzlerGenerator<0, 2, 1>::Type xzy;
		typename SwizzlerGenerator<0, 2, 2>::Type xzz;
		typename SwizzlerGenerator<1, 0, 0>::Type yxx;
		typename SwizzlerGenerator<1, 0, 1>::Type yxy;
		typename SwizzlerGenerator<1, 0, 2>::Type yxz;
		typename SwizzlerGenerator<1, 1, 0>::Type yyx;
		typename SwizzlerGenerator<1, 1, 1>::Type yyy;
		typename SwizzlerGenerator<1, 1, 2>::Type yyz;
		typename SwizzlerGenerator<1, 2, 0>::Type yzx;
		typename SwizzlerGenerator<1, 2, 1>::Type yzy;
		typename SwizzlerGenerator<1, 2, 2>::Type yzz;
		typename SwizzlerGenerator<2, 0, 0>::Type zxx;
		typename SwizzlerGenerator<2, 0, 1>::Type zxy;
		typename SwizzlerGenerator<2, 0, 2>::Type zxz;
		typename SwizzlerGenerator<2, 1, 0>::Type zyx;
		typename SwizzlerGenerator<2, 1, 1>::Type zyy;
		typename SwizzlerGenerator<2, 1, 2>::Type zyz;
		typename SwizzlerGenerator<2, 2, 0>::Type zzx;
		typename SwizzlerGenerator<2, 2, 1>::Type zzy;
		typename SwizzlerGenerator<2, 2, 2>::Type zzz;

		// 4-Component Swizzlers
		typename SwizzlerGenerator<0, 0, 0, 0>::Type xxxx;
		typename SwizzlerGenerator<0, 0, 0, 1>::Type xxxy;
		typename SwizzlerGenerator<0, 0, 0, 2>::Type xxxz;
		typename SwizzlerGenerator<0, 0, 1, 0>::Type xxyx;
		typename SwizzlerGenerator<0, 0, 1, 1>::Type xxyy;
		typename SwizzlerGenerator<0, 0, 1, 2>::Type xxyz;
		typename SwizzlerGenerator<0, 0, 2, 0>::Type xxzx;
		typename SwizzlerGenerator<0, 0, 2, 1>::Type xxzy;
		typename SwizzlerGenerator<0, 0, 2, 2>::Type xxzz;
		typename SwizzlerGenerator<0, 1, 0, 0>::Type xyxx;
		typename SwizzlerGenerator<0, 1, 0, 1>::Type xyxy;
		typename SwizzlerGenerator<0, 1, 0, 2>::Type xyxz;
		typename SwizzlerGenerator<0, 1, 1, 0>::Type xyyx;
		typename SwizzlerGenerator<0, 1, 1, 1>::Type xyyy;
		typename SwizzlerGenerator<0, 1, 1, 2>::Type xyyz;
		typename SwizzlerGenerator<0, 1, 2, 0>::Type xyzx;
		typename SwizzlerGenerator<0, 1, 2, 1>::Type xyzy;
		typename SwizzlerGenerator<0, 1, 2, 2>::Type xyzz;
		typename SwizzlerGenerator<0, 2, 0, 0>::Type xzxx;
		typename SwizzlerGenerator<0, 2, 0, 1>::Type xzxy;
		typename SwizzlerGenerator<0, 2, 0, 2>::Type xzxz;
		typename SwizzlerGenerator<0, 2, 1, 0>::Type xzyx;
		typename SwizzlerGenerator<0, 2, 1, 1>::Type xzyy;
		typename SwizzlerGenerator<0, 2, 1, 2>::Type xzyz;
		typename SwizzlerGenerator<0, 2, 2, 0>::Type xzzx;
		typename SwizzlerGenerator<0, 2, 2, 1>::Type xzzy;
		typename SwizzlerGenerator<0, 2, 2, 2>::Type xzzz;
		typename SwizzlerGenerator<1, 0, 0, 0>::Type yxxx;
		typename SwizzlerGenerator<1, 0, 0, 1>::Type yxxy;
		typename SwizzlerGenerator<1, 0, 0, 2>::Type yxxz;
		typename SwizzlerGenerator<1, 0, 1, 0>::Type yxyx;
		typename SwizzlerGenerator<1, 0, 1, 1>::Type yxyy;
		typename SwizzlerGenerator<1, 0, 1, 2>::Type yxyz;
		typename SwizzlerGenerator<1, 0, 2, 0>::Type yxzx;
		typename SwizzlerGenerator<1, 0, 2, 1>::Type yxzy;
		typename SwizzlerGenerator<1, 0, 2, 2>::Type yxzz;
		typename SwizzlerGenerator<1, 1, 0, 0>::Type yyxx;
		typename SwizzlerGenerator<1, 1, 0, 1>::Type yyxy;
		typename SwizzlerGenerator<1, 1, 0, 2>::Type yyxz;
		typename SwizzlerGenerator<1, 1, 1, 0>::Type yyyx;
		typename SwizzlerGenerator<1, 1, 1, 1>::Type yyyy;
		typename SwizzlerGenerator<1, 1, 1, 2>::Type yyyz;
		typename SwizzlerGenerator<1, 1, 2, 0>::Type yyzx;
		typename SwizzlerGenerator<1, 1, 2, 1>::Type yyzy;
		typename SwizzlerGenerator<1, 1, 2, 2>::Type yyzz;
		typename SwizzlerGenerator<1, 2, 0, 0>::Type yzxx;
		typename SwizzlerGenerator<1, 2, 0, 1>::Type yzxy;
		typename SwizzlerGenerator<1, 2, 0, 2>::Type yzxz;
		typename SwizzlerGenerator<1, 2, 1, 0>::Type yzyx;
		typename SwizzlerGenerator<1, 2, 1, 1>::Type yzyy;
		typename SwizzlerGenerator<1, 2, 1, 2>::Type yzyz;
		typename SwizzlerGenerator<1, 2, 2, 0>::Type yzzx;
		typename SwizzlerGenerator<1, 2, 2, 1>::Type yzzy;
		typename SwizzlerGenerator<1, 2, 2, 2>::Type yzzz;
		typename SwizzlerGenerator<2, 0, 0, 0>::Type zxxx;
		typename SwizzlerGenerator<2, 0, 0, 1>::Type zxxy;
		typename SwizzlerGenerator<2, 0, 0, 2>::Type zxxz;
		typename SwizzlerGenerator<2, 0, 1, 0>::Type zxyx;
		typename SwizzlerGenerator<2, 0, 1, 1>::Type zxyy;
		typename SwizzlerGenerator<2, 0, 1, 2>::Type zxyz;
		typename SwizzlerGenerator<2, 0, 2, 0>::Type zxzx;
		typename SwizzlerGenerator<2, 0, 2, 1>::Type zxzy;
		typename SwizzlerGenerator<2, 0, 2, 2>::Type zxzz;
		typename SwizzlerGenerator<2, 1, 0, 0>::Type zyxx;
		typename SwizzlerGenerator<2, 1, 0, 1>::Type zyxy;
		typename SwizzlerGenerator<2, 1, 0, 2>::Type zyxz;
		typename SwizzlerGenerator<2, 1, 1, 0>::Type zyyx;
		typename SwizzlerGenerator<2, 1, 1, 1>::Type zyyy;
		typename SwizzlerGenerator<2, 1, 1, 2>::Type zyyz;
		typename SwizzlerGenerator<2, 1, 2, 0>::Type zyzx;
		typename SwizzlerGenerator<2, 1, 2, 1>::Type zyzy;
		typename SwizzlerGenerator<2, 1, 2, 2>::Type zyzz;
		typename SwizzlerGenerator<2, 2, 0, 0>::Type zzxx;
		typename SwizzlerGenerator<2, 2, 0, 1>::Type zzxy;
		typename SwizzlerGenerator<2, 2, 0, 2>::Type zzxz;
		typename SwizzlerGenerator<2, 2, 1, 0>::Type zzyx;
		typename SwizzlerGenerator<2, 2, 1, 1>::Type zzyy;
		typename SwizzlerGenerator<2, 2, 1, 2>::Type zzyz;
		typename SwizzlerGenerator<2, 2, 2, 0>::Type zzzx;
		typename SwizzlerGenerator<2, 2, 2, 1>::Type zzzy;
		typename SwizzlerGenerator<2, 2, 2, 2>::Type zzzz;
	};
};

// SVectorBase<4>
template<template<std::size_t...> class SwizzlerGenerator, class TArray>
class Epic::detail::SVectorBase<4, SwizzlerGenerator, TArray>
{
public:
	union
	{
		// Value Array
		TArray Values;

		// 1-Component Swizzlers
		struct
		{
			typename SwizzlerGenerator<0>::Type x;
			typename SwizzlerGenerator<1>::Type y;
			typename SwizzlerGenerator<2>::Type z;
			typename SwizzlerGenerator<3>::Type w;
		};

		// 2-Component Swizzlers
		typename SwizzlerGenerator<0, 0>::Type xx;
		typename SwizzlerGenerator<0, 1>::Type xy;
		typename SwizzlerGenerator<0, 2>::Type xz;
		typename SwizzlerGenerator<0, 3>::Type xw;
		typename SwizzlerGenerator<1, 0>::Type yx;
		typename SwizzlerGenerator<1, 1>::Type yy;
		typename SwizzlerGenerator<1, 2>::Type yz;
		typename SwizzlerGenerator<1, 3>::Type yw;
		typename SwizzlerGenerator<2, 0>::Type zx;
		typename SwizzlerGenerator<2, 1>::Type zy;
		typename SwizzlerGenerator<2, 2>::Type zz;
		typename SwizzlerGenerator<2, 3>::Type zw;
		typename SwizzlerGenerator<3, 0>::Type wx;
		typename SwizzlerGenerator<3, 1>::Type wy;
		typename SwizzlerGenerator<3, 2>::Type wz;
		typename SwizzlerGenerator<3, 3>::Type ww;

		// 3-Component Swizzlers
		typename SwizzlerGenerator<0, 0, 0>::Type xxx;
		typename SwizzlerGenerator<0, 0, 1>::Type xxy;
		typename SwizzlerGenerator<0, 0, 2>::Type xxz;
		typename SwizzlerGenerator<0, 0, 3>::Type xxw;
		typename SwizzlerGenerator<0, 1, 0>::Type xyx;
		typename SwizzlerGenerator<0, 1, 1>::Type xyy;
		typename SwizzlerGenerator<0, 1, 2>::Type xyz;
		typename SwizzlerGenerator<0, 1, 3>::Type xyw;
		typename SwizzlerGenerator<0, 2, 0>::Type xzx;
		typename SwizzlerGenerator<0, 2, 1>::Type xzy;
		typename SwizzlerGenerator<0, 2, 2>::Type xzz;
		typename SwizzlerGenerator<0, 2, 3>::Type xzw;
		typename SwizzlerGenerator<0, 3, 0>::Type xwx;
		typename SwizzlerGenerator<0, 3, 1>::Type xwy;
		typename SwizzlerGenerator<0, 3, 2>::Type xwz;
		typename SwizzlerGenerator<0, 3, 3>::Type xww;
		typename SwizzlerGenerator<1, 0, 0>::Type yxx;
		typename SwizzlerGenerator<1, 0, 1>::Type yxy;
		typename SwizzlerGenerator<1, 0, 2>::Type yxz;
		typename SwizzlerGenerator<1, 0, 3>::Type yxw;
		typename SwizzlerGenerator<1, 1, 0>::Type yyx;
		typename SwizzlerGenerator<1, 1, 1>::Type yyy;
		typename SwizzlerGenerator<1, 1, 2>::Type yyz;
		typename SwizzlerGenerator<1, 1, 3>::Type yyw;
		typename SwizzlerGenerator<1, 2, 0>::Type yzx;
		typename SwizzlerGenerator<1, 2, 1>::Type yzy;
		typename SwizzlerGenerator<1, 2, 2>::Type yzz;
		typename SwizzlerGenerator<1, 2, 3>::Type yzw;
		typename SwizzlerGenerator<1, 3, 0>::Type ywx;
		typename SwizzlerGenerator<1, 3, 1>::Type ywy;
		typename SwizzlerGenerator<1, 3, 2>::Type ywz;
		typename SwizzlerGenerator<1, 3, 3>::Type yww;
		typename SwizzlerGenerator<2, 0, 0>::Type zxx;
		typename SwizzlerGenerator<2, 0, 1>::Type zxy;
		typename SwizzlerGenerator<2, 0, 2>::Type zxz;
		typename SwizzlerGenerator<2, 0, 3>::Type zxw;
		typename SwizzlerGenerator<2, 1, 0>::Type zyx;
		typename SwizzlerGenerator<2, 1, 1>::Type zyy;
		typename SwizzlerGenerator<2, 1, 2>::Type zyz;
		typename SwizzlerGenerator<2, 1, 3>::Type zyw;
		typename SwizzlerGenerator<2, 2, 0>::Type zzx;
		typename SwizzlerGenerator<2, 2, 1>::Type zzy;
		typename SwizzlerGenerator<2, 2, 2>::Type zzz;
		typename SwizzlerGenerator<2, 2, 3>::Type zzw;
		typename SwizzlerGenerator<2, 3, 0>::Type zwx;
		typename SwizzlerGenerator<2, 3, 1>::Type zwy;
		typename SwizzlerGenerator<2, 3, 2>::Type zwz;
		typename SwizzlerGenerator<2, 3, 3>::Type zww;
		typename SwizzlerGenerator<3, 0, 0>::Type wxx;
		typename SwizzlerGenerator<3, 0, 1>::Type wxy;
		typename SwizzlerGenerator<3, 0, 2>::Type wxz;
		typename SwizzlerGenerator<3, 0, 3>::Type wxw;
		typename SwizzlerGenerator<3, 1, 0>::Type wyx;
		typename SwizzlerGenerator<3, 1, 1>::Type wyy;
		typename SwizzlerGenerator<3, 1, 2>::Type wyz;
		typename SwizzlerGenerator<3, 1, 3>::Type wyw;
		typename SwizzlerGenerator<3, 2, 0>::Type wzx;
		typename SwizzlerGenerator<3, 2, 1>::Type wzy;
		typename SwizzlerGenerator<3, 2, 2>::Type wzz;
		typename SwizzlerGenerator<3, 2, 3>::Type wzw;
		typename SwizzlerGenerator<3, 3, 0>::Type wwx;
		typename SwizzlerGenerator<3, 3, 1>::Type wwy;
		typename SwizzlerGenerator<3, 3, 2>::Type wwz;
		typename SwizzlerGenerator<3, 3, 3>::Type www;

		// 4-Component Swizzlers
		typename SwizzlerGenerator<0, 0, 0, 0>::Type xxxx;
		typename SwizzlerGenerator<0, 0, 0, 1>::Type xxxy;
		typename SwizzlerGenerator<0, 0, 0, 2>::Type xxxz;
		typename SwizzlerGenerator<0, 0, 0, 3>::Type xxxw;
		typename SwizzlerGenerator<0, 0, 1, 0>::Type xxyx;
		typename SwizzlerGenerator<0, 0, 1, 1>::Type xxyy;
		typename SwizzlerGenerator<0, 0, 1, 2>::Type xxyz;
		typename SwizzlerGenerator<0, 0, 1, 3>::Type xxyw;
		typename SwizzlerGenerator<0, 0, 2, 0>::Type xxzx;
		typename SwizzlerGenerator<0, 0, 2, 1>::Type xxzy;
		typename SwizzlerGenerator<0, 0, 2, 2>::Type xxzz;
		typename SwizzlerGenerator<0, 0, 2, 3>::Type xxzw;
		typename SwizzlerGenerator<0, 0, 3, 0>::Type xxwx;
		typename SwizzlerGenerator<0, 0, 3, 1>::Type xxwy;
		typename SwizzlerGenerator<0, 0, 3, 2>::Type xxwz;
		typename SwizzlerGenerator<0, 0, 3, 3>::Type xxww;
		typename SwizzlerGenerator<0, 1, 0, 0>::Type xyxx;
		typename SwizzlerGenerator<0, 1, 0, 1>::Type xyxy;
		typename SwizzlerGenerator<0, 1, 0, 2>::Type xyxz;
		typename SwizzlerGenerator<0, 1, 0, 3>::Type xyxw;
		typename SwizzlerGenerator<0, 1, 1, 0>::Type xyyx;
		typename SwizzlerGenerator<0, 1, 1, 1>::Type xyyy;
		typename SwizzlerGenerator<0, 1, 1, 2>::Type xyyz;
		typename SwizzlerGenerator<0, 1, 1, 3>::Type xyyw;
		typename SwizzlerGenerator<0, 1, 2, 0>::Type xyzx;
		typename SwizzlerGenerator<0, 1, 2, 1>::Type xyzy;
		typename SwizzlerGenerator<0, 1, 2, 2>::Type xyzz;
		typename SwizzlerGenerator<0, 1, 2, 3>::Type xyzw;
		typename SwizzlerGenerator<0, 1, 3, 0>::Type xywx;
		typename SwizzlerGenerator<0, 1, 3, 1>::Type xywy;
		typename SwizzlerGenerator<0, 1, 3, 2>::Type xywz;
		typename SwizzlerGenerator<0, 1, 3, 3>::Type xyww;
		typename SwizzlerGenerator<0, 2, 0, 0>::Type xzxx;
		typename SwizzlerGenerator<0, 2, 0, 1>::Type xzxy;
		typename SwizzlerGenerator<0, 2, 0, 2>::Type xzxz;
		typename SwizzlerGenerator<0, 2, 0, 3>::Type xzxw;
		typename SwizzlerGenerator<0, 2, 1, 0>::Type xzyx;
		typename SwizzlerGenerator<0, 2, 1, 1>::Type xzyy;
		typename SwizzlerGenerator<0, 2, 1, 2>::Type xzyz;
		typename SwizzlerGenerator<0, 2, 1, 3>::Type xzyw;
		typename SwizzlerGenerator<0, 2, 2, 0>::Type xzzx;
		typename SwizzlerGenerator<0, 2, 2, 1>::Type xzzy;
		typename SwizzlerGenerator<0, 2, 2, 2>::Type xzzz;
		typename SwizzlerGenerator<0, 2, 2, 3>::Type xzzw;
		typename SwizzlerGenerator<0, 2, 3, 0>::Type xzwx;
		typename SwizzlerGenerator<0, 2, 3, 1>::Type xzwy;
		typename SwizzlerGenerator<0, 2, 3, 2>::Type xzwz;
		typename SwizzlerGenerator<0, 2, 3, 3>::Type xzww;
		typename SwizzlerGenerator<0, 3, 0, 0>::Type xwxx;
		typename SwizzlerGenerator<0, 3, 0, 1>::Type xwxy;
		typename SwizzlerGenerator<0, 3, 0, 2>::Type xwxz;
		typename SwizzlerGenerator<0, 3, 0, 3>::Type xwxw;
		typename SwizzlerGenerator<0, 3, 1, 0>::Type xwyx;
		typename SwizzlerGenerator<0, 3, 1, 1>::Type xwyy;
		typename SwizzlerGenerator<0, 3, 1, 2>::Type xwyz;
		typename SwizzlerGenerator<0, 3, 1, 3>::Type xwyw;
		typename SwizzlerGenerator<0, 3, 2, 0>::Type xwzx;
		typename SwizzlerGenerator<0, 3, 2, 1>::Type xwzy;
		typename SwizzlerGenerator<0, 3, 2, 2>::Type xwzz;
		typename SwizzlerGenerator<0, 3, 2, 3>::Type xwzw;
		typename SwizzlerGenerator<0, 3, 3, 0>::Type xwwx;
		typename SwizzlerGenerator<0, 3, 3, 1>::Type xwwy;
		typename SwizzlerGenerator<0, 3, 3, 2>::Type xwwz;
		typename SwizzlerGenerator<0, 3, 3, 3>::Type xwww;
		typename SwizzlerGenerator<1, 0, 0, 0>::Type yxxx;
		typename SwizzlerGenerator<1, 0, 0, 1>::Type yxxy;
		typename SwizzlerGenerator<1, 0, 0, 2>::Type yxxz;
		typename SwizzlerGenerator<1, 0, 0, 3>::Type yxxw;
		typename SwizzlerGenerator<1, 0, 1, 0>::Type yxyx;
		typename SwizzlerGenerator<1, 0, 1, 1>::Type yxyy;
		typename SwizzlerGenerator<1, 0, 1, 2>::Type yxyz;
		typename SwizzlerGenerator<1, 0, 1, 3>::Type yxyw;
		typename SwizzlerGenerator<1, 0, 2, 0>::Type yxzx;
		typename SwizzlerGenerator<1, 0, 2, 1>::Type yxzy;
		typename SwizzlerGenerator<1, 0, 2, 2>::Type yxzz;
		typename SwizzlerGenerator<1, 0, 2, 3>::Type yxzw;
		typename SwizzlerGenerator<1, 0, 3, 0>::Type yxwx;
		typename SwizzlerGenerator<1, 0, 3, 1>::Type yxwy;
		typename SwizzlerGenerator<1, 0, 3, 2>::Type yxwz;
		typename SwizzlerGenerator<1, 0, 3, 3>::Type yxww;
		typename SwizzlerGenerator<1, 1, 0, 0>::Type yyxx;
		typename SwizzlerGenerator<1, 1, 0, 1>::Type yyxy;
		typename SwizzlerGenerator<1, 1, 0, 2>::Type yyxz;
		typename SwizzlerGenerator<1, 1, 0, 3>::Type yyxw;
		typename SwizzlerGenerator<1, 1, 1, 0>::Type yyyx;
		typename SwizzlerGenerator<1, 1, 1, 1>::Type yyyy;
		typename SwizzlerGenerator<1, 1, 1, 2>::Type yyyz;
		typename SwizzlerGenerator<1, 1, 1, 3>::Type yyyw;
		typename SwizzlerGenerator<1, 1, 2, 0>::Type yyzx;
		typename SwizzlerGenerator<1, 1, 2, 1>::Type yyzy;
		typename SwizzlerGenerator<1, 1, 2, 2>::Type yyzz;
		typename SwizzlerGenerator<1, 1, 2, 3>::Type yyzw;
		typename SwizzlerGenerator<1, 1, 3, 0>::Type yywx;
		typename SwizzlerGenerator<1, 1, 3, 1>::Type yywy;
		typename SwizzlerGenerator<1, 1, 3, 2>::Type yywz;
		typename SwizzlerGenerator<1, 1, 3, 3>::Type yyww;
		typename SwizzlerGenerator<1, 2, 0, 0>::Type yzxx;
		typename SwizzlerGenerator<1, 2, 0, 1>::Type yzxy;
		typename SwizzlerGenerator<1, 2, 0, 2>::Type yzxz;
		typename SwizzlerGenerator<1, 2, 0, 3>::Type yzxw;
		typename SwizzlerGenerator<1, 2, 1, 0>::Type yzyx;
		typename SwizzlerGenerator<1, 2, 1, 1>::Type yzyy;
		typename SwizzlerGenerator<1, 2, 1, 2>::Type yzyz;
		typename SwizzlerGenerator<1, 2, 1, 3>::Type yzyw;
		typename SwizzlerGenerator<1, 2, 2, 0>::Type yzzx;
		typename SwizzlerGenerator<1, 2, 2, 1>::Type yzzy;
		typename SwizzlerGenerator<1, 2, 2, 2>::Type yzzz;
		typename SwizzlerGenerator<1, 2, 2, 3>::Type yzzw;
		typename SwizzlerGenerator<1, 2, 3, 0>::Type yzwx;
		typename SwizzlerGenerator<1, 2, 3, 1>::Type yzwy;
		typename SwizzlerGenerator<1, 2, 3, 2>::Type yzwz;
		typename SwizzlerGenerator<1, 2, 3, 3>::Type yzww;
		typename SwizzlerGenerator<1, 3, 0, 0>::Type ywxx;
		typename SwizzlerGenerator<1, 3, 0, 1>::Type ywxy;
		typename SwizzlerGenerator<1, 3, 0, 2>::Type ywxz;
		typename SwizzlerGenerator<1, 3, 0, 3>::Type ywxw;
		typename SwizzlerGenerator<1, 3, 1, 0>::Type ywyx;
		typename SwizzlerGenerator<1, 3, 1, 1>::Type ywyy;
		typename SwizzlerGenerator<1, 3, 1, 2>::Type ywyz;
		typename SwizzlerGenerator<1, 3, 1, 3>::Type ywyw;
		typename SwizzlerGenerator<1, 3, 2, 0>::Type ywzx;
		typename SwizzlerGenerator<1, 3, 2, 1>::Type ywzy;
		typename SwizzlerGenerator<1, 3, 2, 2>::Type ywzz;
		typename SwizzlerGenerator<1, 3, 2, 3>::Type ywzw;
		typename SwizzlerGenerator<1, 3, 3, 0>::Type ywwx;
		typename SwizzlerGenerator<1, 3, 3, 1>::Type ywwy;
		typename SwizzlerGenerator<1, 3, 3, 2>::Type ywwz;
		typename SwizzlerGenerator<1, 3, 3, 3>::Type ywww;
		typename SwizzlerGenerator<2, 0, 0, 0>::Type zxxx;
		typename SwizzlerGenerator<2, 0, 0, 1>::Type zxxy;
		typename SwizzlerGenerator<2, 0, 0, 2>::Type zxxz;
		typename SwizzlerGenerator<2, 0, 0, 3>::Type zxxw;
		typename SwizzlerGenerator<2, 0, 1, 0>::Type zxyx;
		typename SwizzlerGenerator<2, 0, 1, 1>::Type zxyy;
		typename SwizzlerGenerator<2, 0, 1, 2>::Type zxyz;
		typename SwizzlerGenerator<2, 0, 1, 3>::Type zxyw;
		typename SwizzlerGenerator<2, 0, 2, 0>::Type zxzx;
		typename SwizzlerGenerator<2, 0, 2, 1>::Type zxzy;
		typename SwizzlerGenerator<2, 0, 2, 2>::Type zxzz;
		typename SwizzlerGenerator<2, 0, 2, 3>::Type zxzw;
		typename SwizzlerGenerator<2, 0, 3, 0>::Type zxwx;
		typename SwizzlerGenerator<2, 0, 3, 1>::Type zxwy;
		typename SwizzlerGenerator<2, 0, 3, 2>::Type zxwz;
		typename SwizzlerGenerator<2, 0, 3, 3>::Type zxww;
		typename SwizzlerGenerator<2, 1, 0, 0>::Type zyxx;
		typename SwizzlerGenerator<2, 1, 0, 1>::Type zyxy;
		typename SwizzlerGenerator<2, 1, 0, 2>::Type zyxz;
		typename SwizzlerGenerator<2, 1, 0, 3>::Type zyxw;
		typename SwizzlerGenerator<2, 1, 1, 0>::Type zyyx;
		typename SwizzlerGenerator<2, 1, 1, 1>::Type zyyy;
		typename SwizzlerGenerator<2, 1, 1, 2>::Type zyyz;
		typename SwizzlerGenerator<2, 1, 1, 3>::Type zyyw;
		typename SwizzlerGenerator<2, 1, 2, 0>::Type zyzx;
		typename SwizzlerGenerator<2, 1, 2, 1>::Type zyzy;
		typename SwizzlerGenerator<2, 1, 2, 2>::Type zyzz;
		typename SwizzlerGenerator<2, 1, 2, 3>::Type zyzw;
		typename SwizzlerGenerator<2, 1, 3, 0>::Type zywx;
		typename SwizzlerGenerator<2, 1, 3, 1>::Type zywy;
		typename SwizzlerGenerator<2, 1, 3, 2>::Type zywz;
		typename SwizzlerGenerator<2, 1, 3, 3>::Type zyww;
		typename SwizzlerGenerator<2, 2, 0, 0>::Type zzxx;
		typename SwizzlerGenerator<2, 2, 0, 1>::Type zzxy;
		typename SwizzlerGenerator<2, 2, 0, 2>::Type zzxz;
		typename SwizzlerGenerator<2, 2, 0, 3>::Type zzxw;
		typename SwizzlerGenerator<2, 2, 1, 0>::Type zzyx;
		typename SwizzlerGenerator<2, 2, 1, 1>::Type zzyy;
		typename SwizzlerGenerator<2, 2, 1, 2>::Type zzyz;
		typename SwizzlerGenerator<2, 2, 1, 3>::Type zzyw;
		typename SwizzlerGenerator<2, 2, 2, 0>::Type zzzx;
		typename SwizzlerGenerator<2, 2, 2, 1>::Type zzzy;
		typename SwizzlerGenerator<2, 2, 2, 2>::Type zzzz;
		typename SwizzlerGenerator<2, 2, 2, 3>::Type zzzw;
		typename SwizzlerGenerator<2, 2, 3, 0>::Type zzwx;
		typename SwizzlerGenerator<2, 2, 3, 1>::Type zzwy;
		typename SwizzlerGenerator<2, 2, 3, 2>::Type zzwz;
		typename SwizzlerGenerator<2, 2, 3, 3>::Type zzww;
		typename SwizzlerGenerator<2, 3, 0, 0>::Type zwxx;
		typename SwizzlerGenerator<2, 3, 0, 1>::Type zwxy;
		typename SwizzlerGenerator<2, 3, 0, 2>::Type zwxz;
		typename SwizzlerGenerator<2, 3, 0, 3>::Type zwxw;
		typename SwizzlerGenerator<2, 3, 1, 0>::Type zwyx;
		typename SwizzlerGenerator<2, 3, 1, 1>::Type zwyy;
		typename SwizzlerGenerator<2, 3, 1, 2>::Type zwyz;
		typename SwizzlerGenerator<2, 3, 1, 3>::Type zwyw;
		typename SwizzlerGenerator<2, 3, 2, 0>::Type zwzx;
		typename SwizzlerGenerator<2, 3, 2, 1>::Type zwzy;
		typename SwizzlerGenerator<2, 3, 2, 2>::Type zwzz;
		typename SwizzlerGenerator<2, 3, 2, 3>::Type zwzw;
		typename SwizzlerGenerator<2, 3, 3, 0>::Type zwwx;
		typename SwizzlerGenerator<2, 3, 3, 1>::Type zwwy;
		typename SwizzlerGenerator<2, 3, 3, 2>::Type zwwz;
		typename SwizzlerGenerator<2, 3, 3, 3>::Type zwww;
		typename SwizzlerGenerator<3, 0, 0, 0>::Type wxxx;
		typename SwizzlerGenerator<3, 0, 0, 1>::Type wxxy;
		typename SwizzlerGenerator<3, 0, 0, 2>::Type wxxz;
		typename SwizzlerGenerator<3, 0, 0, 3>::Type wxxw;
		typename SwizzlerGenerator<3, 0, 1, 0>::Type wxyx;
		typename SwizzlerGenerator<3, 0, 1, 1>::Type wxyy;
		typename SwizzlerGenerator<3, 0, 1, 2>::Type wxyz;
		typename SwizzlerGenerator<3, 0, 1, 3>::Type wxyw;
		typename SwizzlerGenerator<3, 0, 2, 0>::Type wxzx;
		typename SwizzlerGenerator<3, 0, 2, 1>::Type wxzy;
		typename SwizzlerGenerator<3, 0, 2, 2>::Type wxzz;
		typename SwizzlerGenerator<3, 0, 2, 3>::Type wxzw;
		typename SwizzlerGenerator<3, 0, 3, 0>::Type wxwx;
		typename SwizzlerGenerator<3, 0, 3, 1>::Type wxwy;
		typename SwizzlerGenerator<3, 0, 3, 2>::Type wxwz;
		typename SwizzlerGenerator<3, 0, 3, 3>::Type wxww;
		typename SwizzlerGenerator<3, 1, 0, 0>::Type wyxx;
		typename SwizzlerGenerator<3, 1, 0, 1>::Type wyxy;
		typename SwizzlerGenerator<3, 1, 0, 2>::Type wyxz;
		typename SwizzlerGenerator<3, 1, 0, 3>::Type wyxw;
		typename SwizzlerGenerator<3, 1, 1, 0>::Type wyyx;
		typename SwizzlerGenerator<3, 1, 1, 1>::Type wyyy;
		typename SwizzlerGenerator<3, 1, 1, 2>::Type wyyz;
		typename SwizzlerGenerator<3, 1, 1, 3>::Type wyyw;
		typename SwizzlerGenerator<3, 1, 2, 0>::Type wyzx;
		typename SwizzlerGenerator<3, 1, 2, 1>::Type wyzy;
		typename SwizzlerGenerator<3, 1, 2, 2>::Type wyzz;
		typename SwizzlerGenerator<3, 1, 2, 3>::Type wyzw;
		typename SwizzlerGenerator<3, 1, 3, 0>::Type wywx;
		typename SwizzlerGenerator<3, 1, 3, 1>::Type wywy;
		typename SwizzlerGenerator<3, 1, 3, 2>::Type wywz;
		typename SwizzlerGenerator<3, 1, 3, 3>::Type wyww;
		typename SwizzlerGenerator<3, 2, 0, 0>::Type wzxx;
		typename SwizzlerGenerator<3, 2, 0, 1>::Type wzxy;
		typename SwizzlerGenerator<3, 2, 0, 2>::Type wzxz;
		typename SwizzlerGenerator<3, 2, 0, 3>::Type wzxw;
		typename SwizzlerGenerator<3, 2, 1, 0>::Type wzyx;
		typename SwizzlerGenerator<3, 2, 1, 1>::Type wzyy;
		typename SwizzlerGenerator<3, 2, 1, 2>::Type wzyz;
		typename SwizzlerGenerator<3, 2, 1, 3>::Type wzyw;
		typename SwizzlerGenerator<3, 2, 2, 0>::Type wzzx;
		typename SwizzlerGenerator<3, 2, 2, 1>::Type wzzy;
		typename SwizzlerGenerator<3, 2, 2, 2>::Type wzzz;
		typename SwizzlerGenerator<3, 2, 2, 3>::Type wzzw;
		typename SwizzlerGenerator<3, 2, 3, 0>::Type wzwx;
		typename SwizzlerGenerator<3, 2, 3, 1>::Type wzwy;
		typename SwizzlerGenerator<3, 2, 3, 2>::Type wzwz;
		typename SwizzlerGenerator<3, 2, 3, 3>::Type wzww;
		typename SwizzlerGenerator<3, 3, 0, 0>::Type wwxx;
		typename SwizzlerGenerator<3, 3, 0, 1>::Type wwxy;
		typename SwizzlerGenerator<3, 3, 0, 2>::Type wwxz;
		typename SwizzlerGenerator<3, 3, 0, 3>::Type wwxw;
		typename SwizzlerGenerator<3, 3, 1, 0>::Type wwyx;
		typename SwizzlerGenerator<3, 3, 1, 1>::Type wwyy;
		typename SwizzlerGenerator<3, 3, 1, 2>::Type wwyz;
		typename SwizzlerGenerator<3, 3, 1, 3>::Type wwyw;
		typename SwizzlerGenerator<3, 3, 2, 0>::Type wwzx;
		typename SwizzlerGenerator<3, 3, 2, 1>::Type wwzy;
		typename SwizzlerGenerator<3, 3, 2, 2>::Type wwzz;
		typename SwizzlerGenerator<3, 3, 2, 3>::Type wwzw;
		typename SwizzlerGenerator<3, 3, 3, 0>::Type wwwx;
		typename SwizzlerGenerator<3, 3, 3, 1>::Type wwwy;
		typename SwizzlerGenerator<3, 3, 3, 2>::Type wwwz;
		typename SwizzlerGenerator<3, 3, 3, 3>::Type wwww;
	};
};
