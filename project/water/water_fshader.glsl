#version 330

in vec2 uv;

out vec4 color;

uniform sampler2D water_tex;

void main() {
    int window_width = textureSize(water_tex, 0).x;
    int window_height = textureSize(water_tex, 0).y;
    
    vec2 uv_ = vec2(gl_FragCoord.x / window_width, 1.0f -  gl_FragCoord.y / window_height);

    color = mix(vec4(0.25f, 0.61f, 0.73f, 0.3f), // Natural color of water
		texture(water_tex, uv_), 0.8);
}
