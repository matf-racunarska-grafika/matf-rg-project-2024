//#shader vertex
#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    TexCoords = aPos;
    // ukloni translaciju iz view matrice
    mat4 viewRotOnly = mat4(mat3(view));
    vec4 pos = projection * viewRotOnly * vec4(aPos, 1.0);
    gl_Position = pos.xyww; // z=1 da bi uvek bio "iza"
}

//#shader fragment
#version 330 core
in vec3 TexCoords;
out vec4 FragColor;

uniform samplerCube skybox;

void main()
{
    FragColor = texture(skybox, TexCoords);
}
