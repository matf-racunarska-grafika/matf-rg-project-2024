//#shader vertex
#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    vec4 worldPos = model * vec4(aPos, 1.0);
    FragPos = worldPos.xyz;

    // Transformacija normale
    mat3 normalMatrix = mat3(model[0].xyz, model[1].xyz, model[2].xyz);
    normalMatrix = transpose(inverse(normalMatrix));
    Normal = normalMatrix * aNormal;

    TexCoords = aTexCoords;
    gl_Position = projection * view * worldPos;
}

//#shader fragment
#version 330 core

struct PointLight {
    vec3 position;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

out vec4 FragColor;

uniform sampler2D texture_diffuse1;
uniform vec3 viewPos;
uniform PointLight pointLight;
uniform float material_shininess;

void main() {
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(pointLight.position - FragPos);

    // Diffuse komponenta
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = pointLight.diffuse * diff * texture(texture_diffuse1, TexCoords).rgb;

    // Ambient komponenta
    vec3 ambient = pointLight.ambient * texture(texture_diffuse1, TexCoords).rgb;

    // Specular komponenta
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material_shininess);
    vec3 specular = pointLight.specular * spec;

    // Atenuacija
    float distance = length(pointLight.position - FragPos);
    float attenuation = 1.0 / (pointLight.constant + pointLight.linear * distance + pointLight.quadratic * (distance * distance));

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
}
