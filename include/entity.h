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
#ifndef VULC_TEMPLATE_ENTITY
#define VULC_TEMPLATE_ENTITY

#include "main.h"

#define ENTITY_TYPES (0)

#if ENTITY_TYPES

// If specific entity types need bytes to store their data, change this
// value. The 'data' array can then be cast into whatever struct of the
// same size.
//
// This will increase the size of all entities, no matter their type. If
// this is not acceptable, only store an array index in 'data' and put
// the necessary data into that array.
#define ENTITY_EXTRA_DATA_SIZE (0)

struct entity_Data {
    i8 type;

    i32 x;
    i32 y;

    #if ENTITY_EXTRA_DATA_SIZE
        u8 data[ENTITY_EXTRA_DATA_SIZE];
    #endif
};

struct Entity {
    // entity radius (width and height)
    u8 xr;
    u8 yr;

    void (*tick)(struct entity_Data *data);

    // returns how many sprites were used
    u32 (*draw)(struct entity_Data *data, u32 used_sprites);
};

extern const struct Entity *entity_list[ENTITY_TYPES];

inline const struct Entity *entity_type(struct entity_Data *data) {
    if(data->type < ENTITY_TYPES)
        return entity_list[data->type];
    return NULL;
}

inline bool entity_intersects(struct entity_Data *data,
                              i32 x0, i32 y0, i32 x1, i32 y1) {
    const struct Entity *entity = entity_type(data);

    return (data->x + entity->xr - 1 >= x0) &&
           (data->y + entity->yr - 1 >= y0) &&
           (data->x - entity->xr     <= x1) &&
           (data->y - entity->yr     <= y1);
}

inline bool entity_touches(struct entity_Data *data1,
                           struct entity_Data *data2) {
    const struct Entity *e2 = entity_type(data1);

    return entity_intersects(
        data1,
        data2->x - e2->xr, data2->y - e2->yr,
        data2->x + e2->xr, data2->y + e2->yr
    );
}

#endif // ENTITY_TYPES

#endif // VULC_TEMPLATE_ENTITY
