#version 440 core

in vec2 uv;

uniform sampler2D image;
uniform vec4 outRect = vec4(1.f, 1.f, 1.f, 1.f);
uniform vec4 inRect = vec4(1.f, 0.f, 0.f, 1.f);
uniform vec4 tint = vec4(1.0, 1.0, 1.0, 1.0);

uniform float percentage = 0.75f;

out vec4 fragColor;

void main()
{
    if (uv.y <= percentage)
        fragColor = texture(image, uv) * inRect * tint;
    else
        fragColor = texture(image, uv) * outRect * tint;
}