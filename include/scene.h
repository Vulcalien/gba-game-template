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
    void (*init)(void *data);
    void (*tick)(void);
    void (*draw)(void);
};

extern const struct Scene *scene;

#define SCENE_SET(new_scene)\
    scene_set((new_scene), false, NULL)

ALWAYS_INLINE
inline void scene_set(const struct Scene *new_scene,
                      bool call_init, void *data) {
    scene = new_scene;

    if(call_init && scene->init)
        scene->init(data);
}

// Scenes
extern const struct Scene
    scene_start;

#endif // VULC_TEMPLATE_SCENE
