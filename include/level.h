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

#include "tile.h"
#include "entity.h"

// Level size (in tiles)
#define LEVEL_W (30)
#define LEVEL_H (20)
#define LEVEL_SIZE (LEVEL_W * LEVEL_H)

// Tile size: 3 = 8x8, 4 = 16x16, 5 = 32x32
#define LEVEL_TILE_SIZE (3)

typedef u8 LevelEntityID;
#define LEVEL_ENTITY_LIMIT (255)

// An invalid entity ID.
#define LEVEL_NO_ENTITY (LEVEL_ENTITY_LIMIT)

#define LEVEL_ENTITIES_IN_PARTITION 4

struct Level {
    enum TileTypeID tiles[LEVEL_SIZE];

    struct EntityData entities[LEVEL_ENTITY_LIMIT];

    LevelEntityID
    entity_partitions[LEVEL_SIZE][LEVEL_ENTITIES_IN_PARTITION];

    struct {
        i32 x;
        i32 y;
    } offset;
};

extern void level_init(struct Level *level);
extern void level_tick(struct Level *level);
extern void level_draw(struct Level *level);

// === Tile functions ===

INLINE enum TileTypeID level_get_tile(struct Level *level,
                                      i32 x, i32 y) {
    if(x >= 0 && y >= 0 && x < LEVEL_W && y < LEVEL_H)
        return level->tiles[x + y * LEVEL_W];
    return TILE_INVALID;
}

INLINE void level_set_tile(struct Level *level, i32 x, i32 y,
                           enum TileTypeID id) {
    if(x >= 0 && y >= 0 && x < LEVEL_W && y < LEVEL_H)
        level->tiles[x + y * LEVEL_W] = id;
}

// === Entity functions ===

// returns an available entity ID, or an invalid ID if there is none
extern LevelEntityID level_new_entity(struct Level *level);

// Finalizes the entity having the given ID, setting its type and
// clearing the 'should_remove' bit. Set the entity's properties
// (such as x and y coordinates) *before* calling this function.
extern void level_add_entity(struct Level *level,
                             enum EntityTypeID type,
                             LevelEntityID id);

// ===== ===== =====
