/* Copyright 2024 Vulcalien
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

// Basic types and definitions used throughout the program.

#include "types.h"

#ifndef NULL
    #define NULL ((void *) 0)
#endif

#define static_assert _Static_assert

// Sections
#define SBSS_SECTION         __attribute__((section(".sbss")))
#define EWRAM_SECTION        __attribute__((section(".ewram")))
#define EWRAM_RODATA_SECTION __attribute__((section(".ewram.rodata")))
#define IWRAM_SECTION        __attribute__((section(".iwram")))
#define IWRAM_RODATA_SECTION __attribute__((section(".iwram.rodata")))

// Function attributes
#define ARM   __attribute__((target("arm")))
#define THUMB __attribute__((target("thumb")))

#define NOINLINE __attribute__((noinline))
#define NOCLONE  __attribute__((noclone))

#define ALWAYS_INLINE __attribute__((always_inline))

// Constants
#define CLOCK_FREQUENCY (16 * 1024 * 1024)
