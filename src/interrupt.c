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

#define IME     *((vu32 *) 0x04000208)
#define IE      *((vu16 *) 0x04000200)
#define IF      *((vu16 *) 0x04000202)
#define IF_BIOS *((vu16 *) 0x03007ff8)

#define INTERRUPT_HANDLER *((vu32 *) 0x03007ffc)

#define INTERRUPT_COUNT (14)

static void (*isr_table[INTERRUPT_COUNT])(void);

IWRAM_SECTION ARM
static void interrupt_handler(void) {
    for(u32 i = 0; i < INTERRUPT_COUNT; i++) {
        u32 irq = 1 << i;
        if(!(IF & irq))
            continue;

        // acknowledge the IRQ
        IF_BIOS |= irq;
        IF = irq;

        // call the ISR
        void (*isr)(void) = isr_table[i];
        if(isr)
            isr();
    }
}

void interrupt_init(void) {
    INTERRUPT_HANDLER = (u32) &interrupt_handler;

    IME = 1;
}

void interrupt_enable(u8 irq) {
    IE |= (1 << irq);
}

void interrupt_disable(u8 irq) {
    IE &= ~(1 << irq);
}

void interrupt_set_isr(u8 irq, void (*isr)(void)) {
    if(irq >= INTERRUPT_COUNT)
        return;

    isr_table[irq] = isr;
}
