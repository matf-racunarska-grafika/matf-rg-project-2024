//#shader vertex
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    TexCoords = aTexCoords;
}

//#shader fragment
#version 330 core
in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D texture1;

void main() {
    FragColor = texture(texture1, TexCoords);
}