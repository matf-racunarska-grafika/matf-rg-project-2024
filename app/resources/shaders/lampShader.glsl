//#shader vertex
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;
out vec3 Normal;
out vec3 FragPos;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main() {
    FragPos = vec3(model * vec4(aPos, 1.0));
    TexCoords = aTexCoords;
    Normal = mat3(transpose(inverse(model))) * aNormal;
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}

//#shader fragment
#version 330 core
layout (location = 0) out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

uniform vec3 cameraPos;

struct SpotLight {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 direction;
    vec3 position;
    float cutOff;
    float outer_cutOff;
    float linear;
    float quadratic;
    float shininess;
};

struct PointLight {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float linear;
    float quadratic;
};

uniform vec3 lightIntensity;
uniform PointLight point_light;
uniform sampler2D texture_diffuse0;
uniform sampler2D texture_specular0;
uniform SpotLight spot_light;

vec3 pointLightCalculate(PointLight light, vec3 FragPos, vec3 Normal, vec3 cameraPos) {
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(Normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff;

    vec3 viewDir = normalize(cameraPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, Normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    vec3 specular = light.specular * spec;

    float distance = length(light.position - FragPos);
    float attenuation = 1.0 / (1.0 + light.linear * distance + light.quadratic * distance * distance);

    diffuse *= attenuation;
    specular *= attenuation;

    vec3 ambient = light.ambient;
    return (ambient + diffuse + specular);
}

vec3 spotLightCalculate(SpotLight spot_light, vec3 FragPos, vec3 Normal, vec3 cameraPos, sampler2D texture_diffuse0, sampler2D texture_specular0) {

    vec3 lightDir = normalize(FragPos - spot_light.position);
    float theta = dot(lightDir, normalize(-spot_light.direction));

    float diff = max(dot(-lightDir, normalize(Normal)), 0.0);
    vec3 diffuseSpotLight = spot_light.diffuse * texture(texture_diffuse0, TexCoords).rgb * diff;

    float specularStrength = 0.8;
    vec3 viewDir = normalize(cameraPos - FragPos);
    vec3 reflectDir = normalize(reflect(-lightDir, Normal));
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), spot_light.shininess);
    vec3 specularSpotLight = spot_light.specular * texture(texture_specular0, TexCoords).rgb * spec * specularStrength;

    float p = (theta - spot_light.outer_cutOff) / (spot_light.cutOff - spot_light.outer_cutOff);
    float i = clamp(p, 0.0, 1.0);
    diffuseSpotLight *= i;
    specularSpotLight *= i;

    float d = length(spot_light.position - FragPos);
    float att = 1.0 / (1.0 + d * spot_light.linear + pow(d, 2) * spot_light.quadratic);
    diffuseSpotLight *= att;
    specularSpotLight *= att;
    vec3 ambient = spot_light.ambient * texture(texture_diffuse0, TexCoords).rgb;
    return ambient + diffuseSpotLight + specularSpotLight;
}

void main() {
    vec3 result = pointLightCalculate(point_light, FragPos, Normal, cameraPos);
    result += spotLightCalculate(spot_light, FragPos, Normal, cameraPos, texture_diffuse0, texture_specular0);
    result *= texture(texture_diffuse0, TexCoords).rgb;
    FragColor = vec4(result, 1.0);
}