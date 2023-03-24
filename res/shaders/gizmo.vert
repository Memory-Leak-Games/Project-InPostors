#version 430 core

layout(location = 0) in vec3 Position;

layout(std140, binding = 0) uniform TransformationMatrices {
    mat4 Projection;
    mat4 View;
    vec3 ViewPosition;
};

uniform mat4 World;

void main() {
    gl_Position = Projection * View * World * vec4(Position, 1.0f);
}
