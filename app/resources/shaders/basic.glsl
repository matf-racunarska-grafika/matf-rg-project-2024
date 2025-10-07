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

out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

uniform sampler2D texture_diffuse1;

// jednostavno direkciono svetlo
uniform vec3 lightDir;   // smer svetla (ka objektu)
uniform vec3 lightColor; // boja svetla
uniform float ambient;   // ambientni intenzitet

void main() {
    vec3 base = texture(texture_diffuse1, TexCoords).rgb;

    vec3 norm = normalize(Normal);
    vec3 L = normalize(-lightDir);
    float diff = max(dot(norm, L), 0.0);

    vec3 color = base * (ambient + diff * lightColor);
    FragColor = vec4(color, 1.0);
}