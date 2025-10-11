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
struct DirectionalLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct PointLight {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float linear;
    float quadratic;
    float shininess;
};
out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

uniform sampler2D texture_diffuse1;
uniform vec3 viewPos;

uniform DirectionalLight directional_light;
uniform int point_light_count;
uniform PointLight point_lights[16];

void main() {
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    // ----- Directional Light -----
    vec3 lightDir = normalize(directional_light.direction);
    vec3 ambientDir = directional_light.ambient;
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuseDir = diff * directional_light.diffuse;
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), directional_light.shininess);
    vec3 specularDir = spec * directional_light.specular;
    vec3 dirLighting = ambientDir + diffuseDir + specularDir;

    // ----- Point Lights -----
    vec3 pointLighting = vec3(0,0,0);
    for(int i = 0; i < point_light_count; i++) {
        PointLight point_light = point_lights[i];
        vec3 pointDir = normalize(point_light.position - FragPos);
        float distance = length(point_light.position - FragPos);
        float attenuation = 1.0 / (1.0 + point_light.linear * distance + point_light.quadratic * distance * distance);
        vec3 ambientPoint = point_light.ambient;
        float diffPoint = max(dot(norm, pointDir), 0.0);
        vec3 diffusePoint = diffPoint * point_light.diffuse;
        vec3 reflectDirPoint = reflect(-pointDir, norm);
        float specPoint = pow(max(dot(viewDir, reflectDirPoint), 0.0), point_light.shininess);
        vec3 specularPoint = specPoint * point_light.specular;
        pointLighting += (ambientPoint + diffusePoint + specularPoint) * attenuation;
    }
    // Combine
    vec3 texColor = texture(texture_diffuse1, TexCoords).rgb;
    vec3 lighting = dirLighting + pointLighting;
    FragColor = vec4(texColor * lighting, 1.0);
}