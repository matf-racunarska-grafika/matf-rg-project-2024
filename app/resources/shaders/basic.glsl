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

void main(){
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    TexCoords = aTexCoords;
    gl_Position = projection * view * vec4(FragPos, 1.0);
}

//#shader fragment
#version 330 core

struct DirLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform vec3 viewPos;
uniform DirLight dirLight;

uniform sampler2D texture_diffuse0;
uniform sampler2D texture_specular0;
uniform float material_shininess;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);

void main()
{

    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 result = CalcDirLight(dirLight, norm, viewDir);

    FragColor = vec4(result, 1.0);
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);

    float diff = max(dot(normal, lightDir), 0.0);

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material_shininess);

    vec3 ambient = light.ambient * vec3(texture(texture_diffuse0, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(texture_diffuse0, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(texture_specular0, TexCoords));
    return (ambient + diffuse + specular);
}