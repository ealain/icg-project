#pragma once
#include "icg_helper.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/vector_angle.hpp>

using namespace glm;

class Camera {

private:
    void projectOntoSphere(vec3& p) {
	p = normalize(p) * radius_;
    }

    float radius_;
    float last_time_, previous_time_;
    vec2 anchor_pos_;
    mat4 rotation_;
    vec3 view_dir_;
    vec3 eye_;
    float inertia_h_, inertia_v_, inertia_ud_;

public:
    Camera() : radius_(1.0f),
	       anchor_pos_(vec3(0.0f)),
	       rotation_(IDENTITY_MATRIX),
	       view_dir_(vec3(0.0f, 0.0f, -1.0f)),
	       eye_(vec3(0.0f, 0.2f, 0.0f)) {
	projectOntoSphere(view_dir_);
	rotation_ = lookAt(vec3(0.0f), view_dir_, vec3(0.0f, 1.0f, 0.0f));
    }

    // This function is called when the user presses the left mouse button down.
    // x, and y are in [-1, 1]. (-1, -1) is the bottom left corner while (1, 1)
    // is the top right corner.
    void BeginDrag(float x, float y) {
	    last_time_ = glfwGetTime();
	    anchor_pos_ = vec2(x, y);
	}

    void Drag(float x, float y) {
	vec2 current_pos = vec2(x, y);
	vec3 translation_vector = vec3(current_pos - anchor_pos_, 0.0f);

	translation_vector = vec3(inverse(rotation_) * vec4(translation_vector, 0.0f));
	view_dir_ = view_dir_ + translation_vector  * float(glfwGetTime() - last_time_);
	last_time_ = glfwGetTime();
	projectOntoSphere(view_dir_);

	rotation_ = lookAt(vec3(0.0f), view_dir_, vec3(0.0f, 1.0f, 0.0f));

	return;
    }

    void Update(bool dragging, vec2 p, bool mv_forward, bool mv_backward,
		bool camera_forward, bool camera_backward,
		bool turn_right, bool turn_left, bool turn_up, bool turn_down,
		bool fps_mode, float altitude) {

	float delta = glfwGetTime() - previous_time_;
	previous_time_ = glfwGetTime();
	if(dragging)
	    this->Drag(p.x, p.y);
	if(camera_forward)
	    this->Forward(1);
	if(camera_backward)
	    this->Forward(-1);

	if(turn_right) {
	    if(inertia_h_ < 5.0f)
		inertia_h_ += delta;
	}
	else
	    if(inertia_h_ > 0.0f)
		if(inertia_h_ > 0.01f)
		    inertia_h_ -= delta;
		else
		    inertia_h_ = 0.0f;

	if(turn_left) {
	    if(inertia_h_ > -5.0f)
		inertia_h_ -= delta;
	}
	else
	    if(inertia_h_ < 0.0f)
		if(inertia_h_ < -0.01f)
		    inertia_h_ += delta;
		else
		    inertia_h_ = 0.0f;

	this->Turn_h(inertia_h_);

	if(turn_up) {
	    if(inertia_v_ < 5.0f)
		inertia_v_ += delta;
	}
	else
	    if(inertia_v_ > 0.0f)
		if(inertia_v_ > 0.01f)
		    inertia_v_ -= delta;
		else
		    inertia_v_ = 0.0f;

	if(turn_down) {
	    if(inertia_v_ > -5.0f)
		inertia_v_ -= delta;
	}
	else
	    if(inertia_v_ < 0.0f)
		if(inertia_v_ < -0.01f)
		    inertia_v_ += delta;
		else
		    inertia_v_ = 0.0f;

	this->Turn_v(inertia_v_);


	if(mv_forward) {
	    if(inertia_ud_ < 1.0f)
		inertia_ud_ += delta/10.0f;
	    }
	else
	    if(inertia_ud_ > 0.0f)
		if(inertia_ud_ > 0.01f)
		    inertia_ud_ -= delta/2.0f;
		else
		    inertia_ud_ = 0.0f;

	if(mv_backward) {
	    if(inertia_ud_ > -1.0f)
		inertia_ud_ -= delta/10.0f;
	}
	else
	    if(inertia_ud_ < 0.0f)
		if(inertia_ud_ < -0.01f)
		    inertia_ud_ += delta/2.0f;
		else
		    inertia_ud_ = 0.0f;

	this->UpDown(inertia_ud_);

	if(fps_mode) {
	    if(altitude < 0.0f)
		altitude = 0.0f;
	    eye_.x = 0.0f;
	    eye_.y = exp(altitude) - 0.95f;
	    eye_.z = 0.0f;
	}

    }

    void Forward(char forward) {
	eye_ += float(forward) * 0.05f * (vec3(view_dir_));
    }

    void Turn_h(float turn_right) {
	vec3 translation_vector = vec3(inverse(rotation_) * vec4(turn_right, 0.0f, 0.0f, 0.0f));
	view_dir_ = view_dir_ + translation_vector  * float(glfwGetTime() - last_time_);
	last_time_ = glfwGetTime();
	projectOntoSphere(view_dir_);

	rotation_ = lookAt(vec3(0.0f), view_dir_, vec3(0.0f, 1.0f, 0.0f));
    }

    void Turn_v(float turn_up) {
	vec3 translation_vector = vec3(inverse(rotation_) * vec4(0.0f, 900.0f*turn_up, 0.0f, 0.0f));
	view_dir_ = view_dir_ + translation_vector  * float(glfwGetTime() - last_time_);
	last_time_ = glfwGetTime();
	projectOntoSphere(view_dir_);

	rotation_ = lookAt(vec3(0.0f), view_dir_, vec3(0.0f, 1.0f, 0.0f));
    }

    void UpDown(float up) {
	// Compensation (no need to look very high to get up)
	vec3 view_dir_cp = view_dir_;
	view_dir_cp.y += 0.00f;
	eye_ += vec3(0.0f, 0.05f, 0.0f) * up * (vec3(view_dir_cp));
    }

    vec3 getViewDirection() {return view_dir_;}

    mat4 getViewMatrix() {
	mat4 rotation = lookAt(vec3(0.0f), view_dir_, vec3(0.0f, 1.0f, 0.0f));
	mat4 translation = translate(IDENTITY_MATRIX, -eye_);
	return rotation*translation;
    }

    vec2 getTerrainOffset() {
	vec3 vd = getViewDirection();
	return vec2(dot(vec3(1.0f, 0.0f, 0.0f), getViewDirection()),
			 dot(vec3(0.0f, 0.0f, -1.0f), getViewDirection()));
    }

    mat4 invY() {
	eye_.y = -(eye_.y);
	view_dir_.y = -view_dir_.y;

	mat4 rotation = lookAt(vec3(0.0f), view_dir_, vec3(0.0f, 1.0f, 0.0f));
	mat4 translation = translate(IDENTITY_MATRIX, -eye_);

	eye_.y = -(eye_.y);
	view_dir_.y = -view_dir_.y;

	return rotation*translation;
    }
};
