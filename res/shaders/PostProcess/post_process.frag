#version 430 core

uniform sampler2D ColorTexture;
uniform sampler2D DepthStencilTexture;

out vec4 fragColor;

in VS_OUT {
    vec2 texCoord;
} fs_in;

void main() {
    vec4 color = texture(ColorTexture, fs_in.texCoord);
    float average = color.r + color.g + color.b / 3.f;

    fragColor = vec4(vec3(average), 1.f);
}
