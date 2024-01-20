/* Copyright 2023-2024 Vulcalien
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
#include "interrupt.h"

#include "sound.h"
#include "performance.h"

#define IME *((vu32 *) 0x04000208)
#define IE  *((vu16 *) 0x04000200)
#define IF  *((vu16 *) 0x04000202)

#define IF_BIOS *((vu16 *) 0x03007ff8)

#define INTERRUPT_HANDLER *((vu32 *) 0x03007ffc)

// Interrupt flags
#define VBLANK (1 << 0)
#define HBLANK (1 << 1)
#define VCOUNT (1 << 2)

#define TIMER0 (1 << 3)
#define TIMER1 (1 << 4)
#define TIMER2 (1 << 5)
#define TIMER3 (1 << 6)

#define SERIAL (1 << 7)

#define DMA0 (1 << 8)
#define DMA1 (1 << 9)
#define DMA2 (1 << 10)
#define DMA3 (1 << 11)

#define KEYPAD  (1 << 12)
#define GAMEPAK (1 << 13)

IWRAM_SECTION
static void interrupt_handler(void) {
    if(IF & VBLANK) {
        IF_BIOS |= VBLANK;
        IF = VBLANK;

        performance_vblank();
    }

    if(IF & TIMER1) {
        IF = TIMER1;

        sound_timer1_irq();
    }
}

void interrupt_enable(void) {
    INTERRUPT_HANDLER = (u32) &interrupt_handler;

    IE = VBLANK | TIMER1;

    IME = 1;
}
