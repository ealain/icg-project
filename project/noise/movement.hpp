#ifndef MOVEMENT_INCLUDE
#define MOVEMENT_INCLUDE

#include "icg_helper.h"

float last_time;
vec2 movement_offset = vec2(0.0f);

vec2 movement(vec3 view_dir, char right, char forward, float speed = 1.0f) {
    float delta = (glfwGetTime() - last_time)/10.0f;
    last_time = glfwGetTime();
    
    if(forward == 1)
	movement_offset += vec2(view_dir.x * delta, -view_dir.z * delta);
    if(forward == -1)
	movement_offset += vec2(-view_dir.x * delta, view_dir.z * delta);
    if(right == 1)
	movement_offset += vec2(-cross(vec3(0.0f, 1.0f, 0.0f),
				vec3(view_dir.x, 0.0f, view_dir.z)).x * delta,
			 cross(vec3(0.0f, 1.0f, 0.0f),
			       vec3(view_dir.x, 0.0f, view_dir.z)).z * delta);
    if(right == -1)
	movement_offset += vec2(cross(vec3(0.0f, 1.0f, 0.0f),
			       vec3(view_dir.x, 0.0f, view_dir.z)).x * delta,
			 -cross(vec3(0.0f, 1.0f, 0.0f),
				vec3(view_dir.x, 0.0f, view_dir.z)).z * delta);
    return movement_offset;
}

#endif
