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

static void keypad_isr(void) {
    mgba_printf("YEYYY");
}

void AgbMain(void) {
    // enable VBlank interrupt
    interrupt_toggle(IRQ_VBLANK, true);
    interrupt_isr(IRQ_VBLANK, vblank);

    input_init(30, 2);
    audio_init(AUDIO_MIXER);
    backup_init(BACKUP_SRAM);

    i32 x = 0, y = 0;
    i32 other_pos[4][2];

    /*screen_init();*/
    scene_set(&scene_start, 0);

    timer_config(TIMER3, TIMER_PRESCALER_1024);
    timer_start(TIMER3, SYSTEM_FREQUENCY / 1024);
    interrupt_toggle(IRQ_TIMER3, true);
    interrupt_isr(IRQ_TIMER3, timer3_isr);

    linkcable_init();
    /*linkcable_speed(LINKCABLE_BITRATE_MAX);*/

    display_config(3);
    display_blank(false);

    interrupt_config(IRQ_KEYPAD, KEY_L | BIT(15));
    interrupt_isr(IRQ_KEYPAD, keypad_isr);
    interrupt_toggle(IRQ_KEYPAD, true);
    while(true) {
        if(input_press(KEY_SELECT)) {
            static bool enable = 0;
            enable ^= 1;

            mgba_printf("interrupt enable: %d", enable);
            vu16 *ie = (vu16 *) 0x04000200;
            if(enable)
                *ie |= BIT(IRQ_KEYPAD);
            else
                *ie &= ~BIT(IRQ_KEYPAD);

            /*interrupt_toggle(IRQ_KEYPAD, enable);*/
        }

        tick();

        /*if(should_print) {*/
            /*mgba_printf("received %d bits", to_print_received);*/
            /*should_print = false;*/
        /*}*/

        if(input_down(KEY_LEFT) - input_down(KEY_RIGHT) || input_down(KEY_UP) - input_down(KEY_DOWN)) {
            x -= input_down(KEY_LEFT) - input_down(KEY_RIGHT);
            y -= input_down(KEY_UP) - input_down(KEY_DOWN);

            u8 outbuf[] = {
                input_down(KEY_LEFT)  << 0 |
                input_down(KEY_RIGHT) << 1 |
                input_down(KEY_UP)    << 2 |
                input_down(KEY_DOWN)  << 3
            };
            linkcable_broadcast(&outbuf, sizeof(outbuf));
        }

        interrupt_wait(IRQ_VBLANK);
        memory_clear(display_raster(0), DISPLAY_WIDTH * DISPLAY_HEIGHT * 2);
        display_raster(0)[x + y * 240] = 0x7fff;

        u8 inbuf;
        i32 src;
        while(linkcable_receive(&src, &inbuf, 1)) {
            other_pos[src][0] -= (inbuf & 1) - ((inbuf >> 1) & 1);
            other_pos[src][1] -= ((inbuf >> 2) & 1) - ((inbuf >> 3) & 1);
        }

        for(i32 i = 0; i < 4; i++) {
            display_raster(0)[other_pos[i][0] + other_pos[i][1] * 240] = 0x001f;
        }

        static u16 out = 1;
        if(input_repeat(KEY_A)) {
            /*out++;*/
            /*linkcable_send(-1, &out, 4);*/
        }

        if(input_repeat(KEY_START)) {
            static i32 v = 0;
            v ^= 1;
            u32 speed = v ? 1024 : 4096;
            mgba_printf("new speed: %d bps", speed);
            linkcable_speed(speed);
        }

        /*u16 in[16];*/
        /*i32 r;*/
        /*while((r = linkcable_receive(NULL, &in, 32)))*/
            /*received_bytes += r;*/

        /*interrupt_wait(IRQ_VBLANK);*/
        /*draw();*/
    }
}
