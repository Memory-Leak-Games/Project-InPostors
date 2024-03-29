#version 430 core

layout(location = 0) in vec2 position;

uniform mat4 projection;

uniform vec2 size;
uniform vec2 screenPosition;

out vec2 uv;
out vec2 pos;
out vec2 Size;

void main() {
    vec2 outPosition = position;
    outPosition.x *= size.x;
    outPosition.y *= size.y;

    outPosition += screenPosition;

    gl_Position = projection * vec4(outPosition, 0.0, 1.0);
    uv = position + vec2(0.5, 0.5);
    pos = screenPosition;
    Size = size;
}
