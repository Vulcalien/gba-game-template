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
    background_config(BG0, &(struct Background) {
        .priority = 0,
        .tileset  = 0,
        .tilemap  = 0
    });
    background_config(BG1, &(struct Background) {
        .priority = 1,
        .tileset  = 0,
        .tilemap  = 0
    });
    background_config(BG2, &(struct Background) {
        .priority = 2,
        .tileset  = 0,
        .tilemap  = 0
    });
    background_config(BG3, &(struct Background) {
        .priority = 3,
        .tileset  = 0,
        .tilemap  = 0
    });

    // TODO ...

    sprite_hide_all();

    // disable forced blank
    display_force_blank(false);
}
