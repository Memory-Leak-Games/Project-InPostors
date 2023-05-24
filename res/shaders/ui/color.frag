#version 440 core

in vec2 uv;

uniform vec4 tint;

out vec4 fragColor;

void main()
{
    fragColor = tint;
}