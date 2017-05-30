#version 330

in vec2 uv;
in float dist;


out vec4 color;

uniform sampler2D water_tex;

uniform int fogSelector;
const vec4 fogColor = vec4(0.93333, 0.952941, 0.97647, 1.0);

const float FogDensity = 2;

/*        0.7       ->         0.1
	  0.9       ->         0.9        */
float soft_shifted_interpolation(float t) {
    return 1.023499 + (-4.982824*pow(10,-17) - 1.023499)/(1 + pow((t/0.7993572), 16.74859));
}

void main() {
    int window_width = textureSize(water_tex, 0).x;
    int window_height = textureSize(water_tex, 0).y;
    
    vec2 uv_ = vec2(gl_FragCoord.x / window_width, 1.0f -  gl_FragCoord.y / window_height);

    color = mix(vec4(0.25f, 0.61f, 0.73f, 0.3f), // Natural color of water
		texture(water_tex, uv_), 0.8);
        
        
    float fogFactor;

    
    if(fogSelector == 1)//linear fog
    {
       // 20 - fog starts; 80 - fog ends
       fogFactor = (1.0 - soft_shifted_interpolation(dist))/(1);
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
