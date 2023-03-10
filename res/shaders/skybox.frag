#version 430 core
out vec4 fragColor;

uniform samplerCube cubemap;

in VS_OUT {
    vec3 texCoords;
} fs_in;

void main()
{
    fragColor = texture(cubemap, fs_in.texCoords);
}