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
layout (location = 1) out vec4 LightColor;

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

uniform vec3 lightIntensity;
uniform sampler2D texture_diffuse0;
uniform sampler2D texture_specular0;
uniform SpotLight spot_light;

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
    vec3 result = lightIntensity * texture(texture_diffuse0, TexCoords).rgb;
    float brightness = dot(result, vec3(0.2126, 0.7152, 0.0722));
    result += spotLightCalculate(spot_light, FragPos, Normal, cameraPos, texture_diffuse0, texture_specular0);
    LightColor = vec4(result, 1.0);
    FragColor = vec4(result, 1.0);
}