#version 330

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

float pi = 3.14159265359;

float waterHeight = 0.0;
float sandHeight = waterHeight + 0.003;
float grassHeight = 0.07;
float snowHeight = 0.09;
float rockHeight =  snowHeight - 0.01;

float InterpolRockSnow(){
    float result = height/snowHeight;
    result = result * (snowHeight - rockHeight);
    return result;
}

float InterpolAngle(float angle){
    float tmp = sin(angle);
    float result;
    if(tmp > sin(pi/12.0)){
        result = 1;
    } else {
        result = sin(angle);
    }
    return result;
}


void main() {

    float alpha = 1.0;
    // Set up some useful color for interpolation
    vec3 blue = vec3(0.0,0.0,1.0);
    vec3 darken_blue = vec3(0.1, 0.1,0.6);
    //vec2 position = uv - movement.xy;
    float x = gl_FragCoord.x + movement.x; // * 100.0f;
    float y = gl_FragCoord.y + movement.y; // * 100.0f;
    vec2 position = uv + vec2(movement.x,movement.y)/10.0f;
    vec3 v1 = dFdx(vec3(2*uv.x-1, texture(tex, uv).r, -2*uv.y+1));
    vec3 v2 = dFdy(vec3(2*uv.x-1, texture(tex, uv).r, -2*uv.y+1));

    vec3 normal = normalize(cross(v1, v2));

    // Setting ambient light
    vec3 color_tmp = vec3(0.05f, 0.05f, 0.05f);

    vec3 light_dir = normalize(light_pos - vec3(MV * vec4(pos_3d, 1.0f)));

    float lambert = dot(normal, light_dir);


    vec3 kd = vec3(0.557f, 0.33f, 0.204f);
    vec3 Ld = vec3(1.0f);

    color_tmp += vec3(Ld*kd*lambert);


    float scale = 5;

    float angle = dot(normal, vec3(0.0f, 1.0f, 0.0f));
    float sinAngle = sin(angle);
    float random = noise1(angle/4)/100;
    vec3 texColor;


    //texture
    vec3 sandTex = texture(sandTex, scale * position).rgb;
    vec3 grassTex = texture(grassTex, scale * position).rgb;
    vec3 rockTex = texture(rockTex, scale * position).rgb;
    vec3 snowTex = texture(snowTex, scale * position).rgb;

    //mixed texture
    vec3 MixALtRockSnwoTex = mix(rockTex, snowTex,InterpolRockSnow());
    vec3 MixAngRockSnwoTex = mix(snowTex, rockTex,InterpolAngle(angle));
    vec3 MixAngRockGrassTex = mix(grassTex, rockTex,InterpolAngle(angle));

    if(height < waterHeight){
        texColor = mix(darken_blue, blue, height);
    }
    else if(height < sandHeight){
        texColor = sandTex;
    }
    else if(height < grassHeight){
        if(sinAngle > sin(pi/4.0)){
            texColor = MixAngRockGrassTex;
        }
        texColor = grassTex;
    }
    else if(height < rockHeight){
        texColor = rockTex;
    } else if (height < snowHeight){
        texColor = MixALtRockSnwoTex;
    }
    else {
        texColor = snowTex;
    }

    //vec3 result = tmp_color * texColor;
    color = vec4(texColor, alpha);

}
