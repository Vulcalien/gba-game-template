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
#include "interrupt.h"
#include "sound.h"
#include "input.h"
#include "performance.h"
#include "scene.h"

static inline void tick(void) {
    input_tick();
    scene->tick();

    performance_tick();
}

static inline void draw(void) {
    scene->draw();

    performance_draw();
}

int AgbMain(void) {
    screen_init();
    SCENE_SET(&scene_start);

    interrupt_enable();
    sound_init();

    while(true) {
        tick();

        vsync();
        draw();
    }
    return 0;
}
