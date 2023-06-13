#version 440 core

in vec2 uv;

uniform sampler2D image;
uniform sampler2D values;
uniform vec4 outRect = vec4(0.f, 0.f, 0.f, 0.f);
uniform vec4 inRect = vec4(1.f, 0.f, 0.f, 1.f);

uniform float percentage = 1.0f;

out vec4 fragColor;

void main()
{
    float value = texture(values, uv).r;
    if (value <= percentage)
        fragColor = texture(image, uv) * inRect;
    else
        fragColor = texture(image, uv) * outRect;

//    vec2 pos = uv * 2.0 - 1;
//    float angle = degrees(atan(pos.x, pos.y));
//
//    angle = angle / 360.f;
//
//    if (angle <= percentage)
//        fragColor = texture(image, uv) * inRect;
//    else
//        fragColor = texture(image, uv) * outRect;

//    if (uv.x <= percentage)
//        fragColor = inRect;
//    else
//        fragColor = outRect;
}