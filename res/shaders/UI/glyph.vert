#version 440 core

layout (location = 0) in vec2 vertex;
out vec2 TexCoords;
out float correction;

uniform mat4 projection;
uniform vec2 pos;
uniform vec2 size;

void main()
{
    gl_Position = projection * vec4((vertex.xy + vec2(0.5, 0.5)) * size + pos, 0.0, 1.0);
    TexCoords = vertex + vec2(0.5, 0.5);
    TexCoords.y = 1 - TexCoords.y;

    // Used to tweak antialiasing
    correction = size.y * 0.0004;
}