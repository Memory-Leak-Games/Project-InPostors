#version 440 core

out vec4 fragColor;

layout (binding=0) uniform sampler2D gPosition;
layout (binding=1) uniform sampler2D gNormal;
layout (binding=2) uniform sampler2D gAlbedoSpecular;

layout (binding=3) uniform sampler2D ssao;

in VS_OUT {
    vec2 uv;
} fs_in;

layout(std140, binding = 0) uniform TransformationMatrices {
    mat4 projection;
    mat4 view;
    vec3 viewPosition;
};

layout(std140, binding = 1) uniform light {
    vec3 light_direction;// 16

    vec3 light_ambient;// 16
    vec3 light_diffuse;// 16
    vec3 light_specular;// 16
};


vec3 CalculateDirectionalLight() {
    vec3 fragPos = texture(gPosition, fs_in.uv).rgb;
    vec3 normal = texture(gNormal, fs_in.uv).rgb;
    vec3 albedo = texture(gAlbedoSpecular, fs_in.uv).rgb;
    float specular = texture(gAlbedoSpecular, fs_in.uv).a;

    vec3 lightDirection = normalize(-light_direction);
    vec3 viewDirection = normalize(viewPosition - fragPos);

    // diffuse light
    float diffuse = max(dot(normal, lightDirection), 0.0);

    // specular light
    vec3 reflectDirection = reflect(-lightDirection, normal);
    float calculated_specular = pow(max(dot(viewDirection, reflectDirection), 0.0), specular);

    vec3 ambientColor = light_ambient * albedo * texture(ssao, fs_in.uv).r;
    vec3 diffuseColor = light_diffuse * diffuse * albedo;
    vec3 specularColor = light_specular * calculated_specular;
    return ambientColor + diffuseColor + specularColor;
}

void main()
{
    fragColor = vec4(CalculateDirectionalLight(), 1.0);
//    fragColor = vec4(vec3(texture(ssao, fs_in.uv).x), 1.0);
}
