#version 330 core

in vec2 UV;

out vec3 color;

uniform sampler2D renderedTexture;
uniform float time;

void main(){
    float x = 20;
    float y = 18;
    color = texture( renderedTexture, UV).rgb;
    /*
            UV + 0.01 * 
            vec2( sin(time+x*UV.x),cos(time+y*UV.y)) ).xyz;
            */
}
