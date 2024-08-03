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

const struct EntityType * const entity_type_list[ENTITY_TYPES] = {
    [ENTITY_PLAYER] = &entity_player
};

static INLINE bool tile_blocks(struct Level *level, i32 x, i32 y,
                               struct EntityData *data) {
    const struct TileType *tile_type = tile_get_type(
        level_get_tile(level, x, y)
    );

    // invalid tiles (e.g. those outside the level) block entities
    if(!tile_type)
        return true;

    return tile_type->is_solid;
}

static inline bool blocked_by_tiles(struct Level *level,
                                    struct EntityData *data,
                                    i32 * const xm, i32 * const ym) {
    const struct EntityType *e_type = entity_get_type(data);

    // old corners
    const i32 old_x0 = data->x - e_type->xr;
    const i32 old_y0 = data->y - e_type->yr;
    const i32 old_x1 = data->x + e_type->xr - 1;
    const i32 old_y1 = data->y + e_type->yr - 1;

    // old tile corners
    const i32 old_xt0 = old_x0 >> LEVEL_TILE_SIZE;
    const i32 old_yt0 = old_y0 >> LEVEL_TILE_SIZE;
    const i32 old_xt1 = old_x1 >> LEVEL_TILE_SIZE;
    const i32 old_yt1 = old_y1 >> LEVEL_TILE_SIZE;

    // new tile corners
    const i32 new_xt0 = (old_x0 + *xm) >> LEVEL_TILE_SIZE;
    const i32 new_yt0 = (old_y0 + *ym) >> LEVEL_TILE_SIZE;
    const i32 new_xt1 = (old_x1 + *xm) >> LEVEL_TILE_SIZE;
    const i32 new_yt1 = (old_y1 + *ym) >> LEVEL_TILE_SIZE;

    // 'a' is the axis the player is moving along
    i32 step, first, last;
    if(*xm != 0) {
        step  = (*xm < 0 ?      -1 : +1     );
        first = (*xm < 0 ? old_xt0 : old_xt1);
        last  = (*xm < 0 ? new_xt0 : new_xt1);
    } else {
        step  = (*ym < 0 ?      -1 : +1     );
        first = (*ym < 0 ? old_yt0 : old_yt1);
        last  = (*ym < 0 ? new_yt0 : new_yt1);
    }

    // 'b' is the axis the entity is not moving along
    const i32 b0 = (*xm != 0 ? old_yt0 : old_xt0);
    const i32 b1 = (*xm != 0 ? old_yt1 : old_xt1);

    // ignore tiles the entity is already in
    first += step;

    // iterate tiles from closest ('first') to farthest ('last') along
    // the 'a' axis
    for(i32 a = first; a != last + step; a += step) {
        for(i32 b = b0; b <= b1; b++) {
            // change coordinate system from (a, b) to (x, y)
            const i32 x = (*xm != 0 ? a : b);
            const i32 y = (*xm != 0 ? b : a);

            if(tile_blocks(level, x, y, data)) {
                // decrease xm or ym
                if(*xm < 0) // left
                    *xm = ((a + 1) << LEVEL_TILE_SIZE) - old_x0;
                else if(*xm > 0) // right
                    *xm = (a << LEVEL_TILE_SIZE) - 1 - old_x1;
                else if(*ym < 0) // up
                    *ym = ((a + 1) << LEVEL_TILE_SIZE) - old_y0;
                else if(*ym > 0) // down
                    *ym = (a << LEVEL_TILE_SIZE) - 1 - old_y1;

                return true;
            }
        }
    }
    return false;
}

static inline bool blocked_by_entities(struct Level *level,
                                       struct EntityData *data,
                                       i32 xm, i32 ym) {
    const struct EntityType *type = entity_get_type(data);

    i32 x0 = data->x + xm - type->xr;
    i32 y0 = data->y + ym - type->yr;
    i32 x1 = data->x + xm + type->xr - 1;
    i32 y1 = data->y + ym + type->yr - 1;

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
                LevelEntityID id = level->solid_entities[tile][i];
                if(id >= LEVEL_ENTITY_LIMIT)
                    continue;

                struct EntityData *touched_data = &level->entities[id];
                if(touched_data == data)
                    continue;

                if(entity_intersects(touched_data, x0, y0, x1, y1)) {
                    // by default, solid entities block movement
                    bool should_block = true;

                    // if defined, call 'touch_entity' event
                    if(type->touch_entity)
                        if(!type->touch_entity(level, data, touched_data))
                            should_block = false;

                    if(!should_block)
                        continue;

                    // if the second entity is NOT already touching the
                    // first one, then the first one is blocked by it.
                    if(!blocked && !entity_touches(touched_data, data))
                        blocked = true;
                }
            }
        }
    }
    return blocked;
}

IWRAM_SECTION
static bool move2(struct Level *level, struct EntityData *data,
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
bool entity_move(struct Level *level, struct EntityData *data,
                 i32 xm, i32 ym) {
    bool success = true;

    if(xm != 0 && !move2(level, data, xm, 0))
        success = false;
    if(ym != 0 && !move2(level, data, 0, ym))
        success = false;

    return success;
}
