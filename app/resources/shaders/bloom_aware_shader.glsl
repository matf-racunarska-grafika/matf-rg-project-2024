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
    Normal = aNormal;
    TexCoords = aTexCoords;
    gl_Position = projection * view * vec4(FragPos, 1.0);
}

//#shader fragment
#version 330 core
#define MAX_POINT_LIGHTS 64
#define MAX_SPOT_LIGHTS 16

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;


in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;


struct LightData {
    vec3 position;
    vec3 direction;

    float cutoff;
    float outercutoff;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};


uniform int num_of_point;
uniform int num_of_spot;

uniform int shininess;

uniform LightData light_directional[8];
uniform LightData light_spot[MAX_SPOT_LIGHTS];
uniform LightData light_point[MAX_POINT_LIGHTS];

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;

uniform vec3 viewPos;

void main()
{
    vec3 color = texture(texture_diffuse1, TexCoords).rgb;
    vec3 specMap = vec3(texture(texture_specular1, TexCoords).xxx);
    vec3 normal = normalize(Normal);

    vec3 lighting = vec3(0.0);
    vec3 viewDir = normalize(viewPos - FragPos);


    for(int i = 0; i < MAX_POINT_LIGHTS; i++)
    {
        vec3 lightDir = normalize(light_point[i].position - FragPos);
        float diff = max(dot(lightDir, normal), 0.0);

        vec3 reflectDir = reflect(-lightDir, normal);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);

        float distance = length(FragPos - light_point[i].position);
        float attenuation = 1.0 / (light_point[i].constant + light_point[i].linear * distance + light_point[i].quadratic * (distance * distance));

        vec3 ambient = light_point[i].ambient * color;
        vec3 diffuse = light_point[i].diffuse * diff * color;
        vec3 specular = light_point[i].specular * spec * specMap;

        ambient *= attenuation;
        diffuse *= attenuation;
        specular *= attenuation;
        lighting += (ambient + diffuse + specular);
    }

    for(int i = 0; i < MAX_SPOT_LIGHTS; i++)
    {
        vec3 lightDir = normalize(light_spot[i].position - FragPos);
        float diff = max(dot(lightDir, normal), 0.0);

        vec3 reflectDir = reflect(-lightDir, normal);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);

        float distance = length(FragPos - light_spot[i].position);
        float attenuation = 1.0 / (light_spot[i].constant + light_spot[i].linear * distance + light_spot[i].quadratic * (distance * distance));

        vec3 ambient = light_spot[i].ambient * color;
        vec3 diffuse = light_spot[i].diffuse * diff * color;
        vec3 specular = light_spot[i].specular * spec * specMap;

        ambient *= attenuation;
        diffuse *= attenuation;
        specular *= attenuation;

        float theta = dot(lightDir, normalize(-lightDir));
        float epsilon = (light_spot[i].cutoff - light_spot[i].outercutoff);
        float intensity = clamp((theta - light_spot[i].outercutoff) / epsilon, 0.0, 1.0);

        diffuse  *= intensity;
        specular *= intensity;
        lighting += (ambient + diffuse + specular);

    }


    float brightness = dot(lighting, vec3(0.2126, 0.7152, 0.0722));

    if (brightness > 1.0)
    BrightColor = vec4(lighting, 1.0);
    else
    BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
    FragColor = vec4(lighting, 1.0);
}
