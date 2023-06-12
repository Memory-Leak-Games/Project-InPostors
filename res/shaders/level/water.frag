#version 430 core

layout(location = 0) out vec3 gPosition;
layout(location = 1) out vec3 gNormal;
layout(location = 2) out vec4 gAlbedoSpec;

layout(std140, binding = 0) uniform CommonUnifomrs {
    mat4 projection;
    mat4 view;

    double seconds;
    float deltaSeconds;

    int randInt;
    float randFloat;
};

uniform sampler2D waterBase;
uniform sampler2D waterSurf;

uniform float tilingSize = 25.f;
uniform float surfSize = 1.1f;

uniform float surfStrength = 0.1f;

uniform vec2 baseSpeed = vec2(0.05, 0.01);
uniform vec2 surfSpeed = vec2(0.1, -0.01);

in VS_OUT {
    vec3 position;
    vec3 normal;
    vec2 uv;
}
fs_in;

vec2 GetBaseUV() {
    return fs_in.uv * tilingSize + float(seconds) * baseSpeed;
}

vec2 GetSurfUV() {
    return (fs_in.uv * tilingSize * surfSize ) + float(seconds) * surfSpeed;
}

void main() {
    gPosition = fs_in.position;
    gNormal = fs_in.normal;

    vec3 baseColor = texture(waterBase, GetBaseUV()).rgb;
    vec3 surfColor = texture(waterSurf, GetSurfUV()).rgb * surfStrength;

    gAlbedoSpec.rgb = baseColor + surfColor;
    gAlbedoSpec.a = 1.f;
}
