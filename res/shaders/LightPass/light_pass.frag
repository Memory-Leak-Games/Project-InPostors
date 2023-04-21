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

    vec3 lightAmbient;// 16
    vec3 lightDiffuse;// 16
    vec3 lightSpecular;// 16

    mat4 light_lightSpaceMatrix;
};

uniform mat4 viewToLight;
uniform float fressnelPower = 5.f;

uniform bool isSSAOActive = false;

float CalculateShadow() {
    vec3 normal = normalize(texture(gNormal, fs_in.uv).rgb);
    vec3 position = texture(gPosition, fs_in.uv).rgb;
    vec4 positionInLightSpace = viewToLight * vec4(position, 1.f);

    vec3 projection = positionInLightSpace.xyz / positionInLightSpace.w;
    projection = projection * 0.5 + 0.5;

    float closestDepth = texture(shadowMap, projection.xy).r;
    float currentDepth = projection.z;

    float diffuseFactor = 1.f - dot(normal, normalize(-light_direction));

    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for (int x = -1; x <= 1; ++x) {
        for (int y = -1; y <= 1; ++y) {
            float pcfDepth = texture(shadowMap, projection.xy + vec2(x, y) * texelSize).r;
            shadow += mix(0.f, 1.f, float(currentDepth < pcfDepth));
        }
    }
    shadow /= 9.0;

    return shadow;
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

    // fresnell
    float fresnelFactor = dot(normal, light_direction);
    fresnelFactor = max(fresnelFactor, 0.0);
    fresnelFactor = 1.0 - fresnelFactor;
    fresnelFactor = pow(fresnelFactor, fressnelPower);

    vec3 materialSpecular = mix(albedo, vec3(1.f), fresnelFactor);

    // specular light
    vec3 reflectDirection = reflect(-lightDirection, normal);
    float calculated_specular = pow(max(dot(viewDirection, reflectDirection), 0.0), specular);

    float shadow = CalculateShadow();

    float ssaoValue = mix(1.f, texture(ssao, fs_in.uv).r, float(isSSAOActive));
    vec3 ambientColor = lightAmbient * albedo * ssaoValue;

    vec3 diffuseColor = lightDiffuse * diffuse * albedo * shadow * clamp(ssaoValue + 0.5f, 0.f, 1.f);
    vec3 specularColor = materialSpecular * lightSpecular * calculated_specular * shadow;
    return ambientColor + diffuseColor + specularColor;
}

void main()
{

    fragColor = vec4(CalculateDirectionalLight(), 1.0);
}
