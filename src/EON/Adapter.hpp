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

#include <Epic/EON/Convert.hpp>

//////////////////////////////////////////////////////////////////////////////

namespace Epic::EON
{
	template<class I, class T, class U, class IConverter = DefaultConverter>
	struct Adapter;
}

//////////////////////////////////////////////////////////////////////////////

// Adapter
template<class I, class T, class U, class IConverter>
struct Epic::EON::Adapter
{
	using Type = Epic::EON::Adapter<I, T, U, IConverter>;
	using IntermediateType = I;
	using ConverterType = IConverter;

	U T::* pDest;
	IConverter fnConvertI;

	explicit Adapter(U T::* dest, IConverter convert = IConverter())
		: pDest(dest), fnConvertI(std::move(convert))
	{ }
};


//////////////////////////////////////////////////////////////////////////////

// Adapt
namespace Epic::EON
{
	namespace
	{
		template<class I, class T, class U, class Converter = DefaultConverter>
		auto Adapt(U T::* pMember, Converter fnConvert = Converter()) noexcept
		{
			return Adapter<I, T, U, Converter>(pMember, std::move(fnConvert));
		}
	}
}
