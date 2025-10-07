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

uniform sampler2D texture_diffuse1;

// Point light uniforms
uniform vec3 lightPos;    // pozicija svetla (npr. busStop)
uniform vec3 lightColor;  // boja svetla
uniform vec3 viewPos;     // pozicija kamere

void main()
{
    vec3 color = texture(texture_diffuse1, TexCoords).rgb;
    vec3 norm = normalize(Normal);

    // računanje udaljenosti od svetla i attenucije
    vec3 lightDir = normalize(lightPos - FragPos);
    float distance = length(lightPos - FragPos);
    float attenuation = 1.0 / (distance * distance); // klasična attenucija

    // Diffuse
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // Ambient
    vec3 ambient = 0.1 * lightColor;

    // Specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    vec3 specular = spec * lightColor;

    vec3 result = (ambient + diffuse + specular) * color * attenuation;
    FragColor = vec4(result, 1.0);
}
