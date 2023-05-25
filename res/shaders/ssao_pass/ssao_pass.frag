#version 430 core

out float fragColor;

layout (binding = 0) uniform sampler2D gPosition;
layout (binding = 1) uniform sampler2D gNormal;

layout (std140, binding = 0) uniform CommonUnifomrs {
    mat4 projection;
    mat4 view;

    double seconds;
    float deltaSeconds;

    int randInt;
    float randFloat;
    ivec2 resolution;
};

in VS_OUT {
    vec2 uv;
} fs_in;

uniform int numberOfSamples = 16;
uniform float radius = 0.6;
uniform float bias = 0.005; // reduce acne
uniform float magnitude = 1.1; // smaller - darker
uniform float contrast = 1.1; //

#define MAX_SAMPLES 64

uniform vec3 samples[MAX_SAMPLES];
uniform vec3 noise[MAX_SAMPLES];

vec3 GetNoise(vec2 uv) {
    int noiseWidith = int(sqrt(float(numberOfSamples)));
    int noiseX = int(uv.x * resolution.x) % noiseWidith;
    int noiseY = int(uv.y * resolution.y) % noiseWidith;
    int noiseIndex = noiseX + (noiseY * noiseWidith);

    return noise[noiseIndex];
}

void main() {
    vec2 uv = fs_in.uv;

    // get input for SSAO algorithm
    vec3 fragPos = texture(gPosition, uv).xyz;
    vec3 normal = normalize(texture(gNormal, uv).rgb);
    vec3 randomVec = GetNoise(uv);

    // create TBN change-of-basis matrix: from tangent-space to view-space
    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);

    // iterate over the sample kernel and calculate occlusion factor
    float occlusion = 0.0;
    for (int i = 0; i < numberOfSamples; ++i)
    {
        // get sample position
        vec3 samplePos = TBN * samples[i]; // from tangent to view-space
        samplePos = fragPos + samplePos * radius;

        // project sample position (to sample texture) (to get position on screen/texture)
        vec4 offset = vec4(samplePos, 1.0);
        offset = projection * offset; // from view to clip-space
        offset.xyz /= offset.w; // perspective divide
        offset.xyz = offset.xyz * 0.5 + 0.5; // transform to range 0.0 - 1.0

        // get sample depth
        float sampleDepth = texture(gPosition, offset.xy).z; // get depth value of kernel sample

        // range check & accumulate
        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - sampleDepth));
        occlusion += (sampleDepth >= samplePos.z + bias ? 1.0 : 0.0) * rangeCheck;
    }
    occlusion /= numberOfSamples;
    occlusion = pow(occlusion, magnitude);
    occlusion = 1.0 - (occlusion);

    fragColor = (occlusion - 0.5f) * max(contrast, 0.f) + 0.5f;
}