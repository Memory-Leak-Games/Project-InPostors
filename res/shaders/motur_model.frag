#version 430 core

in vec3 NormalVector;
in vec2 TexCoordFragment;

uniform sampler2D texture_diffuse0;
uniform sampler2D texture_specular0;
uniform sampler2D texture_normalmap0;

uniform samplerCube cubemap;

out vec4 FragColor;

struct DirectionalLight {
    vec4 Color;
    vec3 Direction;
};

struct PointLight {
    vec4 Color;
    vec3 Position;
    float Linear;
    float Quadratic;
};

struct SpotLight {
    vec4 Color;
    vec3 Position;
    vec3 Direction;
    float Linear;
    float Quadratic;
    float CutOff;
    float OuterCutOff;
};

layout(std140, binding = 1) uniform Lights {
    DirectionalLight Sun;   // 32   // 0
    PointLight Bulb;        // 48   // 32
    SpotLight SpotLights[2]; // 48   // 80
};

in VS_OUT {
    vec3 Position;
    vec3 Normal;
    vec2 TexCoord;

    vec3 ViewPosition;
} fs_in;

vec4 CalculatePointLight(PointLight);

float LightAttenuation(float Distance, float Linear, float Quadratic) {
    return 1.f / (1.f + Linear * Distance + Quadratic * Distance * Distance);
}

float CalculateSpecular(vec3 LightDirection) {
    vec3 ViewDir = normalize(fs_in.ViewPosition - fs_in.Position);
    vec3 HalfwayDir = normalize(LightDirection + ViewDir);
    float Spec = pow(max(dot(fs_in.Normal, HalfwayDir), 0.0), 32);
    return Spec;
}

vec4 CalculatePointLight(PointLight _PointLight) {
    vec3 LightDir = normalize(_PointLight.Position - fs_in.Position);
    float Diff = max(dot(fs_in.Normal, LightDir), 0.f);
    vec3 Diffuse = Diff * _PointLight.Color.xyz;
    float Distance = length(_PointLight.Position - fs_in.Position);
    return vec4(Diffuse + CalculateSpecular(LightDir), 1) * _PointLight.Color.w * LightAttenuation(Distance, _PointLight.Linear, _PointLight.Quadratic);
}

vec4 CalculateBulb() {
    return CalculatePointLight(Bulb);
}

vec4 CalculateDirectionalLight() {
    float AngleDifference = max(dot(fs_in.Normal, normalize(-Sun.Direction)), 0.f);
    return (AngleDifference + CalculateSpecular(-Sun.Direction)) * vec4(vec3(Sun.Color), 1.f) * Sun.Color.w;
}

vec4 CalculateSpotLight(SpotLight Light) {
    vec3 LightDir = normalize(Light.Position - fs_in.Position);
    float Epsilon =  cos(Light.CutOff) - cos(Light.OuterCutOff);
    float Theta = dot(LightDir, normalize(-Light.Direction));

    PointLight PseudoPointLight;
    PseudoPointLight.Position = Light.Position;
    PseudoPointLight.Color = Light.Color;
    PseudoPointLight.Linear = Light.Linear;
    PseudoPointLight.Quadratic = Light.Quadratic;

    float Intensity = clamp((Theta - cos(Light.OuterCutOff)) / Epsilon, 0.f, 1.f);
    return CalculatePointLight(PseudoPointLight) * Intensity;
}

void main() {
    vec4 CalculatedSpotLights = vec4(0.f);
    for (int i = 0; i < 2; ++i) {
        CalculatedSpotLights += CalculateSpotLight(SpotLights[i]);
    }

    vec4 Light = CalculateBulb() + CalculateDirectionalLight() + CalculatedSpotLights;

    vec4 color = texture(texture_diffuse0, fs_in.TexCoord);

    if (length(color.xyz - vec3(0.416)) <= 0.05) {
        vec3 viewDirection = normalize(fs_in.Position - fs_in.ViewPosition);
        vec3 reflectVector = reflect(viewDirection, fs_in.Normal);
        FragColor = vec4(texture(cubemap, reflectVector).rgb, 1.0);
    } else if (length(color.xyz - vec3(0.0, 1.0, 1.0)) <= 0.05) {
        float refractionRatioRed = 1.00 / 1.52;
        float refractionRatioGreen = 1.00 / 1.32;
        float refractionRatioBlue = 1.00 / 1.20;

        vec3 viewDirection = normalize(fs_in.Position - fs_in.ViewPosition);
        vec3 refractionVectorRed = refract(viewDirection, fs_in.Normal, refractionRatioRed);
        vec3 refractionVectorGreen = refract(viewDirection, fs_in.Normal, refractionRatioGreen);
        vec3 refractionVectorBlue = refract(viewDirection, fs_in.Normal, refractionRatioBlue);

        float red = texture(cubemap, refractionVectorRed).r;
        float green = texture(cubemap, refractionVectorGreen).g;
        float blue = texture(cubemap, refractionVectorBlue).b;

        FragColor = vec4(red, green, blue, 1.0);
    } else {
        FragColor = color * Light;
    }
}
