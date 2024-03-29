#version 430 core

out float fragColor;

layout (binding = 0) uniform sampler2D ssaoInput;

in VS_OUT {
    vec2 uv;
} fs_in;

void main()
{
    vec2 texelSize = 1.0 / vec2(textureSize(ssaoInput, 0));
    float result = 0.0;
    for (int x = -2; x < 2; ++x)
    {
        for (int y = -2; y < 2; ++y)
        {
            vec2 offset = vec2(float(x), float(y)) * texelSize;
            result += texture(ssaoInput, fs_in.uv + offset).r;
        }
    }
    fragColor = result / (4.0 * 4.0);
}  