#version 430 core

// rect
layout(location = 0) in vec2 rectPosition;
layout(location = 1) in vec2 uv;

// particle
layout(location = 3) in vec3 particlePosition;
layout(location = 4) in vec2 particleSize;
layout(location = 5) in vec4 particleColor;
layout(location = 6) in float particleRotation;

layout(std140, binding = 0) uniform CommonUnifomrs {
    mat4 projection;
    mat4 view;

    double seconds;
    float deltaSeconds;

    int randInt;
    float randFloat;
};

out VS_OUT {
    vec4 color;
    vec2 uv;
} vs_out;

void main() {
    vec3 cameraRight = vec3(view[0][0], view[1][0], view[2][0]);
    vec3 cameraUp = vec3(view[0][1], view[1][1], view[2][1]);

    vec3 position = particlePosition
    + cameraRight * rectPosition.x * particleSize.x
    + cameraUp * rectPosition.y * particleSize.y;

    gl_Position = projection * view * vec4(position, 1.f);

    vs_out.color = particleColor;
    vs_out.uv = uv;
}
