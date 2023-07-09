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
#ifndef VULC_TEMPLATE_TILE
#define VULC_TEMPLATE_TILE

#include "main.h"

#define TILE_TYPES (0)

#define TILE_INVALID (-1)

struct Level;
struct Tile {
    bool is_solid;

    void (*draw)(struct Level *level, i32 x, i32 y);
};

extern const struct Tile tile_list[TILE_TYPES];

inline const struct Tile *tile_type(u8 id) {
    if(id < TILE_TYPES)
        return &tile_list[id];
    return NULL;
}

#endif // VULC_TEMPLATE_TILE
