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

#include <gba/interrupt.h>
#include <gba/backup.h>
#include <gba/audio.h>
#include <gba/input.h>

#include "screen.h"
#include "performance.h"
#include "scene.h"

#include <debug/profiler.h>
#include <debug/mgba.h>
#include <gba/system.h>
#include <math.h>

SBSS_SECTION
i8 sinewave[16384];

static inline void tick(void) {
    static u32 i = 0;
    static u32 time = 0;

    profiler_start();
    audio_update();
    time += profiler_stop();

    i++;
    if(i == 1) {
        /*mgba_printf(*/
            /*"%u\t%u.%u%%",*/
            /*time,*/
            /*time * 100 / (SYSTEM_FREQUENCY / 3),*/
            /*time * 1000 / (SYSTEM_FREQUENCY / 3) % 10*/
        /*);*/
        if(time > 300) {
            mgba_printf(
                "%u\t%u.%u", time,
                    time * 2133 / SYSTEM_FREQUENCY,
                    time * 1667 / (128*1024) % 100
                );
        }
        i = 0;
        time = 0;
    }

    input_update();
    scene->tick();

    static u32 pitch = 0x1000;
    static u32 panning = 0;
    if(input_pressed(KEY_A)) {
        audio_pause(-1);
        /*pitch += 0x100;*/
        /*panning += 1;*/
    }
    if(input_pressed(KEY_B)) {
        audio_resume(-1);
        /*pitch -= 0x100;*/
        /*panning -= 1;*/
    }

    audio_volume(-1, 32);
    audio_pitch(-1, pitch);
    audio_panning(-1, panning);

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

#include "res/song.c"

int AgbMain(void) {
    // enable VBlank interrupt
    interrupt_toggle(IRQ_VBLANK, true);
    interrupt_set_isr(IRQ_VBLANK, vblank);

    for(u32 i = 0; i < sizeof(sinewave); i++) {
        sinewave[i] = math_sin(i * 4 * 300) / 129;
    }

    mgba_open();

    backup_init(BACKUP_SRAM);
    audio_init(AUDIO_MIXER);
    screen_init();

    for(u32 i = 0; i < 8; i++) {
        audio_play(i, sinewave, sizeof(sinewave));
        audio_loop(i, sizeof(sinewave));
        for(u32 i = 0; i < 20; i++) {
            /*audio_update();*/
            /*vsync();*/
        }
    }

    /*audio_panning(0, AUDIO_PANNING_MIN);*/
    /*audio_play(0, song, sizeof(song));*/
    /*audio_loop(0, sizeof(song));*/

    /*audio_panning(-1, +50);*/
    /*audio_play(1, song, sizeof(song));*/
    /*audio_loop(1, sizeof(song));*/

    audio_panning(-1, AUDIO_PANNING_MAX);
    /*audio_loop(-1, sizeof(song));*/

    scene_set(&scene_start, 0);
    while(true) {
        tick();

        vsync();
        draw();
    }
    return 0;
}
