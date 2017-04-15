#version 330

out vec3 color;

uniform float grid_dim;
uniform vec2 ratio;

uniform float time;
uniform vec3 view_dir;

uniform int fmax;

uniform float param;


uniform vec2 grad_values[53];  // NB_GRADIENTS is defined in heightmap.h
uniform int nb_gradients;

int f;                          // Frequency of the noise in the map
                                // If the checkerboard has 3 stripes, the frequency is 4
int offset;                     // Offset to read the desired gradients in grad_values

float ratio_x;                  // Number of pixels per stripe (resolution_x / 3)
float ratio_y;                  // Number of pixels per stripe (resolution_y / 3)

vec2 g0, g1, g2, g3;            // Cell's gradients
vec2 d0, d1, d2, d3;            // Difference vectors from cell corners to current pixel

// Coordinates of the cell (computed in main())
int i, j;

// Coordinates taking time into account for movement
float x, y;


void grad() {
    // Given the position of the cell (i,j) in the checkerboard,
    // determines the gradients (vec2) for each corner
    int a = int(offset + (f+1) * j + i) % nb_gradients;
    int b = int(offset + (f+1) * (j+1) + i) % nb_gradients;
    int c = int(offset + (f+1) * j + i + 1) % nb_gradients;
    int d = int(offset + (f+1) * (j+1) + i + 1) % nb_gradients;
    if(a < 0)
	a += nb_gradients;
    if(b < 0)
	b += nb_gradients;
    if(c < 0)
	c += nb_gradients;
    if(d < 0)
	d += nb_gradients;
    g0.x = grad_values[a][0];
    g0.y = grad_values[a][1];
    g1.x = grad_values[c][0];
    g1.y = grad_values[c][1];
    g2.x = grad_values[d][0];
    g2.y = grad_values[d][1];
    g3.x = grad_values[b][0];
    g3.y = grad_values[b][1];
}


void diff() {
    // Given the positions of the pixel p and the corners fo the cell
    // c0, c1, c2, c3; compute the vectors p-c0, p-c1,...
    d0.x = x / ratio_x - i;
    d0.y = y / ratio_y - j;

    d1.x = x / ratio_x - (i+1);
    d1.y = y / ratio_y - j;

    d2.x = x / ratio_x - (i+1);
    d2.y = y / ratio_y - (j+1);

    d3.x = x / ratio_x - i;
    d3.y = y / ratio_y - (j+1);
}

float interpolation(float t) {
    // Smooth interpolation function (C2 continuity)
    return 6*pow(t,5) - 15*pow(t,4) + 10*pow(t,3);
}

float perlin_noise() {
    // Linear interpolation of scalar products
    return mix(mix(0.25f*dot(g0, d0)+0.5f,
		   0.25f*dot(g1, d1)+0.5f,
		   interpolation(x/ratio_x - i)),

	       mix(0.25f*dot(g3, d3)+0.5f,
		   0.25f*dot(g2, d2)+0.5f,
		   interpolation(x/ratio_x - i)),

		   interpolation(y/ratio_y - j));
}



void main() {

    float x_movement = -normalize(view_dir).x;
    float z_movement = normalize(view_dir).z;

    ratio_x = ratio[0];
    ratio_y = ratio[1];

    x = gl_FragCoord.x + (x_movement * 0.1f * time) * fmax * ratio[0];
    y = gl_FragCoord.y + (z_movement * 0.1f * time) * fmax * ratio[1];

    float n_total = 0.0f;                   // Sum of the noise contributions for distinct freq
    offset = 0;
    int iteration;
    for(f=fmax+1, iteration=1; f >= grid_dim+1; f/=2, iteration++) {

	i = (x >= 0)? int(x / ratio_x) : int(x / ratio_x) - 1;
	j = (y >= 0)? int(y / ratio_y) : int(y / ratio_y) - 1;

	grad();
	diff();

	n_total += (perlin_noise() - 0.5f) * pow(param, iteration);
	offset += int(pow(f, 2));
	ratio_x *= float(f-1) / (f/2 -1);
	ratio_y *= float(f-1) / (f/2 -1);
    }

    color = vec3(n_total / (0.25f*pow(param, iteration-1)));

    // Debugging...
    // color = vec3(int(1.2f));
    // color = vec3(gl_FragCoord.x/512 - int(gl_FragCoord.x/512.0f*2.0f)/2.0);
    // color = vec3(abs(grad_values[1][0]));
    // color = vec3(abs(g1.x));
    // color = vec3(float(j)/8);
}
