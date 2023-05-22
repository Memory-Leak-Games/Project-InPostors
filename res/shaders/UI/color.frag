#version 440 core

in vec2 uv;

uniform vec4 color;

out vec4 fragColor;

void main()
{
    fragColor = color;
}