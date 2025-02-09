//#shader vertex
#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in mat4 aInstanceMatrix;

out vec2 TexCoords;
out vec3 Normal;
out vec3 FragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main(){
    FragPos = vec3(aInstanceMatrix*vec4(aPos,1.0));
    Normal = mat3(aInstanceMatrix) * aNormal;
    TexCoords = aTexCoords;
    gl_Position = projection * view * aInstanceMatrix * vec4(aPos, 1.0);
}

//#shader fragment
#version 330 core

out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

uniform sampler2D texture_diffuse1;
uniform vec3 LightPos;
uniform vec3 viewPos;

void main(){
    vec3 objectColor = vec3(texture(texture_diffuse1, TexCoords).rgb);
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * vec3(1.0f);

    vec3 norm = normalize(Normal);
    vec3 LightDir = normalize(LightPos - FragPos);
    float diff = max(dot(norm, LightDir), 0.0);
    vec3 diffuse = diff * vec3(1.0f);

    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-LightDir, norm);
    float spec = pow(max(dot(viewDir,reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * vec3(1.0f);

    vec3 result = (ambient + diffuse + specular) * objectColor;

    FragColor = vec4(result, 1.0);
    if (texture(texture_diffuse1, TexCoords).a < 0.5)
    discard;
}