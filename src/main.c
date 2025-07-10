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
#include <random.h>

#include <gba/sprite.h>
#include <gba/display.h>
#include <memory.h>

#include "res/song.c"
#include "res/song_1.c"
/*#include "res/anakin.c"*/

SBSS_SECTION
i8 sinewave[16384];

static inline void tick(void) {
    vi32 gh[1];
    memset(gh, 5, 4);
    static u32 i = 0;
    static u32 time = 0;

    input_update();

    #define N 5
    i8 a[N] = {1};
    i8 b[N] = {2};

    profiler_start();
    /*audio_update();*/
    i32 r;
    for(int i = 0; i < 1500; i++)
        r = memory_compare_8(a, b, N);

    static bool c = true;
    if(c) {
        c = false;
        mgba_printf("result: %d", r);
    }
    time += profiler_stop();

    i++;
    if(i == 60) {
        mgba_printf(
            "%u\t%u.%u%%",
            time,
            time * 100 / (SYSTEM_FREQUENCY / 1),
            time * 500 / (SYSTEM_FREQUENCY / 2) % 10
        );
        /*if(time > 300) {*/
            /*mgba_printf(*/
                /*"%u\t%u.%u", time,*/
                    /*time * 2133 / SYSTEM_FREQUENCY,*/
                    /*time * 1667 / (128*1024) % 100*/
                /*);*/
        /*}*/
        i = 0;
        time = 0;
    }

    scene->tick();

    if(input_repeat(KEY_START)) {
        audio_play(0, song, sizeof(song));
    }

    static u32 pitch = 0x1000;
    static u32 panning = 0;
    if(input_press(KEY_A)) {
        /*audio_resume(-1);*/
        pitch += 0x400;
        /*panning += 1;*/
    }
    if(input_press(KEY_B)) {
        /*audio_pause(-1);*/
        pitch -= 0x400;
        /*panning -= 1;*/
    }

    static i32 xz = 0;
    xz++;
    /*panning = math_sin(xz * 3000) * 64 / 0x4000;*/
    /*pitch = 0x1000 + xz / 1;*/
    /*pitch = 0x800;*/

    /*audio_volume(-1, 16);*/
    audio_pitch(-1, pitch);
    /*audio_panning(-1, panning);*/

    performance_tick();
}

static inline void draw(void) {
    /*scene->draw();*/

    memory_set_32(display_charblock(4), 0x11, 32);
    DISPLAY_OBJ_PALETTE[1] = 0x7fff;

    static int x = 0;
    if(input_repeat(KEY_A)) {
        x = (x + 1) % 32;
        sprite_config(0, &(struct Sprite) {
            .x = x * 8,
            .y = 76,
        });
    }

    performance_draw();
}

IWRAM_SECTION
static void vblank(void) {
    /*performance_vblank();*/
    /*audio_update();*/
}

int AgbMain(void) {
    input_init(30, 10);
    audio_init(AUDIO_MIXER);

    // enable VBlank interrupt
    interrupt_toggle(IRQ_VBLANK, true);
    interrupt_set_isr(IRQ_VBLANK, vblank);

    for(u32 i = 0; i < sizeof(sinewave); i++) {
        sinewave[i] = math_sin(i * 4 * 200) / 129;
    }

    mgba_open();

    backup_init(BACKUP_SRAM);
    screen_init();

    /*audio_sample_rate(32768);*/

    for(u32 i = 0; i < 8; i++) {
        /*audio_play(i, sinewave, sizeof(sinewave));*/
        /*audio_loop(i, sizeof(sinewave));*/
        /*audio_play(i, song_1, sizeof(song_1));*/
        /*audio_loop(i, sizeof(song_1));*/
        for(u32 i = 0; i < 20; i++) {
            /*audio_update();*/
            /*vsync();*/
        }
    }

    /*audio_play(0, song, sizeof(song));*/
    /*audio_loop(0, sizeof(song));*/
    /*audio_panning(0, AUDIO_PANNING_MIN);*/

    /*audio_play(1, song_1, sizeof(song_1));*/
    /*audio_loop(1, sizeof(song_1));*/
    /*audio_panning(1, -32);*/

    /*audio_play(2, anakin, sizeof(anakin));*/
    /*audio_loop(2, sizeof(anakin));*/
    /*audio_panning(2, 32);*/

    scene_set(&scene_start, 0);
    while(true) {
        tick();

        interrupt_wait(IRQ_VBLANK);
        draw();
    }
    return 0;
}
