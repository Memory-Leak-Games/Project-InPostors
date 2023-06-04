#version 440 core

uniform sampler2D image;
uniform vec4 tint = vec4(1.0, 1.0, 1.0, 1.0);

in vec2 uv;

out vec4 fragColor;

void main()
{
    fragColor = texture(image, uv) * tint;
}