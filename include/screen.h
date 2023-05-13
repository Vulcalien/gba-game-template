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
#ifndef VULC_TEMPLATE_SCREEN
#define VULC_TEMPLATE_SCREEN

#include "main.h"

#define SCREEN_W (240)
#define SCREEN_H (160)

#define VCOUNT *((vu16 *) 0x04000006)

#define BG0_XOFFSET *((vu16 *) 0x04000010)
#define BG0_YOFFSET *((vu16 *) 0x04000012)

#define BG1_XOFFSET *((vu16 *) 0x04000014)
#define BG1_YOFFSET *((vu16 *) 0x04000016)

#define BG2_XOFFSET *((vu16 *) 0x04000018)
#define BG2_YOFFSET *((vu16 *) 0x0400001a)

#define BG3_XOFFSET *((vu16 *) 0x0400001c)
#define BG3_YOFFSET *((vu16 *) 0x0400001e)

#define OAM ((vu16 *) 0x07000000)

extern void screen_init(void);

extern void vsync(void);

#endif // VULC_TEMPLATE_SCREEN
