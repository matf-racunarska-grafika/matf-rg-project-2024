//#shader vertex
#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aUV;

out vec2 vUV;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    vUV = aUV;
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}

//#shader fragment
#version 330 core

out vec4 FragColor;
in vec2 vUV;

uniform sampler2D uTex;

void main()
{
    vec4 c = texture(uTex, vUV);
    if (c.a < 0.01) discard;
    FragColor = c;
}


