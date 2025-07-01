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

uniform samplerCube shadowMap;  // depth‐cubemap
uniform vec3 lightPos;         // pozicija point svetla
uniform float far_plane;       // far plane za depth pass
uniform bool shadows;          // da li računamo senke (true/false)

uniform float uLightIntensity;

// Predefinisane offset smernice za PCF (20 uzoraka)
vec3 sampleOffsetDirections[20] = vec3[](
vec3(1, 1, 1), vec3(1, -1, 1), vec3(-1, -1, 1), vec3(-1, 1, 1),
vec3(1, 1, -1), vec3(1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
vec3(1, 0, 0), vec3(-1, 0, 0), vec3(0, 1, 0), vec3(0, -1, 0),
vec3(0, 0, 1), vec3(0, 0, -1), vec3(1, 1, 0), vec3(-1, 1, 0),
vec3(1, -1, 0), vec3(-1, -1, 0), vec3(1, 0, 1), vec3(-1, 0, 1)
);

// Phong (ambient + diffuse + specular)
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

float ShadowCalculationPCF(vec3 fragPos)
{
    // Vector od fragmenta do svetla
    vec3 fragToLight = fragPos - lightPos;
    float currentDepth = length(fragToLight);

    // Bias da se izbegne self‐shadowing
    float bias = 0.05;

    // Broj uzoraka koje koristi PCF
    int samples = 20;
    float shadow = 0.0;
    float diskRadius = (1.0 + (currentDepth / far_plane)) / 25.0;

    for (int i = 0; i < samples; ++i) {
        vec3 sampleDir = fragToLight + sampleOffsetDirections[i] * diskRadius;
        float closestDepth = texture(shadowMap, sampleDir).r * far_plane;
        if (currentDepth - bias > closestDepth) {
            shadow += 1.0;
        }
    }
    shadow /= float(samples);
    return shadow;
}

// Phong + attenuation + smooth fade‐out za point svetlo
vec3 CalcPointLightSmooth(int i, vec3 normal)
{
    BaseLight b = gPointLights[i].Base;
    vec3 pos = gPointLights[i].LocalPos;
    Attenuation a = gPointLights[i].Atten;

    // 1) Phong (ambient+diffuse+specular)
    vec3 lightDir = normalize(vLocalPos - pos);
    vec4 phongCol = CalcLightInternal(b, lightDir, normal);

    phongCol.rgb *= uLightIntensity;

    // 2)
    float d = length(vLocalPos - pos);
    float atten = a.Constant + a.Linear * d + a.Exp * (d * d);
    vec3 lit = phongCol.rgb / atten;

    // 3)
    float radius = 25.0;
    float fade = clamp((radius - d) / radius, 0.0, 1.0);

    return lit * fade;
}

void main()
{
    vec3 normal = normalize(vNormal);
    vec3 baseCol = texture(gSampler, vTexCoords).rgb;

    // 1)
    float moonAmb = 0.05;
    vec3 ambientPart = moonAmb * baseCol;

    // 2)
    vec3 dirPart = CalcDirectionalLight(normal).rgb * 0.5 * baseCol;

    vec3 result = ambientPart + dirPart;

    // 3)
    if (gNumPointLights >= 1) {
        // Phong + attenuation + fade
        vec3 Li = CalcPointLightSmooth(0, normal) * baseCol;

        float s0 = shadows ? ShadowCalculationPCF(vLocalPos) : 0.0;
        
        result += Li * (1.0 - s0);
    }

    FragColor = vec4(result, 1.0);
}
