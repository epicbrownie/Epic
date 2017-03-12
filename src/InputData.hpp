//////////////////////////////////////////////////////////////////////////////
//
//            Copyright (c) 2017 Ronnie Brohn (EpicBrownie)      
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

#include <Epic/Clock.hpp>
#include <Epic/StringHash.hpp>
#include <cstdint>
#include <type_traits>

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	using InputDataID = uint64_t;
	
	struct InputButtonData;
	struct InputAxisData;
	struct InputData;
}

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	// eInputDataType
	using InputDataTypes_Type = uint8_t;

	enum class eInputDataType : InputDataTypes_Type
	{
		Button = 1,
		Axis1D = 2,
		Axis2D = 3,
		Axis3D = 4,
		Unknown = 100
	};

	// eInputButtonState
	using InputButtonStates_Type = uint8_t;

	enum class eInputButtonState : InputButtonStates_Type
	{
		Down = 1,
		Up = 0,
		Both = 10
	};

	// eInputAxis
	using InputAxis_Type = uint8_t;

	enum class eInputAxis : InputAxis_Type
	{
		X = 0,
		Y = 1,
		Z = 2,
		XY = 3,
		XZ = 4,
		YZ = 5,
		XYZ = 6
	};

	// eInputKey
	using InputKeys_Type = InputDataID;

	enum class eInputKey : InputKeys_Type
	{
		// Punctuation
		Space = 0x20,
		Apostrophe = 0x27,
		Comma = 0x2C,
		Minus = 0x2D,
		Period = 0x2E,
		Slash = 0x2F, ForwardSlash = Slash,
		Semicolon = 0x3B,
		Equal = 0x3D,
		LeftBracket = 0x5B, OpenBracket = LeftBracket,
		BackSlash = 0x5C,
		RightBracket = 0x5D, CloseBracket = RightBracket,
		GraveAccent = 0x60, BackTick = 0x60,

		// Numbers
		Num0 = 0x30,
		Num1 = Num0 + 1, Num2 = Num0 + 2, Num3 = Num0 + 3,
		Num4 = Num0 + 4, Num5 = Num0 + 5, Num6 = Num0 + 6,
		Num7 = Num0 + 7, Num8 = Num0 + 8, Num9 = Num0 + 9,

		// Alphabet
		A = 0x41,
		B = A + 1, C = A + 2, D = A + 3, E = A + 4, F = A + 5, G = A + 6, H = A + 7,
		I = A + 8, J = A + 9, K = A + 10, L = A + 11, M = A + 12, N = A + 13, O = A + 14,
		P = A + 15, Q = A + 16, R = A + 17, S = A + 18, T = A + 19, U = A + 20, V = A + 21,
		W = A + 22, X = A + 23, Y = A + 24, Z = A + 25,

		// Misc
		Clear = 0x0C, 
		Return = 0x0D,
		Select = 0x29, 
		Print = 0x2A, 
		Execute = 0x2B,
		Sleep = 0x5F,

		// World
		World1 = 0xA1,
		World2 = 0xA2,

		// Function
		Escape = 0x100,
		Enter = 0x101,
		Tab = 0x102,
		Backspace = 0x103,
		Insert = 0x104,
		Delete = 0x105,
		Right = 0x106,
		Left = 0x107,
		Down = 0x108,
		Up = 0x109,
		PageUp = 0x10A,
		PageDown = 0x10B,
		Home = 0x10C,
		End = 0x10D,
		CapsLock = 0x118,
		ScrollLock = 0x119,
		NumLock = 0x11A,
		PrintScreen = 0x11B,
		Pause = 0x11C,

		F1 = 0x122,
		F2 = F1 + 1,	F3 = F1 + 2,	F4 = F1 + 3,	F5 = F1 + 4,	F6 = F1 + 5, 
		F7 = F1 + 6,	F8 = F1 + 7,	F9 = F1 + 8,	F10 = F1 + 9,	F11 = F1 + 10, 
		F12 = F1 + 11,	F13 = F1 + 12,	F14 = F1 + 13,	F15 = F1 + 14,	F16 = F1 + 15, 
		F17 = F1 + 16,	F18 = F1 + 17,	F19 = F1 + 18,	F20 = F1 + 19,	F21 = F1 + 20, 
		F22 = F1 + 21,	F23 = F1 + 22,	F24 = F1 + 23,	F25 = F1 + 24,

		NumPad0 = 0x140,
		NumPad1 = NumPad0 + 1, NumPad2 = NumPad0 + 2, NumPad3 = NumPad0 + 3,
		NumPad4 = NumPad0 + 4, NumPad5 = NumPad0 + 5, NumPad6 = NumPad0 + 6,
		NumPad7 = NumPad0 + 7, NumPad8 = NumPad0 + 8, NumPad9 = NumPad0 + 9,

		NumPadDecimal = 0x14A,
		NumPadDivide = 0x14B,
		NumPadMultiply = 0x14C,
		NumPadSubtract = 0x14D,
		NumPadAdd = 0x14E,
		NumPadEnter = 0x14F,
		NumPadEqual = 0x150,

		LeftShift = 0x154,
		LeftControl = 0x155,
		LeftAlt = 0x156,
		LeftSuper = 0x157,
		RightShift = 0x158,
		RightControl = 0x159,
		RightAlt = 0x15A,
		RightSuper = 0x15B,
		Menu = 0x15C
	};

	using Keys = eInputKey;
}

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	namespace detail
	{
		static constexpr InputDataID InputTextCode = 0x00000001'00000000;
		static constexpr InputDataID InputTextCodeMask = 0xFFFFFFFF'00000000;
	}
}

//////////////////////////////////////////////////////////////////////////////

// InputButtonData
struct Epic::InputButtonData
{
	InputDataID ButtonID;
	eInputButtonState State;
};

// InputAxisData
struct Epic::InputAxisData
{
	InputDataID AxisID;
	int64_t Scalar;
	double Norm;
};

//////////////////////////////////////////////////////////////////////////////

// InputData
struct Epic::InputData
{
	eInputDataType DataType;
	Epic::StringHash Device;
	decltype(Epic::StandardClock)::TimeStamp Timestamp;

	union _Data
	{
		constexpr _Data(Epic::InputButtonData&& buttonData) noexcept
			: Button{ std::move(buttonData) } { }

		constexpr _Data(Epic::InputAxisData&& axisData) noexcept
			: Axis1D{ std::move(axisData) } { }

		constexpr _Data(Epic::InputAxisData&& axisData0, Epic::InputAxisData&& axisData1) noexcept
			: Axis2D{ std::move(axisData0), std::move(axisData1) } { }

		constexpr _Data(Epic::InputAxisData&& axisData0, Epic::InputAxisData&& axisData1, Epic::InputAxisData&& axisData2) noexcept
			: Axis3D{ std::move(axisData0), std::move(axisData1), std::move(axisData2) } { }

		Epic::InputButtonData Button;
		
		struct
		{
			Epic::InputAxisData Axis0;
		} Axis1D;

		struct 
		{
			Epic::InputAxisData Axis0;
			Epic::InputAxisData Axis1;
		} Axis2D;

		struct
		{
			Epic::InputAxisData Axis0;
			Epic::InputAxisData Axis1;
			Epic::InputAxisData Axis2;
		} Axis3D;
	} Data;
};
