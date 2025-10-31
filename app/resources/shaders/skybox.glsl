//#shader vertex

#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 view;
uniform mat4 projection;

void main()
{
    TexCoords = aPos;
    vec4 pos = projection * view * vec4(aPos, 1.0);
    // Guramo z na far ravan uz čuvanje perspektive: z=w i w=w
    gl_Position = vec4(pos.xy, pos.w, pos.w);
}

//#shader fragment

#version 330 core
out vec4 FragColor;

in vec3 TexCoords;
uniform samplerCube skybox;

void main()
{
    FragColor = texture(skybox, TexCoords);
}


