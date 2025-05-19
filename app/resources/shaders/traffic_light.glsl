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
    Normal = mat3(transpose(inverse(model)))*aNormal;
    TexCoords = aTexCoords;
    gl_Position = projection * view * vec4(FragPos, 1.0);
}

//#shader fragment
#version 330 core

out vec4 FragColor;

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

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

uniform DirLight directionalLight;
uniform PointLight redPointLight;
uniform PointLight yellowPointLight;
uniform PointLight greenPointLight;
uniform sampler2D texture_diffuse0;
uniform sampler2D texture_specular0;
uniform float material_shininess;
uniform vec3 viewPosition;
uniform int state;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material_shininess);
    // combine results
    vec3 ambient = light.ambient * vec3(texture(texture_diffuse0, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(texture_diffuse0, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(texture_specular0, TexCoords));
    return (ambient + diffuse + specular);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material_shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    // combine results
    vec3 ambient = light.ambient * vec3(texture(texture_diffuse0, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(texture_diffuse0, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(texture_specular0, TexCoords).xxx);
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

float calcGlow(vec3 texColor, vec3 targetColor) {
    float distance = length(texColor - targetColor);
    return smoothstep(0.4, 0.0, distance);
}

void main()
{
    vec3 normal = normalize(Normal);
    vec3 viewDir = normalize(viewPosition - FragPos);
    vec3 texColor = vec3(texture(texture_diffuse0, TexCoords));

    vec3 result = CalcDirLight(directionalLight, normal, viewDir);
    result += CalcPointLight(redPointLight, normal, FragPos, viewDir);
    result += CalcPointLight(yellowPointLight, normal, FragPos, viewDir);
    result += CalcPointLight(greenPointLight, normal, FragPos, viewDir);

    vec3 redLight =  vec3(0.6,0.0,0.0);
    vec3 yellowLight = vec3(0.6, 0.5, 0.0);
    vec3 greenLight = vec3(0.0,0.55,0.0);

    float glow = 0.0;
    vec3 glowColor = vec3(0.0);

    if (state == 0) {

        glow = calcGlow(texColor, redLight);
        glowColor = redLight * 2.0;
        result += glow * glowColor * 5.0;

    }else if (state == 1) {

         glow = calcGlow(texColor, yellowLight);
         glowColor = yellowLight * 2.0;
         result += glow * glowColor * 5.0;

    }else if (state == 2) {

         glow = calcGlow(texColor, greenLight);
         glowColor = greenLight * 2.0;
         result += glow * glowColor * 5.0;

    }else if (state == 3) {

        float redGlow = calcGlow(texColor, redLight);
        yellowLight = vec3(0.6,0.4,0.0);
        float yellowGlow = calcGlow(texColor, yellowLight);
        result += redGlow * redLight * 5.0;
        result += yellowGlow * yellowLight * 5.0;
    }

    FragColor = vec4(result, 1.0);
}