//#shader vertex
#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 invNormal;

out vec3 fragPos;
out vec3 normal;
out vec2 texCoord;

void main() {
    vec4 worldPos = model * vec4(aPos, 1.0f);
    fragPos = worldPos.xyz;
    normal = invNormal * aNormal;
    texCoord = aTexCoord;
    gl_Position = projection * view * worldPos;
}

//#shader fragment
#version 330 core

struct DirLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

struct SpotLight {
    vec3 position;
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float inner_cut_off;
    float outer_cut_off;

    float constant;
    float linear;
    float quadratic;

    int lamp_on;
};

vec3 CalculateDirLight(DirLight light, vec3 normal, vec3 view_dir);
vec3 CalculatePointLight(PointLight light, vec3 normal, vec3 frag_pos, vec3 view_dir);
vec3 CalculateSpotLight(SpotLight light, vec3 normal, vec3 frag_pos, vec3 view_dir);

in vec3 fragPos;
in vec3 normal;
in vec2 texCoord;

uniform DirLight dirlight;
uniform SpotLight spotlight;
uniform vec3 viewPos;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform float shininess;

out vec4 fragColor;

void main() {
    vec3 norm = normalize(normal);
    vec3 view_dir = normalize(viewPos - fragPos);

    vec3 result = CalculateDirLight(dirlight, norm, view_dir);
    result += CalculateSpotLight(spotlight, norm, fragPos, view_dir);

    fragColor = vec4(result, 1.0f);
}

vec3 CalculateDirLight(DirLight light, vec3 normal, vec3 view_dir) {
    vec3 ambient = light.ambient * vec3(texture(texture_diffuse1, texCoord));

    vec3 light_dir = normalize(-light.direction);
    float diff = max(dot(light_dir, normal), 0.0f);
    vec3 diffuse = light.diffuse * diff * vec3(texture(texture_diffuse1, texCoord));

    vec3 reflect_dir = reflect(-light_dir, normal);
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0f), shininess);
    vec3 specular = light.specular * spec * vec3(texture(texture_specular1, texCoord));

    return (ambient + diffuse + specular);
}

vec3 CalculatePointLight(PointLight light, vec3 normal, vec3 frag_pos, vec3 view_dir) {
    vec3 ambient = light.ambient * vec3(texture(texture_diffuse1, texCoord));

    vec3 light_dir = normalize(light.position - frag_pos);
    float diff = max(dot(light_dir, normal), 0.0f);
    vec3 diffuse = light.diffuse * diff * vec3(texture(texture_diffuse1, texCoord));

    vec3 reflect_dir = reflect(-light_dir, normal);
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0f), shininess);
    vec3 specular = light.specular * spec * vec3(texture(texture_specular1, texCoord));

    float distance = length(light.position - frag_pos);
    float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return (ambient + diffuse + specular);
}

vec3 CalculateSpotLight(SpotLight light, vec3 normal, vec3 frag_pos, vec3 view_dir) {
    vec3 ambient = light.ambient * vec3(texture(texture_diffuse1, texCoord));

    vec3 light_dir = normalize(light.position - frag_pos);
    float diff = max(dot(normal, light_dir), 0.0f);
    vec3 diffuse = light.diffuse * diff * vec3(texture(texture_diffuse1, texCoord));

    vec3 reflect_dir = reflect(-light_dir, normal);
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0f), shininess);
    vec3 specular = light.specular * spec * vec3(texture(texture_specular1, texCoord));

    float distance = length(light.position - frag_pos);
    float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    float theta = dot(-light_dir, normalize(light.direction));
    float epsilon = light.inner_cut_off - light.outer_cut_off;
    float intensity = clamp((theta - light.outer_cut_off) / epsilon, 0.0, 1.0);

    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;

    return (ambient + diffuse + specular) * spotlight.lamp_on;
}