#version 330

in vec2 uv;
in vec3 pos_3d;
in float height; 

out vec4 color;

uniform sampler2D tex;
uniform mat4 MV;
uniform vec3 light_pos;
uniform int zero;



void main() {
    
    if (true) {
    
    // Set up some useful color for interpolation
        float alpha = 1.0; 
        vec4 white = vec4(1.0,1.0,1.0,1.0); 
        vec4 blue = vec4(0.0,0.0,1.0,1.0); 
        vec4 darken_blue = vec4(0.1, 0.1,0.6,0.1); 
        vec4 green = vec4(0.0, 0.7, 0.0, 1.0); 
        vec4 tmp_color = vec4(0.0, 0.0, 0.0, 0.0); 

        vec3 v1 = dFdx(vec3(2*uv.x-1, texture(tex, uv).r, -2*uv.y+1));
        vec3 v2 = dFdy(vec3(2*uv.x-1, texture(tex, uv).r, -2*uv.y+1));

        vec3 normal = normalize(cross(v1, v2));

        // Setting ambient light
        vec4 color_tmp = vec4(0.05f, 0.05f, 0.05f, alpha);

        vec3 light_dir = normalize(light_pos - vec3(MV * vec4(pos_3d, 1.0f)));

        float lambert = dot(normal, light_dir);


        vec3 kd = vec3(0.557f, 0.33f, 0.204f);
        vec3 Ld = vec3(1.0f);

        color_tmp += vec4(Ld*kd*lambert, alpha);
        
        // Set up color in function of the height
        if(height > 0.6){
            color = mix(white, color_tmp, 0.4);
        }
        else if(height < 0.0){
            color = mix(darken_blue, blue, height*100);
        }
        else {
            color = mix(mix(green, white, height / 0.6f), color_tmp, 0.5);
        }
        
    } else {
        
        float alpha = 1.0; 
        vec4 white = vec4(1.0,1.0,1.0,1.0); 
        vec4 blue = vec4(0.0,0.0,1.0,1.0); 
        vec4 darken_blue = vec4(0.1, 0.1,0.6,0.1); 
        vec4 green = vec4(0.0, 0.7, 0.0, 1.0); 
        vec4 tmp_color = vec4(0.0, 0.0, 0.0, 0.0); 

        vec3 v1 = dFdx(vec3(2*uv.x-1, texture(tex, uv).r, -2*uv.y+1));
        vec3 v2 = dFdy(vec3(2*uv.x-1, texture(tex, uv).r, -2*uv.y+1));

        vec3 normal = normalize(cross(v1, v2));

        // Setting ambient light
        vec4 color_tmp = vec4(0.05f, 0.05f, 0.05f, alpha);

        vec3 light_dir = normalize(light_pos - vec3(MV * vec4(pos_3d, 1.0f)));

        float lambert = dot(normal, light_dir);


        vec3 kd = vec3(0.557f, 0.33f, 0.204f);
        vec3 Ld = vec3(1.0f);

        color_tmp += vec4(Ld*kd*lambert, alpha);
        
        // Set up color in function of the height
        if(height > 0.6){
            color = mix(white, color_tmp, 0.4);
        }
        else if(height < 0.0){
            color = mix(darken_blue, blue, height*100);
            color.a = 0.0;
        }
        else {
            color = mix(mix(green, white, height / 0.6f), color_tmp, 0.5);
        }
    }
    
    //color.a = 1.0;
}
