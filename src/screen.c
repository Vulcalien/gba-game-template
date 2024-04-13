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
#include "screen.h"

#include "sprite.h"

#define DISPLAY_CONTROL *((vu16 *) 0x04000000)
#define DISPLAY_STATUS  *((vu16 *) 0x04000004)

#define WINDOW_IN  *((vu16 *) 0x04000048)
#define WINDOW_OUT *((vu16 *) 0x0400004a)

#define CHAR_BLOCK_0 ((vu16 *) 0x06000000)
#define CHAR_BLOCK_1 ((vu16 *) 0x06004000)
#define CHAR_BLOCK_2 ((vu16 *) 0x06008000)
#define CHAR_BLOCK_3 ((vu16 *) 0x0600c000)

#define OBJ_TILESET ((vu16 *) 0x06010000)

#define BG0_CONTROL *((vu16 *) 0x04000008)
#define BG1_CONTROL *((vu16 *) 0x0400000a)
#define BG2_CONTROL *((vu16 *) 0x0400000c)
#define BG3_CONTROL *((vu16 *) 0x0400000e)

#define BG_PALETTE  ((vu16 *) 0x05000000)
#define OBJ_PALETTE ((vu16 *) 0x05000200)

void screen_init(void) {
    DISPLAY_CONTROL = 0 << 0  | // Video mode
                      1 << 6  | // OBJ character mapping (1 = linear)
                      1 << 7  | // Forced blank
                      0 << 8  | // Enable BG 0
                      0 << 9  | // Enable BG 1
                      0 << 10 | // Enable BG 2
                      0 << 11 | // Enable BG 3
                      1 << 12;  // Enable OBJs

    BG0_CONTROL = 0 << 0 | // BG Priority (0 = highest, 3 = lowest)
                  0 << 2 | // Tileset character block (=16K)
                  0 << 8;  // Tilemap screen block (=2K)

    BG1_CONTROL = 1 << 0 | // BG Priority (0 = highest, 3 = lowest)
                  0 << 2 | // Tileset character block (=16K)
                  0 << 8;  // Tilemap screen block (=2K)

    BG2_CONTROL = 2 << 0 | // BG Priority (0 = highest, 3 = lowest)
                  0 << 2 | // Tileset character block (=16K)
                  0 << 8;  // Tilemap screen block (=2K)

    BG3_CONTROL = 3 << 0 | // BG Priority (0 = highest, 3 = lowest)
                  0 << 2 | // Tileset character block (=16K)
                  0 << 8;  // Tilemap screen block (=2K)

    // TODO ...

    // hide all sprites
    for(u32 i = 0; i < 128; i++)
        sprite_hide(i);

    // enable V-Blank IRQ
    DISPLAY_STATUS = (1 << 3);

    // disable forced blank
    DISPLAY_CONTROL &= ~(1 << 7);
}
