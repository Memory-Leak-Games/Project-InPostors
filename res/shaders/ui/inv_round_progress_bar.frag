#version 440 core

in vec2 uv;

uniform sampler2D image;
uniform sampler2D values;
uniform vec4 outRect = vec4(0.f, 0.f, 0.f, 0.f);
uniform vec4 inRect = vec4(1.f, 0.f, 0.f, 1.f);
uniform vec4 tint = vec4(1.0, 1.0, 1.0, 1.0);

uniform float percentage = 1.0f;

out vec4 fragColor;

void main()
{
//    vec2 invUV = uv;
//    invUV.y = 1 - invUV.y;
    float value = texture(values, vec2(1) - uv).r;
    if (value <= percentage)
        fragColor = texture(image, uv) * inRect * tint;
    else
        fragColor = texture(image, uv) * outRect * tint;

}