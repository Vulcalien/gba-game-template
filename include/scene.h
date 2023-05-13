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
#ifndef VULC_TEMPLATE_SCENE
#define VULC_TEMPLATE_SCENE

#include "main.h"

struct Scene {
    void (*init)(u32 flags);
    void (*tick)(void);
    void (*draw)(void);
};

extern const struct Scene *scene;

extern const struct Scene scene_start;

inline void scene_set(const struct Scene *new_scene, u32 flags) {
    scene = new_scene;

    if(flags && scene->init)
        scene->init(flags);
}

#endif // VULC_TEMPLATE_SCENE
