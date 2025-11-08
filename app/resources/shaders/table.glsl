//#shader vertex

#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

out vec2 TexCoords;
out vec3 Normal;
out vec3 FragPos;
out mat3 TBN;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    TexCoords = aTexCoords;

    vec3 N = normalize(mat3(transpose(inverse(model))) * aNormal);

    vec3 T = normalize(mat3(model) * aTangent);
    vec3 B = normalize(mat3(model) * aBitangent);
    T = normalize(T - dot(T, N) * N);
    B = cross(N, T);

    TBN = mat3(T, B, N);
    Normal = N;

    gl_Position = projection * view * vec4(FragPos, 1.0);
}

//#shader fragment

#version 330 core

out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;
in mat3 TBN;

// Textures
uniform sampler2D texture_diffuse1;  // basecolor.jpg
uniform sampler2D texture_normal1;   // normal1.jpg
uniform bool useNormalMap;

uniform vec3 viewPos;

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

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 texColor);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 texColor);

void main() {
    vec3 texColor = texture(texture_diffuse1, TexCoords).rgb;

    vec3 normal;
    if (useNormalMap) {
        normal = texture(texture_normal1, TexCoords).rgb;
        normal = normalize(normal * 2.0 - 1.0);
        normal = normalize(TBN * normal);
    } else {
        normal = normalize(Normal);
    }

    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 result = CalcDirLight(dirLight, normal, viewDir, texColor);

    for(int i = 0; i < numPointLights && i < 4; i++) {
        result += CalcPointLight(pointLights[i], normal, FragPos, viewDir, texColor);
    }
    FragColor.rgb = max(FragColor.rgb, vec3(0.01));
    FragColor = vec4(result, 1.0);
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 texColor) {
    vec3 lightDir = normalize(-light.direction);

    vec3 ambient = light.ambient * texColor;

    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * texColor;

    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material_shininess);
    vec3 specular = light.specular * spec * 0.3;  // Reduced for wood

    return ambient + diffuse + specular;
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 texColor) {
    vec3 lightDir = normalize(light.position - fragPos);

    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance +
    light.quadratic * (distance * distance));

    vec3 ambient = light.ambient * texColor;

    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * texColor;

    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material_shininess);
    vec3 specular = light.specular * spec * 0.3;  // Reduced for wood

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return ambient + diffuse + specular;
}
