//#shader vertex
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 3) in mat4 aInstanceMatrix;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    gl_Position = projection * view * aInstanceMatrix * vec4(aPos, 1.0);
}

//#shader fragment
#version 330 core
out vec4 FragColor;

void main()
{
     FragColor = vec4(1.0, 0.71, 0.76, 1.0);
}
