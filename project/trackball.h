#pragma once
#include "icg_helper.h"

using namespace glm;

class Trackball {
public:
  Trackball() : radius_(1.0f) {}

  // This function is called when the user presses the left mouse button down.
  // x, and y are in [-1, 1]. (-1, -1) is the bottom left corner while (1, 1)
  // is the top right corner.
  void BeingDrag(float x, float y) {
    anchor_pos_ = vec3(x, y, 0.0f);
    ProjectOntoSurface(anchor_pos_);
  }

  // This function is called while the user moves the curser around while the
  // left mouse button is still pressed.
  // x, and y are in [-1, 1]. (-1, -1) is the bottom left corner while (1, 1)
  // is the top right corner.
  // returns the rotation of the trackball in matrix form.
  mat4 Drag(float x, float y, mat4& rot) {
    vec3 current_pos = vec3(x, y, 0.0f);
    ProjectOntoSurface(current_pos);

    mat4 rotation = IDENTITY_MATRIX;
    // Calculate the rotation given the projections of the anocher
    // point and the current position. The rotation axis is given by the cross
    // product of the two projected points, and the angle between them can be
    // used as the magnitude of the rotation.
    // you might want to scale the rotation magnitude by a scalar factor.
    // p.s. No need for using complicated quaternions as suggested in the wiki
    // article.
    vec3 rot_vec = normalize(cross(anchor_pos_, current_pos));

    float angle = acos(dot(normalize(current_pos), normalize(anchor_pos_)));

    return rotate(rotation, angle, rot_vec);
  }

private:
  // Projects the point p (whose z coordiante is still empty/zero) onto the
  // trackball surface. If the position at the mouse cursor is outside the
  // trackball, use a hyberbolic sheet as explained in:
  // https://www.opengl.org/wiki/Object_Mouse_Trackball.
  // The trackball radius is given by 'radius_'.
  void ProjectOntoSurface(vec3& p) const {
    if(p.x*p.x + p.y*p.y <= radius_*radius_ / 2) {
      p.z = sqrt(radius_*radius_ - p.x*p.x - p.y*p.y);
    }
    else {
      p.z = radius_*radius_ / (2*sqrt(p.x*p.x + p.y*p.y));
    }
  }

  float radius_;
  vec3 anchor_pos_;
  mat4 rotation_;
};
