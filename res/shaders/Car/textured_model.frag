#version 430 core

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

uniform sampler2D textureDiffuse;

uniform float shininess = 0.5;
uniform vec4 color = vec4(1., 0., 1., 1.);

#define CHROMA_KEY_COLOR vec3(1., 0., 1.)

in VS_OUT {
    vec3 position;
    vec3 normal;
    vec2 uv;
} fs_in;

vec3 ApplyCarColor() {
    vec3 textureColor = texture(textureDiffuse, fs_in.uv).rgb;
    return mix(textureColor, color.rgb, float(textureColor == CHROMA_KEY_COLOR));
}

void main() {
    gPosition = fs_in.position;
    gNormal = fs_in.normal;

    gAlbedoSpec.rgb = ApplyCarColor();

    gAlbedoSpec.a = shininess;
}
