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

void sound_init(void) {
    SOUND_CONTROL_X = 1 << 7; // Enable sound

    SOUND_CONTROL_H = 1 << 2  | // Channel A Volume (1 = 100%)
                      1 << 3  | // Channel B Volume (1 = 100%)
                      1 << 8  | // Enable Channel A RIGHT
                      1 << 9  | // Enable Channel A LEFT
                      0 << 10 | // Channel A Timer (0 = Timer 0)
                      1 << 12 | // Enable Channel B RIGHT
                      1 << 13 | // Enable Channel B LEFT
                      1 << 14;  // Channel B Timer (1 = Timer 1)
}

// ===== DIRECT SOUND =====

struct DirectChannel {
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
};

static struct SoundData {
    const u8 *sound;
    u32 vblanks;
    u32 length;
    bool loop;
} sound_data[2];

static const struct DirectChannel channels[2] = {
    // Direct Channel A
    {
        .fifo = FIFO_A,
        .dma = {
            .source  = DMA1_SOURCE,
            .dest    = DMA1_DEST,
            .control = DMA1_CONTROL
        },
        .timer = {
            .reload  = TIMER0_RELOAD,
            .control = TIMER0_CONTROL
        }
    },

    // Direct Channel B
    {
        .fifo = FIFO_B,
        .dma = {
            .source  = DMA2_SOURCE,
            .dest    = DMA2_DEST,
            .control = DMA2_CONTROL
        },
        .timer = {
            .reload  = TIMER1_RELOAD,
            .control = TIMER1_CONTROL
        }
    }
};

#define SAMPLE_RATE (16 * 1024)

#define CLOCK_FREQUENCY (16 * 1024 * 1024)
#define CYCLES_PER_SAMPLE (CLOCK_FREQUENCY / SAMPLE_RATE)

void sound_play(const u8 *sound, u32 length,
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

    sound_stop(channel);

    const struct DirectChannel *direct_channel = &channels[channel];
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

void sound_stop(bool channel) {
    const struct DirectChannel *direct_channel = &channels[channel];
    struct SoundData *data = &sound_data[channel];

    *(direct_channel->timer.control) = 0;
    *(direct_channel->dma.control)   = 0;

    data->vblanks = 0;
}

static inline void channel_vblank(bool channel) {
    struct SoundData *data = &sound_data[channel];

    if(data->vblanks > 0) {
        data->vblanks--;
        if(data->vblanks == 0) {
            sound_stop(channel);
            if(data->loop)
                sound_play(data->sound, data->length, channel, true);
        }
    }
}

// ===== ===== =====

IWRAM_SECTION
void sound_vblank(void) {
    // direct channels
    channel_vblank(sound_channel_A);
    channel_vblank(sound_channel_B);
}
