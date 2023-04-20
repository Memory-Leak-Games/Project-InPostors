#version 330 core

in vec2 TexCoords;
out vec4 color;

uniform sampler2D text;
uniform vec3 textColor;

void main()
{
    //vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);
    //color = vec4(textColor, 1.0) * sampled;

    float glyphShape = texture(text, TexCoords).r;

    // Outline test
/*
    if (glyphShape < 0.01)
    discard;

    if(glyphShape < 0.8)
    {
        color = vec4(0, 0, 0, 1);
        return;
    }


   if(glyphShape >= 0.8)
    glyphShape = 1;

    color = vec4(textColor, glyphShape);*/

    if (glyphShape < 0.48)
    discard;

   if(glyphShape >= 0.6)
    glyphShape = 1;

    color = vec4(textColor, glyphShape);
}