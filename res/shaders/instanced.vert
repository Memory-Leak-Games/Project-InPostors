#version 430 core

layout(location = 0) in vec3 Position;
layout(location = 1) in vec3 Normal;
layout(location = 2) in vec2 TexCoord;

layout(location = 3) in mat4 Transform;

layout(std140, binding = 0) uniform TransformationMatrices {
    mat4 Projection;
    mat4 View;
    vec3 ViewPosition;
};

out VS_OUT {
    vec3 Position;
    vec3 Normal;
    vec2 TexCoord;

    vec3 ViewPosition;
} vs_out;

void main() {
    gl_Position = Projection * View * Transform * vec4(Position, 1.0f);
    vs_out.TexCoord = TexCoord;
    vs_out.Position = vec3(Transform * vec4(Position, 1.0f));
    vs_out.Normal = normalize(mat3(transpose(inverse(Transform))) * Normal);

    vs_out.ViewPosition = ViewPosition;
}
