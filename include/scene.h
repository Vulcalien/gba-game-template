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
#pragma once

#include "main.h"

extern const struct Scene {
    void (*init)(void *data);
    void (*tick)(void);
    void (*draw)(void);
} *scene;

INLINE void scene_set(const struct Scene *new_scene, void *data) {
    scene = new_scene;
    scene->init(data);
}

extern void scene_transition_to(const struct Scene *next, void *data);

// Scenes
extern const struct Scene
    scene_start;
