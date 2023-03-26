#version 430 core

in vec3 NormalVector;
in vec2 TexCoordFragment;

uniform sampler2D textureDiffuse;

uniform float shininess = 0.5;
uniform float brightness = 1.0;
uniform vec4 tint = vec4(1., 1., 1., 1.);

out vec4 fragColor;

layout(std140, binding = 1) uniform light {
    struct {
        vec3 direction;// 16

        vec3 ambient;// 16
        vec3 diffuse;// 16
        vec3 specular;// 16
    } directionalLight;
};

in VS_OUT {
    vec3 position;
    vec3 normal;
    vec2 uv;

    vec3 viewPosition;
} fs_in;

vec4 CalculateDirectionalLight() {
    vec3 lightDirection = normalize(-directionalLight.direction);
    vec3 viewDirection = normalize(fs_in.viewPosition - fs_in.position);

    // diffuse light
    float diffuse = max(dot(fs_in.normal, lightDirection), 0.0);

    // specular light
    vec3 reflectDirection = reflect(-lightDirection, fs_in.normal);
    float specular = pow(max(dot(viewDirection, reflectDirection), 0.0), shininess);

    vec3 ambientColor = directionalLight.ambient * vec3(texture(textureDiffuse, fs_in.uv));
    vec3 diffuseColor = directionalLight.diffuse * diffuse * vec3(texture(textureDiffuse, fs_in.uv));
    vec3 specularColor = directionalLight.specular * specular;
    return vec4(ambientColor + diffuseColor + specularColor, 1.f);
}

void main() {
    fragColor = CalculateDirectionalLight() * brightness * tint;
}
