#version 330

in vec2 uv;
in vec3 pos_3d;

out vec3 color;

uniform sampler2D tex;
uniform mat4 MV;
uniform vec3 light_pos;


void main() {
    vec3 v1 = dFdx(pos_3d);
    vec3 v2 = dFdy(pos_3d);

    vec3 normal = normalize(cross(v1, v2));

    vec3 color_tmp = vec3(0.2f);

    vec3 light_dir = normalize(light_pos - vec3(MV * vec4(pos_3d, 1.0f)));

    float lambert = dot(normal, light_dir);

    vec3 kd = vec3(0.9f, 0.5f, 0.5f);
    vec3 Ld = vec3(0.9f);

    if(lambert > 0.0f)
	color = color_tmp + Ld*kd*lambert;
    else
    	color = color_tmp;
}
