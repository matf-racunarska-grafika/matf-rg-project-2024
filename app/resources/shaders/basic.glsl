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
    Normal = aNormal;
    TexCoords = vec2(aTexCoords.x, 1.0 - aTexCoords.y);
    gl_Position = projection * view * vec4(FragPos, 1.0);
}

//#shader fragment

#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform sampler2D texture_diffuse1;

struct DirLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;
    float constant;
    float linear;
    float quadratic;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform DirLight dirLight;
uniform PointLight pointLight;
uniform vec3 viewPos;
uniform float shininess;

vec3 calc_dir_light(DirLight light, vec3 normal, vec3 viewDir, vec3 albedo)
{
    vec3 L = normalize(-light.direction);
    float diff = max(dot(normal, L), 0.0);
    vec3 reflectDir = reflect(-L, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 ambient  = light.ambient  * albedo;
    vec3 diffuse  = light.diffuse  * diff * albedo;
    vec3 specular = light.specular * spec;
    return ambient + diffuse + specular;
}

vec3 calc_point_light(PointLight light, vec3 normal, vec3 viewDir, vec3 fragPos, vec3 albedo)
{
    vec3 L = normalize(light.position - fragPos);
    float diff = max(dot(normal, L), 0.0);
    vec3 reflectDir = reflect(-L, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    float distance    = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * distance * distance);
    vec3 ambient  = light.ambient  * albedo;
    vec3 diffuse  = light.diffuse  * diff * albedo;
    vec3 specular = light.specular * spec;
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    return ambient + diffuse + specular;
}

void main()
{
    vec3 albedo = texture(texture_diffuse1, TexCoords).rgb;
    vec3 N = normalize(Normal);
    vec3 V = normalize(viewPos - FragPos);
    vec3 color = vec3(0.0);
    color += calc_dir_light(dirLight, N, V, albedo);
    color += calc_point_light(pointLight, N, V, FragPos, albedo);
    FragColor = vec4(color, 1.0);
}