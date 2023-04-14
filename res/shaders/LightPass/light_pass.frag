#version 440 core

out vec4 fragColor;

layout (binding = 0) uniform sampler2D gPosition;
layout (binding = 1) uniform sampler2D gNormal;
layout (binding = 2) uniform sampler2D gAlbedoSpecular;

layout (binding = 3) uniform sampler2D shadowMap;
layout (binding = 4) uniform sampler2D ssao;

in VS_OUT {
    vec2 uv;
} fs_in;

layout (std140, binding = 0) uniform CommonUnifomrs {
    mat4 projection;
    mat4 view;

    double seconds;
    float deltaSeconds;

    int randInt;
    float randFloat;
};

layout (std140, binding = 1) uniform light {
    vec3 light_direction;// 16

    vec3 light_ambient;// 16
    vec3 light_diffuse;// 16
    vec3 light_specular;// 16

    mat4 light_lightSpaceMatrix;
};

uniform mat4 viewToLight;
uniform float maxBias = 0.005f;
uniform float minBias = 0.f;
uniform bool isSSAOActive = false;

float CalculateShadow() {
    vec3 normal = normalize(texture(gNormal, fs_in.uv).rgb);
    vec3 position = texture(gPosition, fs_in.uv).rgb;
    vec4 positionInLightSpace = viewToLight * vec4(position, 1.f);

    vec3 projection = positionInLightSpace.xyz / positionInLightSpace.w;
    projection = projection * 0.5 + 0.5;

    float closestDepth = texture(shadowMap, projection.xy).r;
    float currentDepth = projection.z;

    float diffuseFactor = dot(normal, normalize(light_direction));
    float biasFactor = mix(minBias, maxBias, diffuseFactor);

    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for (int x = -1; x <= 1; ++x) {
        for (int y = -1; y <= 1; ++y) {
            float pcfDepth = texture(shadowMap, projection.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - biasFactor > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;

    return 1 - shadow;
}

vec3 CalculateDirectionalLight() {
    vec3 fragPos = texture(gPosition, fs_in.uv).rgb;
    vec3 normal = texture(gNormal, fs_in.uv).rgb;
    vec3 albedo = texture(gAlbedoSpecular, fs_in.uv).rgb;
    float specular = texture(gAlbedoSpecular, fs_in.uv).a;

    vec3 lightDirection = normalize(-light_direction);
    mat3 viewNormalMatrix = mat3(transpose(inverse(view)));
    lightDirection = normalize(viewNormalMatrix * lightDirection);

    vec3 viewDirection = normalize(-fragPos);

    // diffuse light
    float diffuse = max(dot(normal, lightDirection), 0.0);

    // specular light
    vec3 reflectDirection = reflect(-lightDirection, normal);
    float calculated_specular = pow(max(dot(viewDirection, reflectDirection), 0.0), specular);

    float shadow = CalculateShadow();

    vec3 ambientColor = light_ambient * albedo;
    vec3 diffuseColor = light_diffuse * diffuse * albedo * shadow;
    vec3 specularColor = light_specular * calculated_specular * shadow;
    return ambientColor + diffuseColor + specularColor;
}

void main()
{
    float ssaoValue = mix(1.f, texture(ssao, fs_in.uv).r, float(isSSAOActive));

    fragColor = vec4(CalculateDirectionalLight() * ssaoValue, 1.0);
}
