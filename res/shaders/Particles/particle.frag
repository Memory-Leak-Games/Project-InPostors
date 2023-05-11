#version 440 core

uniform sampler2D image;

in VS_OUT {
    vec4 color;
    vec2 uv;
} fs_in;

out vec4 fragColor;

void main()
{
    if(texture(image, fs_in.uv).a == 0.f)
        discard;

    fragColor = texture(image, fs_in.uv) * fs_in.color;
}