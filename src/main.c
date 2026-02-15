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
#include "main.h"

#include "screen.h"
#include "performance.h"
#include "scene.h"

static inline void tick(void) {
    audio_update();
    input_update();
    scene->tick();

    performance_tick();
}

static inline void draw(void) {
    scene->draw();

    performance_draw();
}

IWRAM_SECTION
static void vblank(void) {
    performance_vblank();
}

void AgbMain(void) {
    // enable VBlank interrupt
    interrupt_toggle(IRQ_VBLANK, true);
    interrupt_isr(IRQ_VBLANK, vblank);

    backup_init(BACKUP_SRAM);
    audio_init(AUDIO_BASIC);
    screen_init();

    scene_set(&scene_start, 0);

    while(true) {
        tick();

        interrupt_wait(IRQ_VBLANK);
        draw();
    }
}
