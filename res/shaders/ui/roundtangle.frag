#version 440 core

uniform vec4 tint = vec4(1.0, 1.0, 1.0, 1.0);
uniform float radius = 1.0f;

in vec2 uv;
in vec2 pos;
in vec2 Size;

out vec4 fragColor;

void main()
{
    // Rounded oifjweiofjwpfjepwojepojf
    float d = length(max(abs(uv - pos), Size) - Size) - radius;
    fragColor = max(uv.x, uv.y) > 0.5f ? tint : vec4(0.0);
}