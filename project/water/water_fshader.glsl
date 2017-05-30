#version 330

in vec2 uv;
in float dist;


out vec4 color;

uniform sampler2D water_tex;

uniform int fogSelector;
const vec4 fogColor = vec4(0.816, 0.859, 0.857, 1.0);

const float FogDensity = 2;


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
       fogFactor = (1.0 - dist)/(1);
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
