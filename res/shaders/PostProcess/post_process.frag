#version 440 core

layout (binding=0) uniform sampler2D colorTexture;
uniform sampler2D whiteNoise;

out vec4 fragColor;

uniform float Gamma = 2.2;
uniform float Brightness = 0.0;
uniform float Contrast = 1.0;
uniform float Saturation = 1.0;

mat4 BrightnessMatrix(float brightness)
{
    return mat4(1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 1, 0,
    brightness, brightness, brightness, 1);
}

mat4 ContrastMatrix(float contrast)
{
    float t = (1.0 - contrast) / 2.0;

    return mat4(contrast, 0, 0, 0,
    0, contrast, 0, 0,
    0, 0, contrast, 0,
    t, t, t, 1);

}

mat4 SaturationMatrix(float saturation)
{
    vec3 luminance = vec3(0.3086, 0.6094, 0.0820);

    float oneMinusSat = 1.0 - saturation;

    vec3 red = vec3(luminance.x * oneMinusSat);
    red+= vec3(saturation, 0, 0);

    vec3 green = vec3(luminance.y * oneMinusSat);
    green += vec3(0, saturation, 0);

    vec3 blue = vec3(luminance.z * oneMinusSat);
    blue += vec3(0, 0, saturation);

    return mat4(red, 0,
    green, 0,
    blue, 0,
    0, 0, 0, 1);
}

in VS_OUT {
    vec2 uv;
} fs_in;

vec4 GammaCorection(vec4 color) {
    color.rgb = pow(color.rgb, vec3(1.0/Gamma));
    return color;
}

void main() {
    vec4 textureColor = texture(colorTexture, fs_in.uv);

    fragColor = BrightnessMatrix(Brightness) *
    ContrastMatrix(Contrast) *
    SaturationMatrix(Saturation) *
    GammaCorection(textureColor);
}
