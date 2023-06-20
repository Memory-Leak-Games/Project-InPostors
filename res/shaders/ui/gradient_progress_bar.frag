#version 440 core

in vec2 uv;

uniform vec4 outRect = vec4(0.f, 0.f, 0.f, 0.f);
uniform vec4 inRectFull = vec4(1.f, 1.f, 0.f, 1.f);
uniform vec4 inRectEmpty = vec4(1.f, 0.f, 0.f, 1.f);
uniform vec4 tint = vec4(1.f, 1.f, 1.f, 1.f);

uniform float percentage = 0.75f;

out vec4 fragColor;

void main()
{
    if (uv.x <= percentage)
        fragColor = mix(inRectEmpty, inRectFull, percentage) * tint;
    else
        fragColor = outRect * tint;
}