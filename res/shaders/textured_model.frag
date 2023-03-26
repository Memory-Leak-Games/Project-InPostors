#version 430 core

in vec3 NormalVector;
in vec2 TexCoordFragment;

uniform sampler2D texture_diffuse0;

uniform float brightness;
uniform vec4 tint;

out vec4 FragColor;

struct DirectionalLight {
    vec4 Color;
    vec3 Direction;
};

layout(std140, binding = 1) uniform Lights {
    DirectionalLight Sun;   // 32   // 0
};

in VS_OUT {
    vec3 Position;
    vec3 Normal;
    vec2 TexCoord;

    vec3 ViewPosition;
} fs_in;


float CalculateSpecular(vec3 LightDirection) {
    vec3 ViewDir = normalize(fs_in.ViewPosition - fs_in.Position);
    vec3 HalfwayDir = normalize(LightDirection + ViewDir);
    float Spec = pow(max(dot(fs_in.Normal, HalfwayDir), 0.0), 32);
    return Spec;
}

vec4 CalculateDirectionalLight() {
    float AngleDifference = max(dot(fs_in.Normal, normalize(-Sun.Direction)), 0.f);
    return (AngleDifference + CalculateSpecular(-Sun.Direction)) * vec4(vec3(Sun.Color), 1.f) * Sun.Color.w;
}

void main() {
    vec4 Light = CalculateDirectionalLight();
    FragColor = texture(texture_diffuse0, fs_in.TexCoord) * Light * brightness * tint;
    FragColor.w = 1.f;
}
