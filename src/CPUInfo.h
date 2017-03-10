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

#include <Epic/STL/String.hpp>
#include <Epic/STL/Vector.hpp>
#include <array>
#include <bitset>
#include <intrin.h>
#include <iostream>

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	class CPUInfo;
}

//////////////////////////////////////////////////////////////////////////////

// CPUInfo
class Epic::CPUInfo
{
private:
	CPUInfo() = delete;

private:
	struct InstructionSet
	{
		Epic::String Vendor, Brand;
		Epic::STLVector<std::array<int, 4>> Data, ExtData;
		std::bitset<32> Fn1ECX, Fn1EDX, Fn7EBX, Fn7ECX, Fn81ECX, Fn81EDX;
		int IDs, ExIDs;
		bool IsIntel, IsAMD;

		InstructionSet() noexcept
			: IDs{ 0 }, ExIDs{ 0 }, IsIntel{ false }, IsAMD{ false },
			  Fn1ECX{ 0 }, Fn1EDX{ 0 }, Fn7EBX{ 0 }, Fn7ECX{ 0 },
			  Fn81ECX{ 0 }, Fn81EDX{ 0 }, Data{}, ExtData{}
		{
			std::array<int, 4> cpuidData;

			// __cpuid(0x0) - Get the number of the highest valid function ID.
			__cpuid(cpuidData.data(), 0);

			// Data
			IDs = cpuidData[0];
			for (int i = 0; i <= IDs; ++i)
			{
				__cpuidex(cpuidData.data(), i, 0);
				Data.emplace_back(cpuidData);
			}

			// Vendor
			char vendor[0x20];
			memset(vendor, 0, sizeof(vendor));
			*reinterpret_cast<int*>(vendor + 0) = Data[0][1];
			*reinterpret_cast<int*>(vendor + 4) = Data[0][3];
			*reinterpret_cast<int*>(vendor + 8) = Data[0][2];
			Vendor = vendor;

			// IsIntel / IsAMD
			IsIntel = (Vendor == "GenuineIntel");
			IsAMD = (Vendor == "AuthenticAMD");

			// Load bitset with flags for function 0x00000001
			if (IDs >= 1)
			{
				Fn1ECX = Data[1][2];
				Fn1EDX = Data[1][3];
			}

			// Load bitset with flags for function 0x00000007
			if (IDs >= 7)
			{
				Fn7EBX = Data[7][1];
				Fn7ECX = Data[7][2];
			}

			// __cpuid(0x80000000) - Get the number of the highest valid extended ID.
			__cpuid(cpuidData.data(), 0x80000000);
			ExIDs = cpuidData[0];

			// ExtData
			for (int i = 0x80000000; i <= ExIDs; ++i)
			{
				__cpuidex(cpuidData.data(), i, 0);
				ExtData.emplace_back(cpuidData);
			}

			// Load bitset with flags for function 0x80000001
			if (ExIDs >= 0x80000001)
			{
				Fn81ECX = ExtData[1][2];
				Fn81EDX = ExtData[1][3];
			}

			// Brand
			if (ExIDs >= 0x80000004)
			{
				char brand[0x40];
				memset(brand, 0, sizeof(brand));
				memcpy(brand + 0, ExtData[2].data(), sizeof(cpuidData));
				memcpy(brand + 16, ExtData[3].data(), sizeof(cpuidData));
				memcpy(brand + 32, ExtData[4].data(), sizeof(cpuidData));
				Brand = brand;
			}
		}
	};

public:
	static inline const Epic::String& Vendor() noexcept	{ return s_InstructionSet.Vendor; }
	static inline const Epic::String& Brand() noexcept	{ return s_InstructionSet.Brand; }

	static inline bool SSE3() noexcept			{ return s_InstructionSet.Fn1ECX[0]; }
	static inline bool PCLMULQDQ() noexcept		{ return s_InstructionSet.Fn1ECX[1]; }
	static inline bool MONITOR() noexcept		{ return s_InstructionSet.Fn1ECX[3]; }
	static inline bool SSSE3() noexcept			{ return s_InstructionSet.Fn1ECX[9]; }
	static inline bool FMA() noexcept			{ return s_InstructionSet.Fn1ECX[12]; }
	static inline bool CMPXCHG16B() noexcept	{ return s_InstructionSet.Fn1ECX[13]; }
	static inline bool SSE41() noexcept			{ return s_InstructionSet.Fn1ECX[19]; }
	static inline bool SSE42() noexcept			{ return s_InstructionSet.Fn1ECX[20]; }
	static inline bool MOVBE() noexcept			{ return s_InstructionSet.Fn1ECX[22]; }
	static inline bool POPCNT() noexcept		{ return s_InstructionSet.Fn1ECX[23]; }
	static inline bool AES() noexcept			{ return s_InstructionSet.Fn1ECX[25]; }
	static inline bool XSAVE() noexcept			{ return s_InstructionSet.Fn1ECX[26]; }
	static inline bool OSXSAVE() noexcept		{ return s_InstructionSet.Fn1ECX[27]; }
	static inline bool AVX() noexcept			{ return s_InstructionSet.Fn1ECX[28]; }
	static inline bool F16C() noexcept			{ return s_InstructionSet.Fn1ECX[29]; }
	static inline bool RDRAND() noexcept		{ return s_InstructionSet.Fn1ECX[30]; }

	static inline bool MSR() noexcept		{ return s_InstructionSet.Fn1EDX[5]; }
	static inline bool CX8() noexcept		{ return s_InstructionSet.Fn1EDX[8]; }
	static inline bool SEP() noexcept		{ return s_InstructionSet.Fn1EDX[11]; }
	static inline bool CMOV() noexcept		{ return s_InstructionSet.Fn1EDX[15]; }
	static inline bool CLFSH() noexcept		{ return s_InstructionSet.Fn1EDX[19]; }
	static inline bool MMX() noexcept		{ return s_InstructionSet.Fn1EDX[23]; }
	static inline bool FXSR() noexcept		{ return s_InstructionSet.Fn1EDX[24]; }
	static inline bool SSE() noexcept		{ return s_InstructionSet.Fn1EDX[25]; }
	static inline bool SSE2() noexcept		{ return s_InstructionSet.Fn1EDX[26]; }

	static inline bool FSGSBASE() noexcept	{ return s_InstructionSet.Fn7EBX[0]; }
	static inline bool BMI1() noexcept		{ return s_InstructionSet.Fn7EBX[3]; }
	static inline bool HLE() noexcept		{ return s_InstructionSet.IsIntel && s_InstructionSet.Fn7EBX[4]; }
	static inline bool AVX2() noexcept		{ return s_InstructionSet.Fn7EBX[5]; }
	static inline bool BMI2() noexcept		{ return s_InstructionSet.Fn7EBX[8]; }
	static inline bool ERMS() noexcept		{ return s_InstructionSet.Fn7EBX[9]; }
	static inline bool INVPCID() noexcept	{ return s_InstructionSet.Fn7EBX[10]; }
	static inline bool RTM() noexcept		{ return s_InstructionSet.IsIntel && s_InstructionSet.Fn7EBX[11]; }
	static inline bool AVX512F() noexcept	{ return s_InstructionSet.Fn7EBX[16]; }
	static inline bool RDSEED() noexcept	{ return s_InstructionSet.Fn7EBX[18]; }
	static inline bool ADX() noexcept		{ return s_InstructionSet.Fn7EBX[19]; }
	static inline bool AVX512PF() noexcept	{ return s_InstructionSet.Fn7EBX[26]; }
	static inline bool AVX512ER() noexcept	{ return s_InstructionSet.Fn7EBX[27]; }
	static inline bool AVX512CD() noexcept	{ return s_InstructionSet.Fn7EBX[28]; }
	static inline bool SHA() noexcept		{ return s_InstructionSet.Fn7EBX[29]; }

	static inline bool PREFETCHWT1() noexcept	{ return s_InstructionSet.Fn7ECX[0]; }

	static inline bool LAHF() noexcept		{ return s_InstructionSet.Fn81ECX[0]; }
	static inline bool LZCNT() noexcept		{ return s_InstructionSet.IsIntel && s_InstructionSet.Fn81ECX[5]; }
	static inline bool ABM() noexcept		{ return s_InstructionSet.IsAMD   && s_InstructionSet.Fn81ECX[5]; }
	static inline bool SSE4a() noexcept		{ return s_InstructionSet.IsAMD   && s_InstructionSet.Fn81ECX[6]; }
	static inline bool XOP() noexcept		{ return s_InstructionSet.IsAMD   && s_InstructionSet.Fn81ECX[11]; }
	static inline bool TBM() noexcept		{ return s_InstructionSet.IsAMD   && s_InstructionSet.Fn81ECX[21]; }

	static inline bool SYSCALL() noexcept	{ return s_InstructionSet.IsIntel && s_InstructionSet.Fn81EDX[11]; }
	static inline bool MMXEXT() noexcept	{ return s_InstructionSet.IsAMD   && s_InstructionSet.Fn81EDX[22]; }
	static inline bool RDTSCP() noexcept	{ return s_InstructionSet.IsIntel && s_InstructionSet.Fn81EDX[27]; }
	static inline bool _3DNOWEXT() noexcept	{ return s_InstructionSet.IsAMD   && s_InstructionSet.Fn81EDX[30]; }
	static inline bool _3DNOW() noexcept	{ return s_InstructionSet.IsAMD   && s_InstructionSet.Fn81EDX[31]; }

private:
	static const InstructionSet s_InstructionSet;
};

//////////////////////////////////////////////////////////////////////////////

const decltype(Epic::CPUInfo::s_InstructionSet) Epic::CPUInfo::s_InstructionSet;
