#version 430 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;

layout(std140, binding = 0) uniform TransformationMatrices {
    mat4 projection;
    mat4 view;
    vec3 viewPosition;
};

uniform mat4 world;

out VS_OUT {
    vec3 position;
    vec3 normal;
    vec2 uv;
} vs_out;

void main() {
    mat4 viewWorld = view * world;

    vec4 positionInViewSpace = viewWorld * vec4(position, 1.0f);

    // Send data to Fragment shader
    vs_out.uv = uv;
    vs_out.position = vec3(positionInViewSpace);
    mat3 viewWorldNormalMatrix = mat3(transpose(inverse(viewWorld)));
    vs_out.normal = normalize(viewWorldNormalMatrix * normal);

    // Set vertex position
    gl_Position = projection * positionInViewSpace;
}
