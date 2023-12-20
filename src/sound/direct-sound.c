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

#ifdef DIRECT_SOUND_ENABLE

#define DIRECT_SOUND_CONTROL *((vu16 *) 0x04000082)

#define FIFO_A ((vu32 *) 0x040000a0)
#define FIFO_B ((vu32 *) 0x040000a4)

// DMA
#define DMA1_SOURCE  ((vu32 *) 0x040000bc)
#define DMA1_DEST    ((vu32 *) 0x040000c0)
#define DMA1_CONTROL ((vu16 *) 0x040000c6)

#define DMA2_SOURCE  ((vu32 *) 0x040000c8)
#define DMA2_DEST    ((vu32 *) 0x040000cc)
#define DMA2_CONTROL ((vu16 *) 0x040000d2)

// Timers
#define TIMER0_RELOAD  ((vu16 *) 0x04000100)
#define TIMER0_CONTROL ((vu16 *) 0x04000102)

#define TIMER1_RELOAD  ((vu16 *) 0x04000104)
#define TIMER1_CONTROL ((vu16 *) 0x04000106)

static const struct Channel {
    vu32 *fifo;

    struct {
        vu32 *source;
        vu32 *dest;
        vu16 *control;
    } dma;

    struct {
        vu16 *reload;
        vu16 *control;
    } timer;
} channels[2] = {
    // Channel A
    {
        .fifo = FIFO_A,
        .dma = { DMA1_SOURCE, DMA1_DEST, DMA1_CONTROL },
        .timer = { TIMER0_RELOAD, TIMER0_CONTROL }
    },

    // Channel B
    {
        .fifo = FIFO_B,
        .dma = { DMA2_SOURCE, DMA2_DEST, DMA2_CONTROL },
        .timer = { TIMER1_RELOAD, TIMER1_CONTROL }
    }
};

static struct SoundData {
    const u8 *sound;
    u32 vblanks;
    u32 length;
    bool loop;
} sound_data[2];

void direct_sound_init(void) {
    DIRECT_SOUND_CONTROL = 1 << 2  | // Channel A Volume (1 = 100%)
                           1 << 3  | // Channel B Volume (1 = 100%)
                           1 << 8  | // Enable Channel A RIGHT
                           1 << 9  | // Enable Channel A LEFT
                           0 << 10 | // Channel A Timer (0 = Timer 0)
                           1 << 12 | // Enable Channel B RIGHT
                           1 << 13 | // Enable Channel B LEFT
                           1 << 14;  // Channel B Timer (1 = Timer 1)
}

static inline void channel_vblank(bool channel) {
    struct SoundData *data = &sound_data[channel];

    if(data->vblanks > 0) {
        data->vblanks--;
        if(data->vblanks == 0) {
            direct_sound_stop(channel);
            if(data->loop) {
                direct_sound_play(
                    data->sound, data->length, channel, true
                );
            }
        }
    }
}

IWRAM_SECTION
void direct_sound_vblank(void) {
    channel_vblank(direct_sound_channel_A);
    channel_vblank(direct_sound_channel_B);
}

#define SAMPLE_RATE (16 * 1024)

#define CLOCK_FREQUENCY (16 * 1024 * 1024)
#define CYCLES_PER_SAMPLE (CLOCK_FREQUENCY / SAMPLE_RATE)

void direct_sound_play(const u8 *sound, u32 length,
                       bool channel, bool loop) {
    const u16 dma_control = 2 << 5  | // Dest address control (2 = Fixed)
                            1 << 9  | // DMA repeat
                            1 << 10 | // Transfer type (1 = 32bit)
                            3 << 12 | // Start timing (3 = Sound FIFO)
                            1 << 15;  // DMA enable

    // vblanks = length * 59.7275 / SAMPLE_RATE
    //         = length * 59.7275 * 11 / (SAMPLE_RATE * 11)
    //        ~= length * 657 / (SAMPLE_RATE * 11)
    u32 vblanks = length * 657 / (SAMPLE_RATE * 11);
    if(vblanks == 0)
        return;

    direct_sound_stop(channel);

    const struct Channel *direct_channel = &channels[channel];
    struct SoundData *data = &sound_data[channel];

    *(direct_channel->dma.source)  = (u32) sound;
    *(direct_channel->dma.dest)    = (u32) direct_channel->fifo;
    *(direct_channel->dma.control) = dma_control;

    *(direct_channel->timer.reload) = 65536 - CYCLES_PER_SAMPLE;
    *(direct_channel->timer.control) = 1 << 7; // Timer start

    *data = (struct SoundData) {
        .sound   = sound,
        .vblanks = vblanks,
        .length  = length,
        .loop    = loop
    };
}

void direct_sound_stop(bool channel) {
    const struct Channel *direct_channel = &channels[channel];
    struct SoundData *data = &sound_data[channel];

    *(direct_channel->timer.control) = 0;
    *(direct_channel->dma.control)   = 0;

    data->vblanks = 0;
}

#endif // DIRECT_SOUND_ENABLE
