#version 330

out vec3 color;

uniform float grid_dim;
uniform vec2 ratio;

uniform vec2 grad_values[81]; // (Grid dim + 1) * (GRID_DIM + 1)

vec2 g0, g1, g2, g3;
vec2 d0, d1, d2, d3;
float s, t, u, v;

// Coordinates of the cell
int i, j;

void grad() {
    g0.x = grad_values[int((grid_dim+1) * j + i)][0];
    g0.y = grad_values[int((grid_dim+1) * j + i)][1];
    g1.x = grad_values[int((grid_dim+1) * j + i+1)][0];
    g1.y = grad_values[int((grid_dim+1) * j + i+1)][1];
    g2.x = grad_values[int((grid_dim+1) * (j+1) + i+1)][0];
    g2.y = grad_values[int((grid_dim+1) * (j+1) + i+1)][1];
    g3.x = grad_values[int((grid_dim+1) * (j+1) + i)][0];
    g3.y = grad_values[int((grid_dim+1) * (j+1) + i)][1];
}


void diff() {
    d0.x = (gl_FragCoord.x / ratio[0] - int(gl_FragCoord.x / ratio[0]));
    d0.y = (gl_FragCoord.y / ratio[1] - int(gl_FragCoord.y / ratio[1]));

    d1.x = (gl_FragCoord.x / ratio[0] - int(gl_FragCoord.x / ratio[0] + 1));
    d1.y = (gl_FragCoord.y / ratio[1] - int(gl_FragCoord.y / ratio[1]));

    d2.x = (gl_FragCoord.x / ratio[0] - int(gl_FragCoord.x / ratio[0] + 1));
    d2.y = (gl_FragCoord.y / ratio[1] - int(gl_FragCoord.y / ratio[1] + 1));

    d3.x = (gl_FragCoord.x / ratio[0] - int(gl_FragCoord.x / ratio[0]));
    d3.y = (gl_FragCoord.y / ratio[1] - int(gl_FragCoord.y / ratio[1] + 1));
}


void scal() {
    s = 0.5f*dot(g0, d0) +0.5f;
    t = 0.5f*dot(g1, d1) +0.5f;
    u = 0.5f*dot(g3, d3) +0.5f;
    v = 0.5f*dot(g2, d2) +0.5f;
}

float f(float t) {
    return 6*pow(t,5) - 15*pow(t,4) + 10*pow(t,3);
}

float n() {
    return mix(mix(s, t, f(gl_FragCoord.x/ratio[0] - int(gl_FragCoord.x / ratio[0]))),
	       mix(u, v, f(gl_FragCoord.x/ratio[0] - int(gl_FragCoord.x / ratio[0]))),
	       f(gl_FragCoord.y/ratio[1] - int(gl_FragCoord.y / ratio[1])));
}

void main() {

    i = int(gl_FragCoord.x / ratio[0]);
    j = int(gl_FragCoord.y / ratio[1]);

    grad();
    diff();
    scal();

    color = vec3(n());
    // Debugging...
    // color = vec3(int(1.2f));
    // color = vec3(gl_FragCoord.x/512 - int(gl_FragCoord.x/512.0f*2.0f)/2.0);
    // color = vec3(abs(grad_values[1][0]));
    // color = vec3(abs(g1.x));
    // color = vec3(float(j)/8);
}
