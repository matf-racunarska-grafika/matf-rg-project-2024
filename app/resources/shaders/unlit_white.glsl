//#shader vertex
#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;
out vec3 FragPos;
out vec3 Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    vec4 worldPos = model * vec4(aPos, 1.0);
    FragPos = worldPos.xyz;
    
    mat3 m3 = mat3(model[0].xyz, model[1].xyz, model[2].xyz);
    mat3 normalMatrix = transpose(inverse(m3));
    Normal = normalMatrix * aNormal;

    TexCoords = aTexCoords;
    gl_Position = projection * view * worldPos;
}

//#shader fragment
#version 330 core

out vec4 FragColor;

void main()
{
    FragColor = vec4(0.9, 0.9, 0.9, 1.0);
}