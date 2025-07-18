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
#include "performance.h"

#define PRINT_TO_MGBA

static u16 tick_vcount;
static u16 draw_vcount;

static u16 ticks = 0, frames = 0;
static u16 tps   = 0, fps    = 0;

static bool show_performance = false;
static bool should_refresh = false;

void performance_tick(void) {
    tick_vcount = display_vcount();
    ticks++;
}

void performance_draw(void) {
    draw_vcount = display_vcount();
    frames++;

    if(!should_refresh)
        return;
    should_refresh = false;

    #ifdef PRINT_TO_MGBA
    mgba_open();
    mgba_printf(
        "tps %u - fps %u - tick_vcount %x - draw_vcount %x",
        tps, fps, tick_vcount, draw_vcount
    );
    #endif
}

IWRAM_SECTION
void performance_vblank(void) {
    static u32 vblanks = 0;
    vblanks++;

    if(vblanks == 60) {
        vblanks = 0;

        tps = ticks;
        fps = frames;

        ticks = 0;
        frames = 0;

        should_refresh = show_performance;
    }
}
