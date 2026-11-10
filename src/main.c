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

static u32 received_bytes = 0;
static i32 to_print_received;
static bool should_print = false;

static void timer3_isr(void) {
    to_print_received = received_bytes * 8;
    received_bytes = 0;
    should_print = true;
}

void AgbMain(void) {
    // enable VBlank interrupt
    interrupt_toggle(IRQ_VBLANK, true);
    interrupt_isr(IRQ_VBLANK, vblank);

    input_init(30, 2);
    audio_init(AUDIO_MIXER);
    backup_init(BACKUP_SRAM);

    screen_init();
    scene_set(&scene_start, 0);

    timer_config(TIMER3, TIMER_PRESCALER_1024);
    timer_start(TIMER3, SYSTEM_FREQUENCY / 1024);
    interrupt_toggle(IRQ_TIMER3, true);
    interrupt_isr(IRQ_TIMER3, timer3_isr);

    linkcable_init();
    /*linkcable_speed(LINKCABLE_BITRATE_MAX);*/

    while(true) {
        tick();

        if(should_print) {
            mgba_printf("received %d bits", to_print_received);
            should_print = false;
        }

        static u16 out = 1;
        if(input_repeat(KEY_A)) {
            /*out++;*/
            linkcable_send(-1, &out, 4);
        }
        linkcable_send(-1, &out, 32);

        if(input_repeat(KEY_START)) {
            static i32 v = 0;
            v ^= 1;
            linkcable_speed(v ? 1024 : 50000);
        }

        u16 in[16];
        /*i32 src;*/
        /*for(i32 i = 0; i < 5; i++) {*/
            /*i32 val = linkcable_receive(&src, &in, 32);*/
            /*if(val) {*/
                /*display_palette(0)[0] = 0x7fff;*/
                /*mgba_printf("read (%d) from %d = %d", val, src, in[0]);*/
            /*} else {*/
                /*display_palette(0)[0] = 0;*/
            /*}*/
        /*}*/

        i32 r;
        while((r = linkcable_receive(NULL, &in, 32)))
            received_bytes += r;

        interrupt_wait(IRQ_VBLANK);
        draw();
    }
}
