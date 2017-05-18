#version 330

in vec3 texCoord;

out vec4 color;

uniform samplerCube tex;

void main() {
    color = vec4(texture(tex, texCoord).rgb, 0.8);
}
