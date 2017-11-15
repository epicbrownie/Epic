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

#define EPIC_EXPAND(x) x
#define EPIC_CONCATENATE(x,y) x##y

#define EPIC_FOREACH_1(what, x, ...) what(x)
#define EPIC_FOREACH_2(what, x, ...) \
	what(x) \
	EPIC_EXPAND(EPIC_FOREACH_1(what, __VA_ARGS__))
#define EPIC_FOREACH_3(what, x, ...) \
	what(x) \
	EPIC_EXPAND(EPIC_FOREACH_2(what, __VA_ARGS__))
#define EPIC_FOREACH_4(what, x, ...) \
	what(x) \
	EPIC_EXPAND(EPIC_FOREACH_3(what,  __VA_ARGS__))
#define EPIC_FOREACH_5(what, x, ...) \
	what(x) \
	EPIC_EXPAND(EPIC_FOREACH_4(what,  __VA_ARGS__))
#define EPIC_FOREACH_6(what, x, ...) \
	what(x) \
	EPIC_EXPAND(EPIC_FOREACH_5(what,  __VA_ARGS__))
#define EPIC_FOREACH_7(what, x, ...) \
	what(x) \
	EPIC_EXPAND(EPIC_FOREACH_6(what,  __VA_ARGS__))
#define EPIC_FOREACH_8(what, x, ...) \
	what(x) \
	EPIC_EXPAND(EPIC_FOREACH_7(what,  __VA_ARGS__))
#define EPIC_FOREACH_9(what, x, ...) \
	what(x) \
	EPIC_EXPAND(EPIC_FOREACH_8(what,  __VA_ARGS__))
#define EPIC_FOREACH_10(what, x, ...) \
	what(x) \
	EPIC_EXPAND(EPIC_FOREACH_9(what,  __VA_ARGS__))

#define EPIC_RSEQ_N() 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0
#define EPIC_ARG_N(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, N, ...) N
#define EPIC_NARG_(...) EPIC_EXPAND(EPIC_ARG_N(__VA_ARGS__))
#define EPIC_NARG(...) EPIC_NARG_(__VA_ARGS__, EPIC_RSEQ_N())

#define EPIC_FOREACH_(N, what, ...) EPIC_EXPAND(EPIC_CONCATENATE(EPIC_FOREACH_, N)(what, __VA_ARGS__))
#define EPIC_FOREACH(what, ...) EPIC_FOREACH_(EPIC_NARG(__VA_ARGS__), what, __VA_ARGS__)
