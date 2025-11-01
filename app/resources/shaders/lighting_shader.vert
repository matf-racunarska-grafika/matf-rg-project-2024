//#shader vertex
#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 FragPos;
out vec3 Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    gl_Position = projection * view * vec4(FragPos, 1.0);
}

//#shader fragment
#version 330 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;

uniform vec3 viewPos;

// Directional light
uniform vec3 dirLightDir;
uniform vec3 dirLightColor;

// Point light
uniform vec3 pointLightPos;
uniform vec3 pointLightColor;

// Material
uniform vec3 objectColor;

void main() {
    vec3 norm = normalize(Normal);

    // Directional light
    vec3 lightDir = normalize(-dirLightDir);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuseDir = diff * dirLightColor;

    // Point light
    vec3 toLight = normalize(pointLightPos - FragPos);
    float diffPoint = max(dot(norm, toLight), 0.0);
    float dist = length(pointLightPos - FragPos);
    float attenuation = 1.0 / (1.0 + 0.09 * dist + 0.032 * dist * dist);
    vec3 diffusePoint = diffPoint * pointLightColor * attenuation;

    vec3 result = (diffuseDir + diffusePoint) * objectColor;
    FragColor = vec4(result, 1.0);
}
