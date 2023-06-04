#version 330 core

in vec2 TexCoords;
in float correction;
out vec4 color;

uniform sampler2D text;
uniform vec3 textColor;

// No-if-inator
float when_lt(float x, float y) {
    return max(sign(y - x), 0.0);
}

// No-if-inator 2
float when_ge(float x, float y) {
    return 1.0 - when_lt(x, y);
}

void main()
{
    float cutoff = 0.5f;
    float dist = (cutoff - texture(text, TexCoords).r);

    vec2 ddist = vec2(dFdx(dist), dFdy(dist));

    float pixelDist = dist / length(ddist);

    color = vec4(textColor, clamp(0.5 - pixelDist, 0.0, 1.0));

    // Smooth out glyph using MUAA (Made Up AntiAliasing)
//    glyphShape *= when_ge(glyphShape, 0.492);
//    glyphShape += (1 - glyphShape) * when_ge(glyphShape, (0.525 - correction));
//
//    color = vec4(textColor, glyphShape);



//  if (glyphShape < 0.49)
//      discard;
//  if(glyphShape >= (0.55 - correction))
//      glyphShape = 1;
}

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