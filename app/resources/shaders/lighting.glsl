//#shader vertex
#version 330 core

layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 Normal0;
layout (location = 2) in vec2 TexCoord0;

out vec3 vLocalPos;
out vec3 vNormal;
out vec2 vTexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    // 1) world-space pozicija
    vec4 worldPos = model * vec4(Position, 1.0);
    vLocalPos = worldPos.xyz;

    // 2) normalna u world-space
    mat3 normalMat = transpose(inverse(mat3(model)));
    vNormal = normalize(normalMat * Normal0);

    // 3) UV
    vTexCoords = TexCoord0;

    // 4) IDEALNO PROJEKTOVANJE
    gl_Position = projection * view * worldPos;
}

//#shader fragment
#version 330 core

const int MAX_POINT_LIGHTS = 2;

// Ulazi iz vertexa
in vec2 vTexCoords;
in vec3 vNormal;
in vec3 vLocalPos;

// Izlaz
out vec4 FragColor;

// Osnovni tip svetla: boja + jačine
struct BaseLight {
    vec3 Color;
    float AmbientIntensity;
    float DiffuseIntensity;
};

// Directional light
struct DirectionalLight {
    BaseLight Base;
    vec3 Direction;
};

// Atenuacija za point light
struct Attenuation {
    float Constant;
    float Linear;
    float Exp;
};

// Point light
struct PointLight {
    BaseLight Base;
    vec3 LocalPos;
    Attenuation Atten;
};

// Materijal
struct Material {
    vec3 AmbientColor;
    vec3 DiffuseColor;
    vec3 SpecularColor;
};

// Uniformi
uniform DirectionalLight gDirectionalLight;
uniform int gNumPointLights;
uniform PointLight gPointLights[MAX_POINT_LIGHTS];
uniform Material gMaterial;
uniform sampler2D gSampler;                // diffuse mapa
uniform sampler2D gSamplerSpecularExponent;
uniform vec3 gCameraLocalPos;         // pozicija kamere

vec4 CalcLightInternal(BaseLight light, vec3 lightDir, vec3 normal)
{
    // Ambient
    vec4 ambient = vec4(light.Color, 1.0)
    * light.AmbientIntensity
    * vec4(gMaterial.AmbientColor, 1.0);

    // Diffuse i specular
    float diffFactor = max(dot(normal, -lightDir), 0.0);
    vec4 diffuse = vec4(0.0);
    vec4 specular = vec4(0.0);
    if (diffFactor > 0.0) {
        diffuse = vec4(light.Color, 1.0)
        * light.DiffuseIntensity
        * vec4(gMaterial.DiffuseColor, 1.0)
        * diffFactor;

        // Phong specular
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

//==========================================================================
// Directional light
//==========================================================================
vec4 CalcDirectionalLight(vec3 normal)
{
    return CalcLightInternal(
        gDirectionalLight.Base,
        normalize(-gDirectionalLight.Direction),
        normal
    );
}

//==========================================================================
// Jedan point light
//==========================================================================
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

    // 1) Directional light
    vec4 totalLight = CalcDirectionalLight(normal);

    // 2) Point lights
    for (int i = 0; i < gNumPointLights; ++i)
    totalLight += CalcPointLight(i, normal);

    // 3) Miks sa diffuse teksturom
    vec4 baseColor = texture(gSampler, vTexCoords);
    FragColor = baseColor * totalLight;
}