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
#ifdef TILE_ENABLE
    #define LEVEL_W (30)
    #define LEVEL_H (20)
    #define LEVEL_SIZE (LEVEL_W * LEVEL_H)

    // 0 = 8x8, 1 = 16x16
    #define LEVEL_TILE_SIZE (0)
#endif

#if ENTITY_TYPES > 0
    #define LEVEL_ENTITY_LIMIT (255)
#endif

struct Level {
    #if LEVEL_SIZE > 0
        u8 tiles[LEVEL_SIZE];
    #endif

    #if LEVEL_ENTITY_LIMIT > 0
        struct entity_Data entities[LEVEL_ENTITY_LIMIT];
    #endif
};

extern void level_tick(struct Level *level);
extern void level_draw(struct Level *level);

#if LEVEL_SIZE > 0
inline u8 level_get_tile(struct Level *level, i32 x, i32 y) {
    if(x >= 0 && y >= 0 && x < LEVEL_W && y < LEVEL_H)
        return level->tiles[x + y * LEVEL_W];
    return TILE_INVALID;
}

inline void level_set_tile(struct Level *level, i32 x, i32 y, u8 id) {
    if(x >= 0 && y >= 0 && x < LEVEL_W && y < LEVEL_H)
        level->tiles[x + y * LEVEL_W] = id;
}
#endif // LEVEL_SIZE

#endif // VULC_TEMPLATE_LEVEL
