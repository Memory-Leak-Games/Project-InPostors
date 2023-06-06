#version 440 core

in vec2 uv;

uniform vec4 tint = vec4(1.f);
uniform vec4 color = vec4(1.f);

out vec4 fragColor;

void main() {
    fragColor = color * tint;
}