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

struct DirectionalLight {
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

uniform sampler2D texture_diffuse1;
uniform vec3 viewPos;
uniform DirectionalLight dirLight;
uniform PointLight pointLight;
uniform bool isLightSource;
uniform vec3 lightColor;

void main() {
    // If this is a light source, just render the color
    if (isLightSource) {
        FragColor = vec4(lightColor, 1.0);
        return;
    }

    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 texColor = texture(texture_diffuse1, TexCoords).rgb;

    vec3 result = vec3(0.0);

    // Directional light
    vec3 lightDir = normalize(-dirLight.direction);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);

    vec3 ambient = dirLight.ambient * texColor;
    vec3 diffuse = dirLight.diffuse * diff * texColor;
    vec3 specular = dirLight.specular * spec * texColor;

    result += (ambient + diffuse + specular);

    // Point light
    vec3 lightDir2 = normalize(pointLight.position - FragPos);
    float diff2 = max(dot(norm, lightDir2), 0.0);
    vec3 reflectDir2 = reflect(-lightDir2, norm);
    float spec2 = pow(max(dot(viewDir, reflectDir2), 0.0), 35.0);

    float distance = length(pointLight.position - FragPos);
    float attenuation = 1.0 / (pointLight.constant + pointLight.linear * distance + pointLight.quadratic * (distance * distance));

    vec3 ambient2 = pointLight.ambient * texColor;
    vec3 diffuse2 = pointLight.diffuse * diff2 * texColor;
    vec3 specular2 = pointLight.specular * spec2 * texColor;

    ambient2 *= attenuation;
    diffuse2 *= attenuation;
    specular2 *= attenuation;

    result += (ambient2 + diffuse2 + specular2);

    FragColor = vec4(result, 1.0);
}
