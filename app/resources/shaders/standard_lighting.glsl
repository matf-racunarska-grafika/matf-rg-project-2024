//#shader vertex
#version 330

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;
out vec3 FragPos;
out vec3 Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main(){
    FragPos = vec3(model * vec4(aPos,1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    TexCoords = aTexCoords;

    gl_Position = projection*view*vec4(FragPos,1.0);
}

//#shader fragment

#version 330 core

//1 direkciono i 2 point svetla

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

out vec4 FragColor;

struct Material{
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

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

#define NR_POINT_LIGHTS 2

uniform vec3 viewPos;
uniform Material material;
uniform DirLight dirLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];

vec3 CalcDirLight(DirLight light, vec3 normal,vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal,vec3 fragPos,vec3 viewDir);


void main(){

    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 result = CalcDirLight(dirLight,norm,viewDir);
    for (int i = 0; i<NR_POINT_LIGHTS;++i){
        result += CalcPointLight(pointLights[i],norm,FragPos,viewDir);
    }

    FragColor = vec4(result,1.0);
}

vec3 CalcDirLight(DirLight light, vec3 normal,vec3 viewDir){

    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(normal,lightDir),0.0);

    vec3 reflectDir = reflect(-lightDir,normal);
    float spec = pow(max(dot(viewDir,reflectDir),0.0),material.shininess);

    vec3 ambient = light.ambient * vec3(texture(material.diffuse,TexCoords).rgb);
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse,TexCoords).rgb);
    vec3 specular = light.specular * spec * vec3(texture(material.specular,TexCoords).rgb);

    return (ambient+diffuse+specular);
}


vec3 CalcPointLight(PointLight light, vec3 normal,vec3 fragPos,vec3 viewDir){

    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(normal,lightDir),0.0);

    vec3 reflectDir = reflect(-lightDir,normal);
    float spec = pow(max(dot(viewDir,reflectDir),0.0),material.shininess);

    vec3 ambient = light.ambient * vec3(texture(material.diffuse,TexCoords).rgb);
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse,TexCoords).rgb);
    vec3 specular = light.specular * spec * vec3(texture(material.specular,TexCoords).rgb);

    //attenuation
    float dis = length(light.position-fragPos);//distance
    float attenuation = 1.0 / (light.constant +light.linear*dis + light.quadratic * dis*dis);

    ambient*=attenuation;
    diffuse*=attenuation;
    specular*=attenuation;

    return (ambient+diffuse+specular);
}


