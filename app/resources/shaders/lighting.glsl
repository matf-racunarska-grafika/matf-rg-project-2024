//#shader vertex
#version 330 core

layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 Normal0;
layout (location = 2) in vec2 TexCoord0;

out vec2 vTexCoords;
out vec3 vNormal;
out vec3 vLocalPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    vec4 worldPos = model * vec4(Position, 1.0);
    vLocalPos = worldPos.xyz;

    mat3 normalMat = transpose(inverse(mat3(model)));
    vNormal = normalize(normalMat * Normal0);

    vTexCoords = TexCoord0;

    gl_Position = projection * view * worldPos;
}

//#shader fragment
#version 330 core

const int MAX_POINT_LIGHTS = 2;

in vec2 vTexCoords;
in vec3 vNormal;
in vec3 vLocalPos;

out vec4 FragColor;

struct BaseLight {
    vec3 Color;
    float AmbientIntensity;
    float DiffuseIntensity;
};

struct DirectionalLight {
    BaseLight Base;
    vec3 Direction;
};

struct Attenuation {
    float Constant;
    float Linear;
    float Exp;
};

struct PointLight {
    BaseLight Base;
    vec3 LocalPos;
    Attenuation Atten;
};

struct Material {
    vec3 AmbientColor;
    vec3 DiffuseColor;
    vec3 SpecularColor;
};

uniform DirectionalLight gDirectionalLight;
uniform int gNumPointLights;
uniform PointLight gPointLights[MAX_POINT_LIGHTS];
uniform Material gMaterial;
uniform sampler2D gSampler;
uniform sampler2D gSamplerSpecularExponent;
uniform vec3 gCameraLocalPos;

vec4 CalcLightInternal(BaseLight light, vec3 lightDir, vec3 normal)
{
    vec4 ambient = vec4(light.Color, 1.0)
    * light.AmbientIntensity
    * vec4(gMaterial.AmbientColor, 1.0);

    float diffFactor = max(dot(normal, -lightDir), 0.0);
    vec4 diffuse = vec4(0.0);
    vec4 specular = vec4(0.0);

    if (diffFactor > 0.0) {
        diffuse = vec4(light.Color, 1.0)
        * light.DiffuseIntensity
        * vec4(gMaterial.DiffuseColor, 1.0)
        * diffFactor;

        vec3 toCam = normalize(gCameraLocalPos - vLocalPos);
        vec3 reflectR = reflect(lightDir, normal);
        float specFactor = max(dot(toCam, reflectR), 0.0);
        if (specFactor > 0.0) {
            float exponent = texture(gSamplerSpecularExponent, vTexCoords).r * 255.0;
            specFactor = pow(specFactor, exponent);
            specular = vec4(light.Color, 1.0)
            * light.DiffuseIntensity
            * vec4(gMaterial.SpecularColor, 1.0)
            * specFactor;
        }
    }

    return ambient + diffuse + specular;
}

vec4 CalcDirectionalLight(vec3 normal)
{
    return CalcLightInternal(
        gDirectionalLight.Base,
        normalize(-gDirectionalLight.Direction),
        normal
    );
}

vec4 CalcPointLight(int i, vec3 normal)
{
    vec3 lightDir = normalize(vLocalPos - gPointLights[i].LocalPos);
    vec4 color = CalcLightInternal(
        gPointLights[i].Base,
        lightDir,
        normal
    );

    float dist = length(vLocalPos - gPointLights[i].LocalPos);
    float atten = gPointLights[i].Atten.Constant
    + gPointLights[i].Atten.Linear * dist
    + gPointLights[i].Atten.Exp * dist * dist;
    return color / atten;
}

void main()
{
    vec3 normal = normalize(vNormal);

    vec4 totalLight = CalcDirectionalLight(normal);

    for (int i = 0; i < gNumPointLights; ++i)
    totalLight += CalcPointLight(i, normal);

    vec4 baseColor = texture(gSampler, vTexCoords);
    FragColor = baseColor * totalLight;
}
