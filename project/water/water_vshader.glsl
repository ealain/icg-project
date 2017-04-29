#version 330

//in vec3 vpoint;
in vec2 position;

out vec2 uv;
out vec3 pos_3d;

uniform mat4 MVP;

void main() {

    uv = (position + vec2(1.0, 1.0)) * 0.5;

    pos_3d = vec3(position.x, 0, -position.y);

    gl_Position = MVP * vec4(pos_3d, 1.0f);
}