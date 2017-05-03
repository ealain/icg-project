#version 330

in vec2 uv;

out vec3 color;

uniform sampler2D water_tex;
uniform sampler2D noise_tex;

void main() {
    color = texture(water_tex, uv).rgb;
    
    int window_width = textureSize(water_tex, 0).x;
    int window_height = textureSize(water_tex, 0).y;
    
    vec2 uv_ = vec2(gl_FragCoord.x / window_width, 1.0f -  gl_FragCoord.y / window_height);


    color = texture(water_tex, uv_).rgb;

    //color = vec3(gl_FragCoord.y / window_height-1.0f);
}