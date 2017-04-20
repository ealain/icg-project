#pragma once
#include "icg_helper.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/vector_angle.hpp>

using namespace glm;

class Camera {

private:
    // Projects the point p (whose z coordiante is still empty/zero) onto the
    // sphere surface. If the position at the mouse cursor is outside the
    // sphere, use a hyberbolic sheet.
    void ProjectOntoSurface(vec3& p) const {
	if(p.x*p.x + p.y*p.y <= radius_*radius_ / 2) {
	    p.z = -sqrt(radius_*radius_ - p.x*p.x - p.y*p.y);
	}
	else {
	    p.z = -radius_*radius_ / (2*sqrt(p.x*p.x + p.y*p.y));
	}
    }

    float radius_;
    vec3 anchor_pos_;
    mat4 rotation_;
    vec4 target_;
    vec3 eye_;

public:
    Camera() : radius_(1.0f),
	       anchor_pos_(vec3(0.0f)),
	       rotation_(IDENTITY_MATRIX),
	       target_(vec4(0.0f, 0.0f, -0.5f, glfwGetTime())),
	       eye_(vec3(0.0f, 0.2f, 0.0f)) {}

    // This function is called when the user presses the left mouse button down.
    // x, and y are in [-1, 1]. (-1, -1) is the bottom left corner while (1, 1)
    // is the top right corner.
    void BeginDrag(float x, float y) {
	anchor_pos_ = vec3(x, y, 0.0f);
	ProjectOntoSurface(anchor_pos_);
    }

    // This function is called while the user moves the curser around while the
    // left mouse button is still pressed.
    // x, and y are in [-1, 1]. (-1, -1) is the bottom left corner while (1, 1)
    // is the top right corner.
    void Drag(float x, float y) {
	vec3 current_pos = vec3(x, y, 0.0f);
	ProjectOntoSurface(current_pos);

	vec3 rotation_vector = -cross(normalize(anchor_pos_), normalize(current_pos));

	if(length(rotation_vector) != 0.0f) {
	    mat4 rot_mat = rotate(rotation_,
				  -angle(normalize(anchor_pos_), normalize(current_pos))
				  * (float)(glfwGetTime() - target_.w),
				  rotation_vector);

	    target_ =  rot_mat * target_;
	    target_.w = glfwGetTime();
	}

	return;
    }

    void Zoom(float y) {
	eye_ += (y - anchor_pos_.y) * 0.05f * (vec3(target_) - eye_);
    }

    vec3 getTarget() {return vec3(target_.x, target_.y, target_.z);}
    vec3 getEye() {return eye_;}

    mat4 getViewMatrix() {
	return lookAt(eye_, vec3(target_.x, target_.y, target_.z), vec3(0.0f, 1.0f, 0.0f));
    }
};
