#version 430 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;

layout(std140, binding = 0) uniform CommonUnifomrs {
    mat4 projection;
    mat4 view;

    double seconds;
    float deltaSeconds;

    int randInt;
    float randFloat;
};

uniform mat4 world;

out VS_OUT {
    vec3 position;
    vec3 normal;
    vec2 uv;
}
vs_out;

float hash(float p) {
    p = fract(p * 0.011);
    p *= p + 7.5;
    p *= p + p;
    return fract(p);
}

float hash(vec2 p) {
    vec3 p3 = fract(vec3(p.xyx) * 0.13);
    p3 += dot(p3, p3.yzx + 3.333);
    return fract((p3.x + p3.y) * p3.z);
}

float GetRandomFloat(float x) {
    float i = floor(x);
    float f = fract(x);
    float u = f * f * (3.0 - 2.0 * f);
    return mix(hash(i), hash(i + 1.0), u);
}

vec3 ComputePosition() {
    const float t = float(seconds) * 2.f;
    const float maxOffset = 0.2f;
    const float maxRandOffset = 0.3f;
    const float noiseSpeed = 0.3;

    float height = position.y;
    vec3 offset = vec3(1.f, 0.f, 1.f) * (height / 4.f) * sin(height + float(t)) * maxOffset;
    // vec3 offset = vec3(0.f);

    vec3 worldPosition = vec3(world[3]);

    vec3 randomOffset = vec3(0.f);
    randomOffset.x = GetRandomFloat((t + worldPosition.z) * noiseSpeed);
    randomOffset.z = GetRandomFloat((t + worldPosition.x) * noiseSpeed);
    offset += randomOffset * maxRandOffset * (height / 4.f);

    return position + offset;
}

void main() {
    mat4 model2View = view * world;
    mat3 model2ViewNormals = mat3(transpose(inverse(model2View)));
    mat4 model2Screen = projection * model2View;

    vec3 computedPosition = ComputePosition();

    vec4 positionInViewSpace = model2View * vec4(computedPosition, 1.0f);

    // Send data to Fragment shader
    vs_out.uv = uv;
    vs_out.position = vec3(positionInViewSpace);
    vs_out.normal = normalize(model2ViewNormals * normal);

    // Set vertex position
    gl_Position = model2Screen * vec4(computedPosition, 1.0f);
}
