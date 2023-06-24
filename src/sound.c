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
#include "sound.h"

#define SOUND_CONTROL_H *((vu16 *) 0x04000082)
#define SOUND_CONTROL_X *((vu16 *) 0x04000084)

#define FIFO_A (0x040000a0)
#define FIFO_B (0x040000a4)

// DMA
#define DMA1_SOURCE  *((vu32 *) 0x040000bc)
#define DMA1_DEST    *((vu32 *) 0x040000c0)
#define DMA1_CONTROL *((vu16 *) 0x040000c6)

#define DMA2_SOURCE  *((vu32 *) 0x040000c8)
#define DMA2_DEST    *((vu32 *) 0x040000cc)
#define DMA2_CONTROL *((vu16 *) 0x040000d2)

// Timers
#define TIMER0_RELOAD  *((vu16 *) 0x04000100)
#define TIMER0_CONTROL *((vu16 *) 0x04000102)

#define TIMER1_RELOAD  *((vu16 *) 0x04000104)
#define TIMER1_CONTROL *((vu16 *) 0x04000106)

static struct {
    const u8 *sound;
    u32 vblanks;
    u32 length;
    bool loop;
} a_info, b_info;

void sound_init(void) {
    SOUND_CONTROL_X = 1 << 7; // Enable FIFO Channels

    SOUND_CONTROL_H = 1 << 2  | // Channel A Volume (1 is 100%)
                      1 << 3  | // Channel B Volume (1 is 100%)
                      1 << 8  | // Enable Channel A RIGHT
                      1 << 9  | // Enable Channel A LEFT
                      0 << 10 | // Channel A Timer (0 is Timer 0)
                      1 << 12 | // Enable Channel B RIGHT
                      1 << 13 | // Enable Channel B LEFT
                      1 << 14;  // Channel B Timer (1 is Timer 1)
}

#define SAMPLE_RATE (16 * 1024)

#define CLOCK_FREQUENCY (16 * 1024 * 1024)
#define CYCLES_PER_SAMPLE (CLOCK_FREQUENCY / SAMPLE_RATE)

void sound_play(const u8 *sound, u32 length,
                enum sound_Channel channel, bool loop) {
    const u16 dma_value = 2 << 5  | // Dest Address Control (2 is Fixed)
                          1 << 9  | // DMA repeat
                          1 << 10 | // Transfer type (1 is 32bit)
                          3 << 12 | // Start Timing (3 is Sound FIFO)
                          1 << 15;  // DMA Enable

    const u16 feed_timer_value = 1 << 7; // Timer Start

    // vblanks = length * 59.7275 / SAMPLE_RATE
    //         = length * 59.7275 * 11 / (SAMPLE_RATE * 11)
    //        ~= length * 657 / (SAMPLE_RATE * 11)
    u32 vblanks = length * 657 / (SAMPLE_RATE * 11);
    if(vblanks == 0)
        return;

    sound_stop(channel);
    if(channel == sound_channel_A) {
        DMA1_SOURCE = (u32) sound;
        DMA1_DEST = FIFO_A;
        DMA1_CONTROL = dma_value;

        TIMER0_RELOAD = 65536 - CYCLES_PER_SAMPLE;
        TIMER0_CONTROL = feed_timer_value;

        a_info.sound = sound;
        a_info.vblanks = vblanks;
        a_info.length = length;
        a_info.loop = loop;
    } else {
        DMA2_SOURCE = (u32) sound;
        DMA2_DEST = FIFO_B;
        DMA2_CONTROL = dma_value;

        TIMER1_RELOAD = 65536 - CYCLES_PER_SAMPLE;
        TIMER1_CONTROL = feed_timer_value;

        b_info.sound = sound;
        b_info.vblanks = vblanks;
        b_info.length = length;
        b_info.loop = loop;
    }
}

void sound_stop(enum sound_Channel channel) {
    if(channel == sound_channel_A) {
        TIMER0_CONTROL = 0;
        DMA1_CONTROL   = 0;

        a_info.vblanks = 0;
    } else {
        TIMER1_CONTROL = 0;
        DMA2_CONTROL   = 0;

        b_info.vblanks = 0;
    }
}

IWRAM_SECTION
void sound_vblank(void) {
    if(a_info.vblanks > 0) {
        a_info.vblanks--;
        if(a_info.vblanks == 0) {
            sound_stop(sound_channel_A);
            if(a_info.loop) {
                sound_play(
                    a_info.sound, a_info.length,
                    sound_channel_A, true
                );
            }
        }
    }

    if(b_info.vblanks > 0) {
        b_info.vblanks--;
        if(b_info.vblanks == 0) {
            sound_stop(sound_channel_B);
            if(b_info.loop) {
                sound_play(
                    b_info.sound, b_info.length,
                    sound_channel_B, true
                );
            }
        }
    }
}
