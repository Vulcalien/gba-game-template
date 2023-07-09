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
#include "level.h"

#include "entity.h"

static inline void tick_tiles(struct Level *level) {
    // ...
}

static inline void insert_solid_entity(struct Level *level,
                                       struct entity_Data *data,
                                       u32 entity_id,
                                       i32 x, i32 y) {
    if(x < 0 || y < 0 || x >= LEVEL_W || y >= LEVEL_H)
        return;

    const u32 tile = x + y * LEVEL_W;
    for(u32 i = 0; i < LEVEL_SOLID_ENTITIES_IN_TILE; i++) {
        if(level->solid_entities[tile][i] >= ENTITY_TYPES) {
            level->solid_entities[tile][i] = entity_id;
            data->solid_id = i;

            break;
        }
    }
}

static inline void remove_solid_entity(struct Level *level,
                                       struct entity_Data *data,
                                       u32 entity_id,
                                       i32 x, i32 y) {
    if(x < 0 || y < 0 || x >= LEVEL_W || y >= LEVEL_H)
        return;

    const u32 tile = x + y * LEVEL_W;
    if(level->solid_entities[tile][data->solid_id] == entity_id)
        level->solid_entities[tile][data->solid_id] = ENTITY_INVALID;
}

static inline void tick_entities(struct Level *level) {
    for(u32 i = 0; i < LEVEL_ENTITY_LIMIT; i++) {
        struct entity_Data *data = &level->entities[i];
        if(data->type >= ENTITY_TYPES)
            continue;

        i32 xt0 = data->x >> LEVEL_TILE_SIZE;
        i32 yt0 = data->y >> LEVEL_TILE_SIZE;

        const struct Entity *entity = entity_type(data);
        entity->tick(level, data);

        if(data->should_remove) {
            if(entity->is_solid)
                remove_solid_entity(level, data, i, xt0, yt0);

            data->type = ENTITY_INVALID;
        } else if(entity->is_solid) {
            i32 xt1 = data->x >> LEVEL_TILE_SIZE;
            i32 yt1 = data->y >> LEVEL_TILE_SIZE;

            if(xt1 != xt0 || yt1 != yt0) {
                remove_solid_entity(level, data, i, xt0, yt0);
                insert_solid_entity(level, data, i, xt1, yt1);
            }
        }
    }
}

IWRAM_SECTION
void level_tick(struct Level *level) {
    tick_tiles(level);
    tick_entities(level);
}

IWRAM_SECTION
void level_draw(struct Level *level) {
    // ...
}
