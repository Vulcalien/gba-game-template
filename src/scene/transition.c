/* Copyright 2024 Vulcalien
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */
#include "scene.h"

// it's best if this value is a power of 2, to avoid division
#define TRANSITION_HALFTIME 32

static u32 time;

static const struct Scene *previous_scene;
static const struct Scene *next_scene;
static u32 next_init_data;

THUMB
static void transition_init(u32 data) {
    time = 0;
}

THUMB
static void transition_tick(void) {
    if(time < TRANSITION_HALFTIME)
        previous_scene->tick();
    else if(time == TRANSITION_HALFTIME)
        next_scene->init(next_init_data);
    else if(time > TRANSITION_HALFTIME)
        next_scene->tick();

    time++;

    // if the transition time is over, set the next scene
    if(time > 2 * TRANSITION_HALFTIME) {
        // set the scene directly: 'scene_set' would call 'init' again
        scene = next_scene;
    }
}

THUMB
static void transition_draw(void) {
    // calculate how much to fade
    u32 fade = math_sin(
        time * math_brad(90) / TRANSITION_HALFTIME
    ) * 20 / 0x4000;

    display_darken(NULL, fade);

    if(time < TRANSITION_HALFTIME)
        previous_scene->draw();
    else if(time > TRANSITION_HALFTIME)
        next_scene->draw();
}

static const struct Scene scene_transition = {
    .init = transition_init,
    .tick = transition_tick,
    .draw = transition_draw
};

THUMB
void scene_transition_to(const struct Scene *next, u32 data) {
    // if already transitioning, ignore this request
    if(scene == &scene_transition)
        return;

    previous_scene = scene;
    next_scene     = next;
    next_init_data = data;

    scene_set(&scene_transition, 0);
}
