#version 330

//in vec3 vpoint;
in vec2 position;


out vec2 uv;
out vec3 pos_3d;

uniform mat4 MVP;
uniform float time;

void main() {

    uv = (position + vec2(1.0, 1.0)) * 0.5;
/*    
    float dist = uv[0] + uv[1];
    float waves_amplitude = 0.01;
    float time_attenuation = 0.3;
    float height = waves_amplitude * sin(3 * 20 * 3.14 * dist + time * time_attenuation);

*/

    pos_3d = vec3(position.x, 0, -position.y);

    gl_Position = MVP * vec4(pos_3d, 1.0f);
}