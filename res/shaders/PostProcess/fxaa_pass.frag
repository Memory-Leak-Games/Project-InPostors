#version 440

layout (binding=0) uniform sampler2D inTexture;

uniform float resolution;
uniform float radius;
uniform vec2 dir;

in VS_OUT {
    vec2 uv;
} fs_in;

out vec4 fragColor;

void main() {
    fragColor = texture(inTexture, fs_in.uv).grba;
}
