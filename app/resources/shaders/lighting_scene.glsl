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

struct Light_point {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

struct Light_dir {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
};

#define NUM_POINT_LIGHTS 2

out vec4 FragColor;

in vec3 FragPos;
in vec2 TexCoords;
in vec3 Normal;

uniform vec3 viewPos;
uniform Light_dir LightDirectional;
uniform Light_point LightPoints[NUM_POINT_LIGHTS];

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;

vec3 CalculatePointLight(Light_point source_point, vec3 norm_vec, vec3 viewers_position);
vec3 CalculateDirectionalLight(Light_dir direct_light, vec3 norm_vec);

void main() {
    vec3 norm = normalize(Normal);

    vec3 result = CalculateDirectionalLight(LightDirectional, norm);
    for (int i = 0; i < NUM_POINT_LIGHTS; i++) {
        result += CalculatePointLight(LightPoints[i], norm, viewPos);
    }

    FragColor = vec4(result, 1.0f);
}

vec3 CalculatePointLight(Light_point LightPoint, vec3 norm, vec3 viewPos) {
    // Ambient color
    vec3 ambientCol = LightPoint.ambient * texture(texture_diffuse1, TexCoords).rgb;

    // Diffuse color
    vec3 lightDir = normalize(LightPoint.position - FragPos);
    vec3 diffuseCol = LightPoint.diffuse * (max(dot(lightDir, norm), 0) * texture(texture_diffuse1, TexCoords).rgb);

    // Attenuation
    float distance = length(LightPoint.position - FragPos);
    float attenuation = 1.0 / (LightPoint.constant + LightPoint.linear * distance + LightPoint.quadratic * (distance * distance));

    ambientCol *= attenuation;
    diffuseCol *= attenuation;

    //  Specular color(Just for example purposes since my model island has not fragments that will have an actual specular component)
    //    vec3 reflectDir = normalize(reflect(-lightDir, norm));
    //    vec3 viewDir = normalize(FragPos - viewPos);
    //    vec3 specularCol = LightPoint.specular * max(dot(viewDir, reflectDir), 0) * texture(texture_specular1, TexCoords).rgb;

    return (ambientCol + diffuseCol);
}

vec3 CalculateDirectionalLight(Light_dir directionalLight, vec3 normal) {
    vec3 lightDir = normalize(-directionalLight.direction);
    float diff = max(dot(normal, lightDir), 0.0);

    vec3 ambient = directionalLight.ambient * vec3(texture(texture_diffuse1, TexCoords));
    vec3 diffuse = directionalLight.diffuse * diff * vec3(texture(texture_diffuse1, TexCoords));
    return (ambient + diffuse);
}
