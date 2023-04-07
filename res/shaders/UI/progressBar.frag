#version 440 core

in vec2 uv;

uniform vec4 outRect = {1.f, 1.f, 1.f, 1.f};
uniform vec4 inRect = {1.f, 0.f, 0.f, 1.f};

uniform float percentage = 0.75f;

out vec4 fragColor;

void main()
{
    if (uv.x <= percentage)
        fragColor = inRect;
    else
        fragColor = outRect;
}