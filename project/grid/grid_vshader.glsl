#version 330

#define M_PI 3.14159265359

in vec2 position;

out vec2 uv;

uniform mat4 MVP;
uniform float time;

void main() {
  uv = (position + vec2(1.0, 1.0)) * 0.5;

  // convert the 2D position into 3D positions that all lay in a horizontal
  // plane.
  // TODO 6: animate the height of the grid points as a sine function of the
  // 'time' and the position ('uv') within the grid
  float dist = uv[0] + uv[1];
  float waves_amplitude = 0.1;
  float time_attenuation = 0.3;
  float height = waves_amplitude * sin(3 * 2 * 3.14 * dist + time * time_attenuation);
  vec3 pos_3d = vec3(position.x, height, -position.y);

  gl_Position = MVP * vec4(pos_3d, 1.0);
}