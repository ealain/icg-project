#version 330

in vec2 uv;

out vec4 color;

uniform sampler2D water_tex;
uniform sampler2D noise_tex;

void main() {
    //color = texture(water_tex, uv).rgb;
    
    int window_width = textureSize(water_tex, 0).x;
    int window_height = textureSize(water_tex, 0).y;
    
    vec2 uv_ = vec2(gl_FragCoord.x / window_width, 1.0f -  gl_FragCoord.y / window_height);


    color = vec4(texture(water_tex, uv_).rgb, 0.5);

    //color = vec3(gl_FragCoord.y / window_height-1.0f);
}