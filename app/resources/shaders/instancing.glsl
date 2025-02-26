//#shader vertex
#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in mat4 aInstanceMatrix;

out vec2 TexCoords;
out vec3 Normal;
out vec3 FragPos;

uniform mat4 view;
uniform mat4 projection;

void main() {
    FragPos = vec3(aInstanceMatrix * vec4(aPos, 1.0));
    Normal = mat3(aInstanceMatrix) * aNormal;
    TexCoords = aTexCoords;
    gl_Position = projection * view * aInstanceMatrix * vec4(aPos, 1.0);
}

//#shader fragment
#version 330 core

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

uniform sampler2D texture_diffuse1;

uniform vec3 LightPos;
uniform vec3 LightColor;

uniform vec3 moonLightDir;
uniform vec3 moonLightColor;

uniform vec3 viewPos;

void main() {
    float specularStrength = 0.0;
    vec3 color = vec3(texture(texture_diffuse1, TexCoords).rgb);
    vec3 normal = normalize(Normal);

    vec3 ambient = 0.05 * color;

    vec3 lighting = vec3(0.0);
    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 lightDir = normalize(LightPos - FragPos);
    float lampDiffuse = max(dot(lightDir, normal), 0.0);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * LightColor;
    float distance = length(FragPos - LightPos);
    vec3 lampResult = (LightColor * lampDiffuse * color) / (distance * distance);

    vec3 moonDir = normalize(-moonLightDir);
    float moonDiffuse = max(dot(normal, moonDir), 0.0);
    vec3 moonReflectDir = reflect(-moonDir, normal);
    float moonSpecular = pow(max(dot(viewDir, moonReflectDir), 0.0), 8);
    vec3 moonSpecularComponent = specularStrength * moonSpecular * moonLightColor;
    vec3 moonResult = moonLightColor * moonDiffuse * color + moonSpecularComponent;

    vec3 res = ambient + specular + lampResult + moonResult;

    float brightness = dot(res, vec3(0.2126, 0.7152, 0.0722));
    if (brightness > 1.0)
    BrightColor = vec4(res, 1.0);
    else
    BrightColor = vec4(0.0, 0.0, 0.0, 1.0);

    FragColor = vec4(res, 1.0);

    if (texture(texture_diffuse1, TexCoords).a < 0.5)
    discard;
}
