//#shader vertex
#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;

out vec3 FragPos;
out vec2 TexCoords;
out mat3 TBN;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    vec4 worldPos = model * vec4(aPos, 1.0);
    FragPos = worldPos.xyz;
    TexCoords = aTexCoords;

    mat3 normalMatrix = transpose(inverse(mat3(model)));
    vec3 worldNormal = normalize(normalMatrix * aNormal);

    vec3 worldTangent = normalize(mat3(model) * aTangent);

    vec3 worldBitangent = normalize(cross(worldNormal, worldTangent));

    TBN = mat3(worldTangent, worldBitangent, worldNormal);

    gl_Position = projection * view * worldPos;
}

//#shader fragment
#version 330 core

#define NR_POINT_LIGHTS 2

struct PointLight {
    vec3 position;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform PointLight pointLights[NR_POINT_LIGHTS];

in vec3 FragPos;
in vec2 TexCoords;
in mat3 TBN;

out vec4 FragColor;

uniform sampler2D texture_diffuse1;  // Diffuse mapa
uniform sampler2D texture_specular1; // Specular mapa

uniform vec3 viewPos;
uniform float material_shininess;

vec3 CalcPointLight(PointLight light, vec3 norm, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);

    // Primena light wrapping-a
    float wrapFactor = 0.3;
    float diffRaw = max(dot(norm, lightDir), 0.0);
    float diff = (diffRaw + wrapFactor) / (1.0 + wrapFactor);

    // Spekularnaa komponenta
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = (diffRaw > 0.0) ? pow(max(dot(viewDir, reflectDir), 0.0), material_shininess) : 0.0;

    // Atenuacija
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    // Uzimanje boje iz diffuse i specular mapa
    vec3 diffuseColor = texture(texture_diffuse1, TexCoords).rgb;
    float specularMap = texture(texture_specular1, TexCoords).r;

    // Kombinovanje komponenata
    vec3 ambient = light.ambient * diffuseColor;
    vec3 diffuse = light.diffuse * diff * diffuseColor;
    vec3 specular = light.specular * spec * specularMap;

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return ambient + diffuse + specular;
}

void main()
{
    vec3 norm = normalize(TBN[2]);

    // Pravac ka kameri
    vec3 viewDir = normalize(viewPos - FragPos);

    // Sumiramo osvetljenje od svih point light-ova
    vec3 result = vec3(0.0);
    for (int i = 0; i < NR_POINT_LIGHTS; i++) {
        result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);
    }

    FragColor = vec4(result, 1.0);
}