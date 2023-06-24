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
#ifndef VULC_TEMPLATE_SOUND
#define VULC_TEMPLATE_SOUND

#include "main.h"

extern void sound_init(void);

enum sound_Channel {
    sound_channel_A,
    sound_channel_B
};

#define SOUND_PLAY(sound, channel, loop)\
    sound_play((sound), sizeof(sound), (channel), (loop))
extern void sound_play(const u8 *sound, u32 length,
                       enum sound_Channel channel, bool loop);
extern void sound_stop(enum sound_Channel channel);

extern void sound_vblank(void);

#endif // VULC_TEMPLATE_SOUND
