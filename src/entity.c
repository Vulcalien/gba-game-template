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
#include "entity.h"

#include "level.h"
#include "tile.h"

#ifdef ENTITY_ENABLE

const struct Entity *entity_list[ENTITY_TYPES] = {
    // ...
};

#ifdef TILE_ENABLE
static inline bool blocked_by_tiles(struct Level *level,
                                    struct entity_Data *data,
                                    i32 xm, i32 ym) {
    const struct Entity *entity = entity_type(data);

    i32 xto0 = (data->x - entity->xr)     >> LEVEL_TILE_SIZE;
    i32 yto0 = (data->y - entity->yr)     >> LEVEL_TILE_SIZE;
    i32 xto1 = (data->x + entity->xr - 1) >> LEVEL_TILE_SIZE;
    i32 yto1 = (data->y + entity->yr - 1) >> LEVEL_TILE_SIZE;

    i32 xt0 = (data->x + xm - entity->xr)     >> LEVEL_TILE_SIZE;
    i32 yt0 = (data->y + ym - entity->yr)     >> LEVEL_TILE_SIZE;
    i32 xt1 = (data->x + xm + entity->xr - 1) >> LEVEL_TILE_SIZE;
    i32 yt1 = (data->y + ym + entity->yr - 1) >> LEVEL_TILE_SIZE;

    for(i32 y = yt0; y <= yt1; y++) {
        for(i32 x = xt0; x <= xt1; x++) {
            if(x >= xto0 && x <= xto1 && y >= yto0 && y <= yto1)
                continue;

            const struct Tile *tile = tile_type(
                level_get_tile(level, x, y)
            );

            if(!tile || tile->is_solid)
                return true;
        }
    }
    return false;
}
#endif // TILE_ENABLE

IWRAM_SECTION
static bool move2(struct Level *level, struct entity_Data *data,
                  i32 xm, i32 ym) {
    #ifdef TILE_ENABLE
        if(blocked_by_tiles(level, data, xm, ym))
            return false;
    #endif

    #ifdef ENTITY_ENABLE
        /*if(blocked_by_entities(level, data, xm, ym))*/
            /*return false;*/
    #endif

    data->x += xm;
    data->y += ym;
    return true;
}

IWRAM_SECTION
bool entity_move(struct Level *level, struct entity_Data *data,
                 i32 xm, i32 ym) {
    if(xm == 0 && ym == 0)
        return true;

    bool stopped = true;
    if(xm != 0 && move2(level, data, xm, 0))
        stopped = false;
    if(ym != 0 && move2(level, data, 0, ym))
        stopped = false;

    return !stopped;
}

#endif // ENTITY_ENABLE
