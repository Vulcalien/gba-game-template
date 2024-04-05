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
#pragma once

#include "main.h"

#define ENTITY_TYPES (0)

#define ENTITY_INVALID (-1)

// If specific entity types need bytes to store their data, change this
// value. The 'data' array can then be cast into whatever struct of the
// same size.
//
// This will increase the size of all entities, no matter their type. If
// this is not acceptable, only store an array index in 'data' and put
// the necessary data into that array.
#define ENTITY_EXTRA_DATA_SIZE (0)

struct entity_Data {
    u8 type;

    i32 x;
    i32 y;

    u8 should_remove : 1;
    u8 solid_id : 7;

    #if ENTITY_EXTRA_DATA_SIZE > 0
        u8 data[ENTITY_EXTRA_DATA_SIZE];
    #endif
};

struct Level;
struct entity_Type {
    // entity radius (width and height)
    u8 xr;
    u8 yr;

    bool is_solid;

    void (*tick)(struct Level *level, struct entity_Data *data);

    // returns how many sprites were used
    u32 (*draw)(struct Level *level, struct entity_Data *data,
                u32 used_sprites);
};

extern const struct entity_Type *entity_type_list[ENTITY_TYPES];

ALWAYS_INLINE
inline bool entity_is_valid(struct entity_Data *data) {
    return (data->type >= 0 && data->type < ENTITY_TYPES);
}

ALWAYS_INLINE
inline
const struct entity_Type *entity_get_type(struct entity_Data *data) {
    if(entity_is_valid(data))
        return entity_type_list[data->type];
    return NULL;
}

// returns 'true' if the entity was moved by (xm, ym)
extern bool entity_move(struct Level *level, struct entity_Data *data,
                        i32 xm, i32 ym);

ALWAYS_INLINE
inline bool entity_intersects(struct entity_Data *data,
                              i32 x0, i32 y0, i32 x1, i32 y1) {
    const struct entity_Type *entity_type = entity_get_type(data);

    return (data->x + entity_type->xr - 1 >= x0) &&
           (data->y + entity_type->yr - 1 >= y0) &&
           (data->x - entity_type->xr     <= x1) &&
           (data->y - entity_type->yr     <= y1);
}

ALWAYS_INLINE
inline bool entity_touches(struct entity_Data *data1,
                           struct entity_Data *data2) {
    const struct entity_Type *e2_type = entity_get_type(data1);

    return entity_intersects(
        data1,
        data2->x - e2_type->xr,     data2->y - e2_type->yr,
        data2->x + e2_type->xr - 1, data2->y + e2_type->yr - 1
    );
}

// Entity types
//extern const struct entity_Type
    // ...
