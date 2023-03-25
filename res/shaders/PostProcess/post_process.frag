#version 440 core

layout (binding=0) uniform sampler2D  ColorTexture;
layout (binding=1) uniform sampler2D  DepthStencilTexture;


out vec4 fragColor;

in VS_OUT {
    vec2 texCoord;
} fs_in;

void main() {
    fragColor = texture(ColorTexture, fs_in.texCoord);
}
