#version 430 core

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

uniform sampler2D textureDiffuse;

uniform float shininess = 0.5;
uniform float brightness = 1.0;
uniform vec4 tint = vec4(1., 1., 1., 1.);

in VS_OUT {
    vec3 position;
    vec3 normal;
    vec2 uv;
} fs_in;

void main() {
    gPosition = fs_in.position;
    gNormal = fs_in.normal;
    gAlbedoSpec.rgb = texture(textureDiffuse, fs_in.uv).rgb * brightness * tint.rgb;
    gAlbedoSpec.a = shininess;
}
