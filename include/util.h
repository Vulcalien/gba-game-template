/* Copyright 2023 Vulcalien
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#pragma once

#include "main.h"

extern void memcpy16(vu16 *dest, const vu16 *src, u32 n);

// === Pseudorandom Number Generator ===
extern u16 rand(void);
extern void srand(u32 val, bool reset);

// === Math functions ===
// Approximate 'sin' and 'cos' functions
//
// 'angle' in range [0, 0xffff]
// Output in range [-0x4000, 0x4000]
extern i16 math_sin(u16 angle);
extern i16 math_cos(u16 angle);

// ===== ===== =====

ALWAYS_INLINE
inline void itoa(u32 number, u8 radix, char *array,
                 u8 digits, bool zero_fill) {
    u32 pos = 0;
    for(u32 i = 0; i < digits; i++) {
        u32 digit = number;
        for(u32 j = 1; j < digits - i; j++)
            digit /= radix;
        digit %= radix;

        if(digit != 0 || pos != 0 || i == digits - 1 || zero_fill) {
            if(radix <= 10 || digit < 10)
                array[pos++] = '0' + digit;
            else
                array[pos++] = 'A' + (digit - 10);
        }
    }
}
