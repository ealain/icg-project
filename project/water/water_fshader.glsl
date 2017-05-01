#version 330

in vec2 uv;

out vec3 color;

uniform sampler2D water_tex;

void main() {
    color = texture(water_tex, uv).rgb;
    //color = vec3(0.2, 0.0, 0.0);
}