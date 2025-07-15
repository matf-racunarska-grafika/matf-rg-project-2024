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
in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D groundTexture;
uniform vec3 tintColor;
uniform float tintStrength;

void main()
{
    vec3 texColor = texture(groundTexture, TexCoords).rgb;
    vec3 finalColor = mix(texColor, tintColor, tintStrength);
    FragColor = vec4(finalColor, 1.0);
}
