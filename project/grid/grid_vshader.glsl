#version 330

#define M_PI 3.14159265359

in vec2 position;

out vec2 uv;
out vec3 pos_3d;
out float height; 

uniform sampler2D tex;
uniform mat4 MVP;
uniform mat4 MV;
uniform float time;

void main() {
    uv = (position + vec2(1.0, 1.0)) * 0.5;
    // Convert the 2D position into 3D positions that all lay 
    // in a horizontal plane
    height = texture(tex, uv).r;
    pos_3d = vec3(position.x, height, -position.y);

    gl_Position = MVP * vec4(pos_3d, 1.0);



    //pos_3d = vec3(position.x, position.y, 0);

    //gl_Position = vec4(pos_3d, 1.0);
}
