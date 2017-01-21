#version 330 core

in vec2 UV;

out vec3 color;

uniform sampler2D renderedTexture;
uniform float time;


float flatv(float v)
{
    return v - (1.0f/16.0f) * fract(v*16.0f);
}

vec3 flatvv(vec3 v)
{
    return vec3(flatv(v.x),flatv(v.y), flatv(v.z));
}

vec3 fetch(vec2 uv)
{
    vec3 c = flatvv(vec3(texture(renderedTexture, uv)));
    return c;
}

void main(){
    float x = 1.0f / 1280.0f;
    float y = 1.0f / 720.0f;

    vec3 v = vec3(0);
    v += fetch(UV-vec2(x,0))*2.0f - fetch(UV+vec2(x,0))*2.0f;
    v += fetch(UV-vec2(x,y))*1.0f - fetch(UV+vec2(x,-y))*1.0f;
    v += fetch(UV+vec2(-x,y))*1.0f - fetch(UV+vec2(x,y))*1.0f;

    vec3 c = vec3(0);
    c += fetch(UV-vec2(0,y))*2.0f + fetch(UV-vec2(x,y)) + fetch(UV+vec2(x,-y));
    c -= fetch(UV+vec2(0,y))*2.0f + fetch(UV+vec2(x,y)) + fetch(UV+vec2(-x,y));
    
    c = vec3(v.x*v.x + c.x*c.x, v.y*v.y + c.y*c.y, v.z*v.z + c.z*c.z);
    float q = 0.2f;
    if(c.x + c.y + c.z > q)
        color = vec3(0);
    else
        color = 0.1f + flatvv(fetch(UV));
}
