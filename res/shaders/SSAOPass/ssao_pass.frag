#version 430 core

out vec4 fragColor;

layout (binding = 0) uniform sampler2D gPosition;
layout (binding = 1) uniform sampler2D gNormal;
layout (binding = 2) uniform sampler2D gAlbedoSpecular;

in VS_OUT {
    vec2 uv;
} fs_in;

#define SAMPLES 16
#define INTENSITY 24.
#define SCALE 10
#define BIAS 0.05
#define SAMPLE_RAD 0.5
#define MAX_DISTANCE 2.

#define MOD3 vec3(.1031, .11369, .13787)

float Hash12(vec2 position)
{
    vec3 p3 = fract(vec3(position.xyx) * MOD3);
    p3 += dot(p3, p3.yzx + 19.19);
    return fract((p3.x + p3.y) * p3.z);
}

float DoAmbientOcclusion(in vec2 tcoord, in vec2 uv, in vec3 p, in vec3 cnorm)
{
    vec3 diff = texture(gPosition, (tcoord + uv)).rgb - p;
    float l = length(diff);
    vec3 v = diff / l;
    float d = l * SCALE;
    float ao = max(0.0, dot(cnorm, v) - BIAS) * (1.0 / (1.0 + d));
    ao *= smoothstep(MAX_DISTANCE, MAX_DISTANCE * 0.5, l);
    return ao;

}

float SpiralAO()
{
    vec2 uv = fs_in.uv;
    vec3 position = texture(gPosition, uv).rgb;
    vec3 normal = texture(gNormal, uv).rgb;

    float rad = SAMPLE_RAD / position.z;

    float goldenAngle = 2.4;
    float ao = 0.;
    float inv = 1. / float(SAMPLES);
    float radius = 0.;

    float rotatePhase = Hash12(uv * 100.) * 6.28;
    float rStep = inv * rad;
    vec2 spiralUV;

    for (int i = 0; i < SAMPLES; i++) {
        spiralUV.x = sin(rotatePhase);
        spiralUV.y = cos(rotatePhase);
        radius += rStep;
        ao += DoAmbientOcclusion(uv, spiralUV * radius, position, normal);
        rotatePhase += goldenAngle;
    }
    ao *= inv;
    return ao;
}

void main()
{
    float ao = 0.;

    ao = SpiralAO();

    ao = 1. - ao * INTENSITY;

    fragColor = vec4(ao, ao, ao, 1.);
}
