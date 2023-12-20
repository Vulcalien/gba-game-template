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

#define DIRECT_SOUND_ENABLE

extern void sound_init(void);
extern void sound_vblank(void);

#ifdef DIRECT_SOUND_ENABLE
    #define direct_sound_channel_A (0)
    #define direct_sound_channel_B (1)

    // Do not use these two functions outside of the sound driver
    extern void direct_sound_init(void);
    extern void direct_sound_vblank(void);

    #define DIRECT_SOUND_PLAY(sound, channel, loop)\
        direct_sound_play((sound), sizeof(sound), (channel), (loop))
    extern void direct_sound_play(const u8 *sound, u32 length,
                                  bool channel, bool loop);
    extern void direct_sound_stop(bool channel);
#endif // DIRECTSOUND_ENABLE

#endif // VULC_TEMPLATE_SOUND
