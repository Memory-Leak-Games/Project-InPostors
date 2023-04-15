#version 440

layout (binding=0) uniform sampler2D inTexture;

in VS_OUT {
    vec2 uv;
} fs_in;

out vec4 fragColor;

void main() {
    fragColor = texture(inTexture, fs_in.uv);
}