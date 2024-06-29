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
#include "screen.h"

#include <gba/display.h>
#include <gba/background.h>
#include <gba/sprite.h>

#define WINDOW_IN  *((vu16 *) 0x04000048)
#define WINDOW_OUT *((vu16 *) 0x0400004a)

#define CHAR_BLOCK_0 ((vu16 *) 0x06000000)
#define CHAR_BLOCK_1 ((vu16 *) 0x06004000)
#define CHAR_BLOCK_2 ((vu16 *) 0x06008000)
#define CHAR_BLOCK_3 ((vu16 *) 0x0600c000)

#define OBJ_TILESET ((vu16 *) 0x06010000)

static const struct Background bg_configs[BACKGROUND_COUNT] = {
    // BG0
    {
        .priority = 0,
        .tileset  = 0,
        .tilemap  = 0
    },

    // BG1
    {
        .priority = 1,
        .tileset  = 0,
        .tilemap  = 0
    },

    // BG2
    {
        .priority = 2,
        .tileset  = 0,
        .tilemap  = 0
    },

    // BG3
    {
        .priority = 3,
        .tileset  = 0,
        .tilemap  = 0
    }
};

void screen_init(void) {
    display_config(&(struct Display) {
        .mode = 0,

        .oam_hblank  = 0,
        .obj_mapping = 1,

        .enable_bg0 = 0,
        .enable_bg1 = 0,
        .enable_bg2 = 0,
        .enable_bg3 = 0,
        .enable_obj = 1
    });

    // configure backgrounds
    for(u32 i = 0; i < BACKGROUND_COUNT; i++)
        background_config(i, &bg_configs[i]);

    // TODO ...

    sprite_hide_all();

    // disable forced blank
    display_force_blank(false);
}
