#version 430 core

layout(location = 0) in vec3 Position;

layout(std140, binding = 0) uniform CommonUnifomrs {
    mat4 projection;
    mat4 view;

    float seconds;
    float deltaSeconds;

    int randInt;
    float randFloat;
};

uniform mat4 world;
uniform bool alwaysFront;
uniform float pointSize;

void main() {
    gl_Position = projection * view * world * vec4(Position, 1.0f);
    if(alwaysFront)
    {
        gl_Position.z = -gl_Position.w;
    }

    // Only used by point gizmo
    gl_PointSize = pointSize;
}
