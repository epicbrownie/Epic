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

#include <Epic/TMP/TypeTraits.hpp>

//////////////////////////////////////////////////////////////////////////////

namespace Epic::EON::detail
{
	template<class T>
	struct EONTraits
	{
		static constexpr bool IsVectorLike = Epic::TMP::IsVectorLike<T>::value;
		static constexpr bool IsSetLike = Epic::TMP::IsSetLike<T>::value;
		static constexpr bool IsMapLike = Epic::TMP::IsMapLike<T>::value;
		static constexpr bool IsStringLike = Epic::TMP::IsStringLike<T>::value;
		static constexpr bool IsIndexable = Epic::TMP::IsIndexable<T>::value;

		static constexpr bool IsContainer = IsVectorLike || IsSetLike || IsMapLike;
		static constexpr bool IsIndexableScalar = !IsStringLike && !IsContainer && IsIndexable;
	};
}
