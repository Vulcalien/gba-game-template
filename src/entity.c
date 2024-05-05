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
#include "entity.h"

#include "level.h"
#include "tile.h"

const struct entity_Type * const entity_type_list[ENTITY_TYPES] = {
    [ENTITY_PLAYER] = &entity_player
};

ALWAYS_INLINE
static inline bool tile_blocks(struct Level *level, i32 x, i32 y) {
    const struct tile_Type *tile_type = tile_get_type(
        level_get_tile(level, x, y)
    );

    if(!tile_type || tile_type->is_solid)
        return true;
    return false;
}

static inline bool blocked_by_tiles(struct Level *level,
                                    struct entity_Data *data,
                                    i32 * const xm, i32 * const ym) {
    const struct entity_Type *e_type = entity_get_type(data);

    const i32 old_x0 = data->x - e_type->xr;
    const i32 old_y0 = data->y - e_type->yr;
    const i32 old_x1 = data->x + e_type->xr - 1;
    const i32 old_y1 = data->y + e_type->yr - 1;

    const i32 old_xt0 = old_x0 >> LEVEL_TILE_SIZE;
    const i32 old_yt0 = old_y0 >> LEVEL_TILE_SIZE;
    const i32 old_xt1 = old_x1 >> LEVEL_TILE_SIZE;
    const i32 old_yt1 = old_y1 >> LEVEL_TILE_SIZE;

    // For each possible direction (left, right, up, down) iterate tiles
    // from closest to farthest and, in case a blocking tile is found,
    // decrease xm or ym.
    // Note: the tile the entity is currently in is ignored.
    if(*xm < 0) { // left
        const i32 new_xt0 = (old_x0 + *xm) >> LEVEL_TILE_SIZE;

        for(i32 x = old_xt0 - 1; x >= new_xt0; x--) {
            for(i32 y = old_yt0; y <= old_yt1; y++) {
                if(tile_blocks(level, x, y)) {
                    *xm = ((x + 1) << LEVEL_TILE_SIZE) - old_x0;
                    return true;
                }
            }
        }
    } else if(*xm > 0) { // right
        const i32 new_xt1 = (old_x1 + *xm) >> LEVEL_TILE_SIZE;

        for(i32 x = old_xt1 + 1; x <= new_xt1; x++) {
            for(i32 y = old_yt0; y <= old_yt1; y++) {
                if(tile_blocks(level, x, y)) {
                    *xm = (x << LEVEL_TILE_SIZE) - 1 - old_x1;
                    return true;
                }
            }
        }
    } else if(*ym < 0) { // up
        const i32 new_yt0 = (old_y0 + *ym) >> LEVEL_TILE_SIZE;

        for(i32 y = old_yt0 - 1; y >= new_yt0; y--) {
            for(i32 x = old_xt0; x <= old_xt1; x++) {
                if(tile_blocks(level, x, y)) {
                    *ym = ((y + 1) << LEVEL_TILE_SIZE) - old_y0;
                    return true;
                }
            }
        }
    } else if(*ym > 0) { // down
        const i32 new_yt1 = (old_y1 + *ym) >> LEVEL_TILE_SIZE;

        for(i32 y = old_yt1 + 1; y <= new_yt1; y++) {
            for(i32 x = old_xt0; x <= old_xt1; x++) {
                if(tile_blocks(level, x, y)) {
                    *ym = (y << LEVEL_TILE_SIZE) - 1 - old_y1;
                    return true;
                }
            }
        }
    }
    return false;
}

static inline bool blocked_by_entities(struct Level *level,
                                       struct entity_Data *data,
                                       i32 xm, i32 ym) {
    const struct entity_Type *entity_type = entity_get_type(data);

    i32 x0 = data->x + xm - entity_type->xr;
    i32 y0 = data->y + ym - entity_type->yr;
    i32 x1 = data->x + xm + entity_type->xr - 1;
    i32 y1 = data->y + ym + entity_type->yr - 1;

    i32 xt0 = (x0 >> LEVEL_TILE_SIZE) - 1;
    i32 yt0 = (y0 >> LEVEL_TILE_SIZE) - 1;
    i32 xt1 = (x1 >> LEVEL_TILE_SIZE) + 1;
    i32 yt1 = (y1 >> LEVEL_TILE_SIZE) + 1;

    if(xt0 < 0) xt0 = 0;
    if(yt0 < 0) yt0 = 0;
    if(xt1 >= LEVEL_W) xt1 = LEVEL_W - 1;
    if(yt1 >= LEVEL_H) yt1 = LEVEL_H - 1;

    bool blocked = false;
    for(u32 y = yt0; y <= yt1; y++) {
        for(u32 x = xt0; x <= xt1; x++) {
            const u32 tile = x + y * LEVEL_W;

            for(u32 i = 0; i < LEVEL_SOLID_ENTITIES_IN_TILE; i++) {
                level_EntityID id = level->solid_entities[tile][i];
                if(id >= LEVEL_ENTITY_LIMIT)
                    continue;

                struct entity_Data *data2 = &level->entities[id];
                if(data2 == data)
                    continue;

                if(entity_intersects(data2, x0, y0, x1, y1)) {
                    // if defined, call 'touch_entity'
                    if(entity_type->touch_entity) {
                        bool should_block = entity_type->touch_entity(
                            level, data, data2
                        );

                        if(!should_block)
                            continue;
                    }

                    // if the second entity is NOT already touching the
                    // first one, then the first one is blocked by it.
                    if(!blocked && !entity_touches(data2, data))
                        blocked = true;
                }
            }
        }
    }
    return blocked;
}

IWRAM_SECTION
static bool move2(struct Level *level, struct entity_Data *data,
                  i32 xm, i32 ym) {
    bool success = true;

    if(blocked_by_tiles(level, data, &xm, &ym))
        success = false;

    if(blocked_by_entities(level, data, xm, ym)) {
        success = false;
        xm = ym = 0;
    }

    data->x += xm;
    data->y += ym;
    return success;
}

IWRAM_SECTION
bool entity_move(struct Level *level, struct entity_Data *data,
                 i32 xm, i32 ym) {
    bool success = true;

    if(xm != 0 && !move2(level, data, xm, 0))
        success = false;
    if(ym != 0 && !move2(level, data, 0, ym))
        success = false;

    return success;
}
