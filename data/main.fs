#version 330 core

in vec2 UV;
in vec3 NORMAL;
out vec4 oColor;

uniform sampler2D uTexture;
uniform vec3 uLight;

void main() {
    vec4 t = texture(uTexture, UV).rgba;
    vec4 l = clamp(t * dot(NORMAL, uLight), 0, 1);
    oColor = min(l* 0.95 + t * 0.05, vec4(1.0));
}
