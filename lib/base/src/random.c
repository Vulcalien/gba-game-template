/* Copyright 2022 Vulcalien
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */
#include "random.h"

static u32 seed = 0;

IWRAM_SECTION
u16 rand(void) {
    seed = seed * 0x248f7b13 + 0xc21840c5;
    return seed >> 16;
}

void srand(u32 val, bool reset) {
    if(reset)
        seed = val;
    else
        seed += val;
}
