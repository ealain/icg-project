#ifndef MOVEMENT_INCLUDE
#define MOVEMENT_INCLUDE

#include "icg_helper.h"

float last_time;
vec2 movement_offset = vec2(0.0f);

float inertia_fb = 0.0f, inertia_rl = 0.0f;

vec2 movement(vec3 view_dir, bool right, bool left, bool forward, bool backward, float speed = 1.0f) {
    float d = (glfwGetTime() - last_time)/2.0f ;
    last_time = glfwGetTime();

    if(forward) {
	if(inertia_fb < 2.0f)
	    inertia_fb += d;
    }
    else {
	if(inertia_fb > 0.0f) {
	    if(inertia_fb > 0.01f)
		inertia_fb -= 2.0f * d;
	    else
		inertia_fb = 0.0;
	}
    }

    if(backward) {
	if(inertia_fb > -2.0f)
	    inertia_fb -= d;
    }
    else {
	if(inertia_fb < 0.0f) {
	    if(inertia_fb < -0.01f)
		inertia_fb += 2.0f * d;
	    else
		inertia_fb = 0.0f;
	}
    }

    movement_offset += inertia_fb * vec2(view_dir.x * d, -view_dir.z * d);


    if(right) {
	if(inertia_rl < 2.0f)
	    inertia_rl += d;
    }
    else {
	if(inertia_rl > 0.0f)
	    if(inertia_rl > 0.01f)
		inertia_rl -= 2.0f * d;
	    else
		inertia_rl = 0.0f;
    }

    if(left) {
	if(inertia_rl > -2.0f)
	    inertia_rl -= d;
    }
    else {
	if(inertia_rl < 0.0f) {
	    if(inertia_rl < -0.01f)
		inertia_rl += 2.0f * d;
	    else
		inertia_rl = 0.0f;
	}
    }

    movement_offset += inertia_rl * vec2(-cross(vec3(0.0f, 1.0f, 0.0f),
						vec3(view_dir.x, 0.0f, view_dir.z)).x * d,
					 cross(vec3(0.0f, 1.0f, 0.0f),
					       vec3(view_dir.x, 0.0f, view_dir.z)).z * d);
    return movement_offset;
}

#endif
