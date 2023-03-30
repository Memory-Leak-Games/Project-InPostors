#version 440 core

out vec4 fragColor;

layout (binding=0) uniform sampler2D gPosition;
layout (binding=1) uniform sampler2D gNormal;
layout (binding=2) uniform sampler2D gAlbedoSpecular;

layout (binding=3) uniform sampler2D ssao;

in VS_OUT {
    vec2 uv;
} fs_in;

layout(std140, binding = 0) uniform CommonUnifomrs {
    mat4 projection;
    mat4 view;

    float seconds;
    float deltaSeconds;

    int randInt;
    float randFloat;
};

layout(std140, binding = 1) uniform light {
    vec3 light_direction;// 16

    vec3 light_ambient;// 16
    vec3 light_diffuse;// 16
    vec3 light_specular;// 16

    mat4 light_lightSpaceMatrix;
};


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

    vec3 ambientColor = light_ambient * albedo;
    vec3 diffuseColor = light_diffuse * diffuse * albedo;
    vec3 specularColor = light_specular * calculated_specular;
    return ambientColor + diffuseColor + specularColor;
}

void main()
{
    fragColor = vec4(CalculateDirectionalLight() * vec3(texture(ssao, fs_in.uv).r), 1.0);
}