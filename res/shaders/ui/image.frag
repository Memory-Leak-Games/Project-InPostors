#version 440 core

uniform sampler2D image;

in vec2 uv;

out vec4 fragColor;

void main()
{
    fragColor = texture(image, uv);
}