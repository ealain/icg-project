#version 330

#define M_PI 3.14159265359

in vec2 uv;
in vec3 pos_3d;
in float height;

out vec4 color;

uniform sampler2D tex;
uniform mat4 MV;
uniform vec3 light_pos;
uniform sampler2D grassTex;
uniform sampler2D rockTex;
uniform sampler2D sandTex;
uniform sampler2D snowTex;
uniform vec3 movement;
uniform float time;


//Set the height of different element
float waterHeight = 0.0;
float sandHeight = waterHeight - 0.001f;
float grassHeight = 0.07;
float snowHeight = 0.09;
float rockHeight =  snowHeight - 0.01;

// Set up some useful color for interpolation
vec3 blue = vec3(0.0,0.0,1.0);
vec3 darken_blue = vec3(0.1, 0.1,0.6);

//Return coef for mix function based on height
float Interpol(float heightMin,float  heightMax){
    if (height > heightMax){
        heightMax = height;
    }
    float result = (height - heightMin)/(heightMax - heightMin);
    return result;
}


float saison(float coef){
    float tmp = cos(time/5);
    tmp = (tmp + 1)/2;
    if(tmp < 0){
        tmp = 0;
    }
    return coef * tmp;
}

/* Interpolation functions. In any case :
   0         ->         0
   1         ->         1          */

/*	  0.3       ->         0.01
	  0.7       ->         0.99       */
float sharp_interpolation(float t) {
    return 1.000007 + (-2.098172*pow(10, -17) - 1.000007)/(1 + pow(t/0.5597708, 20.55199));
}

/*        0.7       ->         0.1
	  0.9       ->         0.9        */
float soft_shifted_interpolation(float t) {
    return 1.023499 + (-4.982824*pow(10,-17) - 1.023499)/(1 + pow((t/0.7993572), 16.74859));
}


/*        0.2       ->         0.05
	  0.8       ->         0.95       */
float soft_interpolation(float t) {
    return 1.040188 + (4.020405*pow(10, -18) - 1.040188)/(1 + pow((t/0.4341559), 3.85231));
}



void main() {

    float alpha = 1.0;

    //relative position
    vec2 position = uv + vec2(movement.x,movement.y)/10.0f;

    // Setting ambient light
    float ambient = 0.6f;
    vec3 ambientLight = vec3(ambient);

    // Setting diffuse light
    vec3 v1 = dFdx(vec3(2*uv.x-1, texture(tex, uv).r, -2*uv.y+1));
    vec3 v2 = dFdy(vec3(2*uv.x-1, texture(tex, uv).r, -2*uv.y+1));
    vec3 normal = normalize(cross(v1, v2));
    vec3 light_dir = normalize(light_pos - vec3(MV * vec4(pos_3d, 1.0f)));
    float lambert = dot(normal, light_dir);
    vec3 kd = vec3(0.557f, 0.33f, 0.204f);
    vec3 Ld = vec3(1.0f);
    vec3 diffureLight = vec3(Ld*kd*lambert);
    vec3 light = ambientLight + diffureLight;


    float cosAngle = dot(normal, vec3(0.0f, 1.0f, 0.0f));

    vec3 texColor;

    //texture
    float scale = 30;
    vec3 sandTex = texture(sandTex, scale * position).rgb;
    vec3 grassTex = texture(grassTex, scale * position).rgb;
    vec3 rockTex = texture(rockTex, scale * position).rgb;
    vec3 snowTex = texture(snowTex, scale * position).rgb;

    float delta_sand = 0.005;

    sandHeight += delta_sand;

    //mixed texture
    vec3 MixALtRockSnwoTex = mix(rockTex, snowTex, saison(Interpol(rockHeight, snowHeight)));
    vec3 MixALtRockGrassTex = mix(grassTex, rockTex, Interpol(grassHeight, rockHeight));
    vec3 MixAngRockGrassTex = mix(grassTex, rockTex, sharp_interpolation(1.0f-cosAngle));
    vec3 MixSandTex = mix(MixAngRockGrassTex, sandTex, sharp_interpolation(1.0f-cosAngle));
    // Texture for any transition between Sand and Grass
    vec3 softSG = mix(sandTex, grassTex, soft_shifted_interpolation((height-waterHeight)/(delta_sand+waterHeight)));

    if(height < waterHeight){
        texColor = mix(darken_blue, blue, height);
    }
    else if(height < grassHeight){
	// Default case
	texColor = MixAngRockGrassTex;
	// If the pixel is sufficiently close to the water to receive sand
	if(height < (delta_sand+waterHeight))
	    if(cosAngle < 0.5f)
		texColor = mix(softSG, MixAngRockGrassTex, sharp_interpolation((1.0f-cosAngle)+0.1f));
	    else
		texColor = mix(grassTex, softSG, sharp_interpolation(cosAngle-0.1));
    } else if(height < rockHeight){
        texColor = MixALtRockGrassTex;
    } else {
	texColor = MixALtRockSnwoTex;
    }

    vec3 result = light * texColor;
    color = vec4(result, alpha);

}
