#version 430 core

layout(location = 0) in vec3 Position;

layout(std140, binding = 0) uniform TransformationMatrices {
    mat4 Projection;
    mat4 View;
    vec3 ViewPosition;
};

uniform mat4 world;
uniform bool alwaysFront;
uniform float pointSize;

void main() {
    gl_Position = Projection * View * world * vec4(Position, 1.0f);
    if(alwaysFront)
    {
        gl_Position.z = -gl_Position.w;
    }

    // Only used by point gizmo
    gl_PointSize = pointSize;
}
