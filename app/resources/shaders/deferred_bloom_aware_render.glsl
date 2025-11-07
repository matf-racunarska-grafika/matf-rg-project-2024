
//#shader vertex

#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;
out vec2 TexCoords;

void main()
{
    TexCoords = aTexCoords;
    gl_Position = vec4(aPos, 1.0);
}

//#shader fragment
#version 330 core
#define MAX_POINT_LIGHTS 128
#define MAX_SPOT_LIGHTS 16

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;


in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform sampler2D gEmissiveShine;


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

uniform LightData light_directional[8];
uniform LightData light_spot[MAX_SPOT_LIGHTS];
uniform LightData light_point[MAX_POINT_LIGHTS];

uniform vec3 viewPos;

uniform float treshold;

void main()
{

    vec3 FragPos = texture(gPosition, TexCoords).rgb;
    vec3 Normal = texture(gNormal, TexCoords).rgb;

    vec3 color = texture(gAlbedoSpec, TexCoords).rgb;
    vec3 specMap = texture(gAlbedoSpec, TexCoords).aaa;
    vec3 normal = normalize(Normal);
    int shininess = (int(texture(gEmissiveShine, TexCoords).a*128.0));

    vec3 lighting = vec3(0.0);
    vec3 viewDir = normalize(viewPos - FragPos);

    int shiness=int(texture(gEmissiveShine, TexCoords).a*128.0);
    vec3 emissive = texture(gEmissiveShine, TexCoords).rgb;

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
    lighting += emissive;
    FragColor = vec4(lighting, 1.0);

    float brightness = dot(lighting, vec3(0.2126, 0.7152, 0.0722));

    if(brightness > treshold)
    BrightColor = vec4(lighting, 1.0);
    else
    BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
}
