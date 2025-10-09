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
uniform mat3 normalMatrix;

void main() {
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = normalize(normalMatrix * aNormal);
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

uniform vec3 dirLightDir;
uniform vec3 dirLightCol;
uniform vec3 dirLightAmb;

uniform vec3 viewPos;

void main() {
    vec3 col = texture(texture_diffuse1, TexCoords).rgb;
    vec3 norm = normalize(Normal);

    vec3 ambient = dirLightAmb * col;

    vec3 dir = normalize(-dirLightDir);
    float diff = max(dot(norm, dir), 0.0);
    vec3 diffuse = dirLightCol * diff * col;

    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-dir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    vec3 specular = dirLightCol * spec;

    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
}
