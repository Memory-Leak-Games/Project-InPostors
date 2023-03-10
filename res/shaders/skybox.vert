#version 430 core

layout(location = 0) in vec3 position;

layout(std140, binding = 0) uniform transformationMatrices {
    mat4 projection;
    mat4 view;
    vec3 ViewPosition;
};

out VS_OUT {
    vec3 texCoords;
} vs_out;

void main() {
    vs_out.texCoords = position;
    mat4 viewWithoutTranslation = mat4(mat3(view));
    vec4 tempPosition = projection * viewWithoutTranslation * vec4(position, 1.0);
    gl_Position = tempPosition.xyww;
}