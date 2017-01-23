#version 330 core

in vec2 UV;

out vec3 color;

uniform sampler2D renderedTexture;
uniform float exposure;

void main() {
    const float gamma = 2.2;
    vec3 hdrColor = texture( renderedTexture, UV).rgb;

    vec3 mapped = vec3(1.0) - exp(-hdrColor * exposure);

    mapped = pow(mapped, vec3(1.0 / gamma));

    color = mapped;
}
