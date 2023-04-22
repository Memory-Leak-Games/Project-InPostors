#version 440 core

layout (location = 0) in vec4 vertex; // <vec2 pos, vec2 tex>
out vec2 TexCoords;

uniform mat4 projection;
uniform vec2 pos;
uniform vec2 size;

void main()
{
    gl_Position = projection * vec4(vertex.xy * size + pos, 0.0, 1.0);
    TexCoords = vertex.zw;
}