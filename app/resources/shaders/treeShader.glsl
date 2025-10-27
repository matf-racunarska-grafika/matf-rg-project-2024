//#shader vertex
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
// NOVI INSTANCIRANI ATRIBUTI NA LOKACIJAMA 5, 6, 7, 8
layout (location = 5) in mat4 instanceModel;

out vec2 TexCoords;
out vec3 FragPos;
out vec3 Normal;

uniform mat4 view;
uniform mat4 projection;

void main() {
    TexCoords = aTexCoords;

    // Koristimo instanceModel
    FragPos = vec3(instanceModel * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(instanceModel))) * aNormal;
    gl_Position = projection * view * instanceModel * vec4(aPos, 1.0);
}

//#shader fragment
#version 330 core
out vec4 FragColor;

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

uniform vec3 viewPos;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform float shininess;

uniform Light light1;   // Cube source of light
uniform Light light2;   // Pendulum source of light

vec3 CalculateLight(Light light, vec3 normal, vec3 viewDir, vec3 fragPos, vec2 texCoords) {
    vec3 lightDir = normalize(light.position - fragPos);
    vec3 reflectDir = reflect(-lightDir, normal);

    // Read textures directly from the engine uniform
    vec3 diffuseColor = texture(texture_diffuse1, texCoords).rgb;
    vec3 specularColor = texture(texture_specular1, texCoords).rgb;

    //    // If model doesn't have a texture, use white as a fallback
    if (length(diffuseColor) < 0.001)
    diffuseColor = vec3(1.0);
    if (length(specularColor) < 0.001)
    specularColor = vec3(0.2);

    // ambient
    vec3 ambient = light.ambient * diffuseColor;

    // diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * diffuseColor;

    // specular
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = light.specular * spec * specularColor;

    return ambient + diffuse + specular;
}

void main() {
    // Normalization and calculations
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    // Calculate contributions from both lights
    vec3 result1 = CalculateLight(light1, norm, viewDir, FragPos, TexCoords);
    vec3 result2 = CalculateLight(light2, norm, viewDir, FragPos, TexCoords);

    // Combine results from both lights
    vec3 result = result1 + result2;

    FragColor = vec4(result, 1.0);
}
