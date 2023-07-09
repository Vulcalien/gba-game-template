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
#ifndef VULC_TEMPLATE_LEVEL
#define VULC_TEMPLATE_LEVEL

#include "main.h"

#include "tile.h"
#include "entity.h"

// Level size (in tiles)
#define LEVEL_W (30)
#define LEVEL_H (20)
#define LEVEL_SIZE (LEVEL_W * LEVEL_H)

// Tile size: 3 = 8x8, 4 = 16x16, 5 = 32x32
#define LEVEL_TILE_SIZE (3)

// If this value is more than 255, then change the type of
// 'solid_entities' to a u16 array.
#define LEVEL_ENTITY_LIMIT (255)

#define LEVEL_SOLID_ENTITIES_IN_TILE (4)

struct Level {
    u8 tiles[LEVEL_SIZE];

    struct entity_Data entities[LEVEL_ENTITY_LIMIT];
    u8 solid_entities[LEVEL_SIZE][LEVEL_SOLID_ENTITIES_IN_TILE];
};

extern void level_tick(struct Level *level);
extern void level_draw(struct Level *level);

inline u8 level_get_tile(struct Level *level, i32 x, i32 y) {
    if(x >= 0 && y >= 0 && x < LEVEL_W && y < LEVEL_H)
        return level->tiles[x + y * LEVEL_W];
    return TILE_INVALID;
}

inline void level_set_tile(struct Level *level, i32 x, i32 y, u8 id) {
    if(x >= 0 && y >= 0 && x < LEVEL_W && y < LEVEL_H)
        level->tiles[x + y * LEVEL_W] = id;
}

#endif // VULC_TEMPLATE_LEVEL
