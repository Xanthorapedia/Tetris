#pragma once
#ifndef INT128_HPP
#define INT128_HPP

/* INT128 - int128_t, uint128_t
// Int128.hpp
//
// Author: Jan Ringos, http://Tringi.TrimCore.cz, tringi@trimcore.cz
// Version: 2.0
// Description: Implements 128-bit Integer using double_integer template
//
// License:
//   This software is provided 'as-is', without any express or implied warranty.
//   In no event will the author be held liable for any damages arising from the
//   use of this software. 
// 
//   Permission is granted to anyone to use this software for any purpose,
//   including commercial applications, and to alter it and redistribute it
//   freely, subject to the following restrictions: 
//
//       1. The origin of this software must not be misrepresented; you must not
//          claim that you wrote the original software. If you use this software
//          in a product, an acknowledgment in the product documentation would
//          be appreciated but is not required.
//
//       2. Altered source versions must be plainly marked as such, and must not
//          be misrepresented as being the original software.
//
//       3. This notice may not be removed or altered from any source
//          distribution.
*/

#include "double_integer.hpp"

// Using "long long" rather than recursively building on "int" not only due to
// native nature of "long long" on x86-64, but it also happens to be manually
// optimized on x86 on practically all compilers.

typedef double_integer <unsigned long long,   signed long long>  int128_t;
typedef double_integer <unsigned long long, unsigned long long> uint128_t;

// typedef double_integer <unsigned long,   signed long>  int64_tx;
// typedef double_integer <unsigned long, unsigned long> uint64_tx;
// typedef double_integer <uint64_tx,  int64_tx>  int128_t;
// typedef double_integer <uint64_tx, uint64_tx> uint128_t;

#endif
