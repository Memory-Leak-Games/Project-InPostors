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

uniform mat4 modelToView;
uniform mat4 modelToScreen;
uniform mat3 modelToViewNormals;

out VS_OUT {
    vec3 position;
    vec3 normal;
    vec2 uv;
} vs_out;

void main() {
    vec4 positionInViewSpace = modelToView * vec4(position, 1.0f);

    // Send data to Fragment shader
    vs_out.uv = uv;
    vs_out.position = vec3(positionInViewSpace);
    vs_out.normal = normalize(modelToViewNormals * normal);

    // Set vertex position
    gl_Position = modelToScreen * vec4(position, 1.0f);
}
