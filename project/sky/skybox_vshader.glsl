#version 330

in vec3 position;

out vec3 texCoord;

uniform mat4 model_matrix;

void main() {
    gl_Position = model_matrix * vec4(position, 1.0f);
    texCoord = position;
}
