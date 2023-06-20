#version 440 core

in vec2 uv;

uniform sampler2D image;
uniform vec4 outRect = vec4(1.f, 1.f, 1.f, 1.f);
uniform vec4 inRect = vec4(1.f, 0.f, 0.f, 1.f);

uniform float percentage = 0.75f;

out vec4 fragColor;

void main()
{
    vec3 color = texture(image, uv).xyz;
    float alpha = texture(image, uv).w;

    if (uv.y <= percentage)
        fragColor = vec4(color, alpha);
    else
        fragColor = vec4(vec3(1) - color, alpha);
}