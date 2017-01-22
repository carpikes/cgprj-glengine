#version 330 core

in vec2 UV;

out vec3 color;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;
uniform float time;

void main() {
    vec2 ruv = vec2(fract(UV.x * 2), fract(UV.y * 2));

    if(UV.x < 0.5)
        if(UV.y > 0.5)
            color = texture( gPosition, ruv).rgb;
        else
            color = texture( gNormal, ruv).rgb;
    else
        if(UV.y > 0.5)
            color = texture( gAlbedo, ruv).rgb;
        else
            color = vec3(texture( gAlbedo, ruv).r);
}
