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

enum EntityTypeID {
    ENTITY_PLAYER,

    ENTITY_INVALID
};
#define ENTITY_TYPES (ENTITY_INVALID)

// If specific entity types need bytes to store their data, change this
// value. The 'data' array can then be cast into whatever struct of the
// same size.
//
// This will increase the size of all entities, no matter their type. If
// this is not acceptable, only store an array index in 'data' and put
// the necessary data into that array.
#define ENTITY_EXTRA_DATA_SIZE (0)

struct EntityData {
    enum EntityTypeID type;

    u8 should_remove : 1;
    u8 solid_id : 7;

    i32 x;
    i32 y;

    #if ENTITY_EXTRA_DATA_SIZE > 0
        ALIGNED(4)
        u8 data[ENTITY_EXTRA_DATA_SIZE];
    #endif
};

struct Level;
struct EntityType {
    // entity radius (width and height)
    u8 xr;
    u8 yr;

    bool is_solid;

    void (*tick)(struct Level *level, struct EntityData *data);

    // returns how many sprites were used
    u32 (*draw)(struct Level *level, struct EntityData *data,
                i32 x, i32 y, u32 used_sprites);

    // If defined, called when this entity (data) is moving and touches
    // another entity (touched_data). Returns 'true' if this entity
    // should be blocked by the touched entity, 'false' otherwise.
    bool (*touch_entity)(struct Level *level, struct EntityData *data,
                         struct EntityData *touched_data);
};

extern const struct EntityType * const entity_type_list[ENTITY_TYPES];

INLINE bool entity_is_valid(struct EntityData *data) {
    return (data->type >= 0 && data->type < ENTITY_TYPES);
}

INLINE
const struct EntityType *entity_get_type(struct EntityData *data) {
    if(entity_is_valid(data))
        return entity_type_list[data->type];
    return NULL;
}

// returns 'true' if the entity moved by exactly (xm, ym)
extern bool entity_move(struct Level *level, struct EntityData *data,
                        i32 xm, i32 ym);

INLINE bool entity_intersects(struct EntityData *data,
                              i32 x0, i32 y0, i32 x1, i32 y1) {
    const struct EntityType *entity_type = entity_get_type(data);

    return (data->x + entity_type->xr - 1 >= x0) &&
           (data->y + entity_type->yr - 1 >= y0) &&
           (data->x - entity_type->xr     <= x1) &&
           (data->y - entity_type->yr     <= y1);
}

INLINE bool entity_touches(struct EntityData *data1,
                           struct EntityData *data2) {
    const struct EntityType *e2_type = entity_get_type(data2);

    return entity_intersects(
        data1,
        data2->x - e2_type->xr,     data2->y - e2_type->yr,
        data2->x + e2_type->xr - 1, data2->y + e2_type->yr - 1
    );
}

// Entity types
extern const struct EntityType
    entity_player;
