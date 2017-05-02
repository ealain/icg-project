#version 330

in vec3 texCoord;

out vec3 color;

uniform samplerCube tex;

void main() {
    color = texture(tex, texCoord).rgb;
}
