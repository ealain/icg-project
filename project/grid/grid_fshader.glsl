#version 330

#define M_PI 3.14159265359

in vec2 uv;
in vec3 pos_3d;
in float height;
in float dist;


out vec4 color;

uniform sampler2D tex;
uniform mat4 MV;
uniform vec3 light_pos;
uniform sampler2D grassTex;
uniform sampler2D rockTex;
uniform sampler2D sandTex;
uniform sampler2D snowTex;
uniform vec2 movement;
uniform float time;
uniform int zero;
uniform int fogSelector;




//Set the height of different element
float waterHeight = 0.0;
float grassHeight = 0.06;
float snowHeight = 0.09;
float rockHeight =  snowHeight - 0.01;

const vec4 fogColor = vec4(0.93333, 0.952941, 0.97647, 1.0);
const float FogDensity = 2;



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

    //relative position
    vec2 position = uv + movement;

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

    if(cosAngle <= 0.0f)
	discard;

    vec3 texColor;

    //texture
    float scale = 30;
    vec3 sandTex = texture(sandTex, scale * position).rgb;
    vec3 grassTex = texture(grassTex, scale * position).rgb;
    vec3 rockTex = texture(rockTex, scale * position).rgb;
    vec3 snowTex = texture(snowTex, scale * position).rgb;

    float delta_sand = 0.001f;

    //mixed texture
    vec3 MixALtRockSnwoTex = mix(rockTex, snowTex, saison(Interpol(rockHeight, snowHeight)));
    vec3 MixALtRockGrassTex = mix(grassTex, rockTex, Interpol(grassHeight, rockHeight));

    vec3 MixAngSnowRockTex = mix(MixALtRockSnwoTex, rockTex, soft_interpolation(1.0f-cosAngle));
    vec3 MixAngRockRockTex = mix(MixALtRockGrassTex, rockTex, sharp_interpolation(1.0f-cosAngle));
    vec3 MixAngRockGrassTex = mix(grassTex, rockTex, sharp_interpolation(1.0f-cosAngle));

    vec3 MixSandTex = mix(MixAngRockGrassTex, sandTex, sharp_interpolation(1.0f-cosAngle));
    // Texture for any transition between Sand and Grass
    vec3 softSG = mix(sandTex, grassTex, soft_shifted_interpolation((height-waterHeight)/(delta_sand+waterHeight)));


    if(height < waterHeight){
	texColor = sandTex;
    }
    else if(height < grassHeight){
	// Default case
	texColor = MixAngRockGrassTex;
	// If the pixel is sufficiently close to the water to receive sand
	if(height < (delta_sand+waterHeight))
	    texColor = mix(softSG, MixAngRockGrassTex, sharp_interpolation((1.0f-cosAngle)+0.4f));
    } else if(height < rockHeight){
        texColor = MixAngRockRockTex;
    } else {
	texColor = MixAngSnowRockTex;
    }

    vec3 result = light * texColor;

    if(zero == 0)
	if(height < waterHeight)
	    //color = vec4(result, 1.0);
	    discard;
	else
	    color = vec4(result, 0.8);
    else
	color = vec4(result, 1.0);
    
    float fogFactor;

    if(fogSelector == 1)//linear fog
    {
       // 20 - fog starts; 80 - fog ends
       fogFactor = (1.0 - soft_shifted_interpolation(dist));
       fogFactor = clamp(fogFactor, 0.0, 1.0);
     
       //if you inverse color in glsl mix function you have to
       //put 1.0 - fogFactor
       color = mix(fogColor, color, fogFactor);
    } else if (fogSelector == 2) {
        fogFactor = 1.0 /exp( (dist * FogDensity)* (dist * FogDensity));
        fogFactor = clamp( fogFactor, 0.0, 1.0 );
 
        color = mix(fogColor, color, fogFactor);
    }
}
