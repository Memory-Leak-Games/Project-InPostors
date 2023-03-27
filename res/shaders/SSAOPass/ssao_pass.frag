#version 430 core

out float fragColor;

layout (binding=0) uniform sampler2D gPosition;
layout (binding=1) uniform sampler2D gNormal;
layout (binding=2) uniform sampler2D gAlbedoSpecular;

uniform sampler2D noise;

in VS_OUT {
    vec2 uv;
} fs_in;

uniform float farClip = 100.f;
uniform float radius = 0.3;
uniform float bias = 0.05;
uniform int samples = 8;

// https://www.shadertoy.com/view/4ltSz2
void main()
{
    vec4 norz = texture(gNormal, fs_in.uv);
    float depth = norz.w * farClip;
    float scale = radius / depth;

    float ao = 0.0;
    for(int i = 0; i < samples; i++)
    {
        vec2 randUv = fs_in.uv.xy + 23.71 * float(i);
        vec3 randNor = texture(noise, randUv).rgb * 2.0 - 1.0;
        if(dot(norz.xyz, randNor) < 0.0)
        randNor *= -1.0;

        vec2 off = randNor.xy * scale;
        vec4 sampleNorz = texture(gNormal, fs_in.uv + off);
        float depthDelta = depth - sampleNorz.w * farClip;

        vec3 sampleDir = vec3(randNor.xy * radius, depthDelta);
        float occ = max(0.0, dot(normalize(norz.xyz), normalize(sampleDir)) - bias) / (length(sampleDir) + 1.0);
        ao += 1.0 - occ;
    }

    ao /= float(samples);

    fragColor = ao;
}
