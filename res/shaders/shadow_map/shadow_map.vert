#version 430 core

layout (location = 0) in vec3 aPos;

layout(std140, binding = 1) uniform light {
    vec3 light_direction;// 16

    vec3 light_ambient;// 16
    vec3 light_diffuse;// 16
    vec3 light_specular;// 16

    mat4 light_lightSpaceMatrix;
};

uniform mat4 modelToLight;

void main() {
    gl_Position = modelToLight * vec4(aPos, 1.0);
}
