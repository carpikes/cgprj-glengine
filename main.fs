#version 330 core

in vec2 UV;
out vec3 oColor;

uniform sampler2D uTexture;

void main() {
    oColor = texture(uTexture, UV).rgb;
}
