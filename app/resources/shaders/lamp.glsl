//#shader vertex

#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;
out vec3 Normal;
out vec3 FragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    TexCoords = aTexCoords;
    gl_Position = projection * view * vec4(FragPos, 1.0);
}

//#shader fragment

#version 330 core

out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

uniform sampler2D texture_diffuse1;   // Lamp_CM.png
uniform sampler2D texture_emission1;  // Lamp_Emission.png
uniform sampler2D texture_specular1;  // Lamp_SM.png

uniform vec3 viewPos;
uniform bool lampIsOn;

struct DirLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform DirLight dirLight;

struct PointLight {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float constant;
    float linear;
    float quadratic;
};
uniform PointLight pointLights[4];
uniform int numPointLights;

uniform float material_shininess;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 diffuseColor, float specularValue);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 diffuseColor, float specularValue);

void main() {
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 diffuseColor = texture(texture_diffuse1, TexCoords).rgb;
    float specularValue = texture(texture_specular1, TexCoords).r;
    vec3 emissionColor = texture(texture_emission1, TexCoords).rgb;

    float emissionIntensity = (emissionColor.r + emissionColor.g + emissionColor.b) / 3.0;

    vec3 result = CalcDirLight(dirLight, norm, viewDir, diffuseColor, specularValue);

    for(int i = 0; i < numPointLights && i < 4; i++) {
        result += CalcPointLight(pointLights[i], norm, FragPos, viewDir, diffuseColor, specularValue);
    }

    if (lampIsOn) {
        if (emissionIntensity > 0.1) {
            result += emissionColor * 2.5;
        }
    } else {
        if (emissionIntensity > 0.1) {
            result += emissionColor * 0.05;
        }
    }

    FragColor = vec4(result, 1.0);
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 diffuseColor, float specularValue) {
    vec3 lightDir = normalize(-light.direction);

    vec3 ambient = light.ambient * diffuseColor;

    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * diffuseColor;

    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material_shininess);
    vec3 specular = light.specular * spec * specularValue;

    return ambient + diffuse + specular;
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 diffuseColor, float specularValue) {
    vec3 lightDir = normalize(light.position - fragPos);

    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance +
    light.quadratic * (distance * distance));

    vec3 ambient = light.ambient * diffuseColor;

    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * diffuseColor;

    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material_shininess);
    vec3 specular = light.specular * spec * specularValue;

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return ambient + diffuse + specular;
}