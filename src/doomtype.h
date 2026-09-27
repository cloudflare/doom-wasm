//
// Copyright(C) 1993-1996 Id Software, Inc.
// Copyright(C) 2005-2014 Simon Howard
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// DESCRIPTION:
//	Simple basic typedefs, isolated here to make it easier
//	 separating modules.
//    


#ifndef __DOOMTYPE__
#define __DOOMTYPE__

#include "config.h"

#if defined(_MSC_VER) && !defined(__cplusplus)
#define inline __inline
#endif

// #define macros to provide functions missing in Windows.
// Outside Windows, we use strings.h for str[n]casecmp.


#if !HAVE_DECL_STRCASECMP || !HAVE_DECL_STRNCASECMP

#include <string.h>
#if !HAVE_DECL_STRCASECMP
#define strcasecmp stricmp
#endif
#if !HAVE_DECL_STRNCASECMP
#define strncasecmp strnicmp
#endif

#else

#include <strings.h>

#endif


//
// The packed attribute forces structures to be packed into the minimum 
// space necessary.  If this is not done, the compiler may align structure
// fields differently to optimize memory access, inflating the overall
// structure size.  It is important to use the packed attribute on certain
// structures where alignment is important, particularly data read/written
// to disk.
//

#ifdef __GNUC__

#if defined(_WIN32) && !defined(__clang__)
#define PACKEDATTR __attribute__((packed,gcc_struct))
#else
#define PACKEDATTR __attribute__((packed))
#endif

#define PRINTF_ATTR(fmt, first) __attribute__((format(printf, fmt, first)))
#define PRINTF_ARG_ATTR(x) __attribute__((format_arg(x)))
#define NORETURN __attribute__((noreturn))

#else
#if defined(_MSC_VER)
#define PACKEDATTR __pragma(pack(pop))
#else
#define PACKEDATTR
#endif
#define PRINTF_ATTR(fmt, first)
#define PRINTF_ARG_ATTR(x)
#define NORETURN
#endif

#ifdef __WATCOMC__
#define PACKEDPREFIX _Packed
#elif defined(_MSC_VER)
#define PACKEDPREFIX __pragma(pack(push,1))
#else
#define PACKEDPREFIX
#endif

#define PACKED_STRUCT(...) PACKEDPREFIX struct __VA_ARGS__ PACKEDATTR

// C99 integer types; with gcc we just use this.  Other compilers
// should add conditional statements that define the C99 types.

// What is really wanted here is stdint.h; however, some old versions
// of Solaris don't have stdint.h and only have inttypes.h (the 
// pre-standardisation version).  inttypes.h is also in the C99 
// standard and defined to include stdint.h, so include this. 

#include <inttypes.h>

#if defined(__cplusplus)

// Use builtin bool type with C++.

typedef bool boolean;

#else

// One boolean ABI for every C translation unit. Picking a 1-byte bool when
// <stdbool.h> was seen and a 4-byte enum otherwise breaks under Emscripten:
// <emscripten.h> pulls stdbool.h in (via em_js.h), so files that include it
// before this header disagree with the rest about sizeof(boolean), and
// shared globals such as playeringame[], netgame and paused read as garbage
// across that boundary. Doom also relies on boolean being int-sized and
// memset()-able to -1 (R_InstallSpriteLump), and C23 makes true/false
// keywords, so use int and only define the constants when nothing else has.
typedef int boolean;

#if !defined(__bool_true_false_are_defined) && !(defined(__STDC_VERSION__) && __STDC_VERSION__ >= 202311L)
#define false 0
#define true 1
#endif

#endif

typedef uint8_t byte;
typedef uint8_t pixel_t;
typedef int16_t dpixel_t;

#include <limits.h>

#ifdef _WIN32

#define DIR_SEPARATOR '\\'
#define DIR_SEPARATOR_S "\\"
#define PATH_SEPARATOR ';'

#else

#define DIR_SEPARATOR '/'
#define DIR_SEPARATOR_S "/"
#define PATH_SEPARATOR ':'

#endif

#define arrlen(array) (sizeof(array) / sizeof(*array))

#endif

