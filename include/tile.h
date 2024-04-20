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

enum tile_TypeID {
    // TODO add tile types ...
    // e.g. TILE_FOO,

    TILE_INVALID
};
#define TILE_TYPES (TILE_INVALID)

struct Level;
struct tile_Type {
    bool is_solid;
};

extern const struct tile_Type * const tile_type_list[TILE_TYPES];

ALWAYS_INLINE
inline const struct tile_Type *tile_get_type(enum tile_TypeID id) {
    if(id >= 0 && id < TILE_TYPES)
        return tile_type_list[id];
    return NULL;
}

// Tile types
//extern const struct tile_Type
    // TODO add tile types ...
    // e.g. tile_foo
