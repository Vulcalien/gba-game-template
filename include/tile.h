/* Copyright 2023-2024 Vulcalien
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

enum TileTypeID {
    // TODO add tile types ...
    // e.g. TILE_FOO,

    TILE_INVALID
};
#define TILE_TYPES (TILE_INVALID)

struct Level;
struct TileType {
    bool solid;
};

extern const struct TileType tile_type_list[TILE_TYPES];

INLINE const struct TileType *tile_get_type(enum TileTypeID id) {
    if(id >= 0 && id < TILE_TYPES)
        return &tile_type_list[id];
    return NULL;
}
