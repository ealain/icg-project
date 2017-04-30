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
    float last_time_;
    vec2 anchor_pos_;
    mat4 rotation_;
    vec3 view_dir_;
    vec3 eye_;

public:
    Camera() : 
        radius_(1.0f),
        anchor_pos_(vec3(0.0f)),
        rotation_(IDENTITY_MATRIX),
        view_dir_(vec3(0.0f, 0.0f, 1.0f)), // cam_look
        eye_(vec3(0.0f, 0.2f, 0.0f)) { //cam_pos
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

    void Zoom(float y) {
	eye_ += (y - anchor_pos_.y) * 0.05f * (vec3(view_dir_));
    }
    
    void invY() {
        eye_.y = -(eye_.y);
        //view_dir_ = vec3(0,0,1);//lookAt(eye_, view_dir_, vec3(0.0f, 1.0f, 0.0f));
        //view_dir_.y = -view_dir_.y;
        rotation_ = lookAt(eye_, view_dir_, vec3(0.0f, 1.0f, 0.0f));
    }

    vec3 getViewDirection() {return view_dir_;}

    mat4 getViewMatrix() {
        mat4 rotation = lookAt(vec3(0.0f), view_dir_, vec3(0.0f, 1.0f, 0.0f));
        mat4 translation = translate(IDENTITY_MATRIX, -eye_);
        return rotation*translation;
    }
};
