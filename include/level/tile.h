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

#define TILE_TYPES (0)

#define TILE_INVALID (-1)

struct Level;
struct Tile {
    bool is_solid;
};

extern const struct Tile *tile_list[TILE_TYPES];

ALWAYS_INLINE
inline const struct Tile *tile_type(u8 id) {
    if(id < TILE_TYPES)
        return tile_list[id];
    return NULL;
}

// Tiles
//extern const struct Tile
    // ...
