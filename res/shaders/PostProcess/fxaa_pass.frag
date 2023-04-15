#version 440

layout (binding = 0) uniform sampler2D inTexture;

layout (std140, binding = 0) uniform CommonUnifomrs {
    mat4 projection;
    mat4 view;

    double seconds;
    float deltaSeconds;

    int randInt;
    float randFloat;
    ivec2 resolution;
};

in VS_OUT {
    vec2 uv;
} fs_in;

out vec4 fragColor;

#define FXAA_SPAN_MAX     8.f
#define FXAA_REDUCE_MUL   1.f / FXAA_SPAN_MAX
#define FXAA_REDUCE_MIN   1.f / 128.f
#define FXAA_SUBPIX_SHIFT 1.f / 4.f

void main() {
    vec2 rcpFrame = 1.f / resolution;
    vec2 uv2 = fs_in.uv;
    vec4 uv = vec4(uv2, uv2 - (rcpFrame * (0.5 + FXAA_SUBPIX_SHIFT)));

    vec3 rgbNW = texture(inTexture, uv.zw).xyz;
    vec3 rgbNE = texture(inTexture, uv.zw + vec2(1, 0) * rcpFrame.xy).xyz;
    vec3 rgbSW = texture(inTexture, uv.zw + vec2(0, 1) * rcpFrame.xy).xyz;
    vec3 rgbSE = texture(inTexture, uv.zw + vec2(1, 1) * rcpFrame.xy).xyz;
    vec3 rgbM = texture(inTexture, uv.xy).xyz;

    vec3 luma = vec3(0.299, 0.587, 0.114);
    float lumaNW = dot(rgbNW, luma);
    float lumaNE = dot(rgbNE, luma);
    float lumaSW = dot(rgbSW, luma);
    float lumaSE = dot(rgbSE, luma);
    float lumaM = dot(rgbM, luma);

    float lumaMin = min(lumaM, min(min(lumaNW, lumaNE), min(lumaSW, lumaSE)));
    float lumaMax = max(lumaM, max(max(lumaNW, lumaNE), max(lumaSW, lumaSE)));

    vec2 dir;
    dir.x = -((lumaNW + lumaNE) - (lumaSW + lumaSE));
    dir.y = ((lumaNW + lumaSW) - (lumaNE + lumaSE));

    float dirReduce = max(
        (lumaNW + lumaNE + lumaSW + lumaSE) * (0.25 * FXAA_REDUCE_MUL),
        FXAA_REDUCE_MIN);
    float rcpDirMin = 1.0 / (min(abs(dir.x), abs(dir.y)) + dirReduce);

    dir = min(vec2(FXAA_SPAN_MAX, FXAA_SPAN_MAX),
              max(vec2(-FXAA_SPAN_MAX, -FXAA_SPAN_MAX),
                  dir * rcpDirMin)) * rcpFrame.xy;

    vec3 rgbA = (1.0 / 2.0) * (
    texture(inTexture, uv.xy + dir * (1.0 / 3.0 - 0.5)).xyz +
    texture(inTexture, uv.xy + dir * (2.0 / 3.0 - 0.5)).xyz);

    vec3 rgbB = rgbA * (1.0 / 2.0) + (1.0 / 4.0) * (
    texture(inTexture, uv.xy + dir * (0.0 / 3.0 - 0.5)).xyz +
    texture(inTexture, uv.xy + dir * (3.0 / 3.0 - 0.5)).xyz);

    float lumaB = dot(rgbB, luma);

    if ((lumaB < lumaMin) || (lumaB > lumaMax)) fragColor = vec4(rgbA, 1.f);

    fragColor = vec4(rgbA, 1.f);
}